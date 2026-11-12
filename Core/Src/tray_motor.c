#include "tray_motor.h"
#include "Emm_V5.h"
#include "usart.h"
#include "app_runtime.h"
#include <math.h>
#define MOTOR_CHECK_BYTE 0x6BU
#define MOTOR_ENABLE_CMD 0xF3U
#define MOTOR_ZERO_CMD 0x0AU
#define MOTOR_POS_CMD 0xFDU
#define MOTOR_STOP_CMD 0xFEU
#define MOTOR_POS_READ_CMD 0x36U
#define MOTOR_POS_READ_RSP_LEN 8U
#define MOTOR_STATUS_READ_CMD 0x3AU
#define MOTOR_STATUS_RSP_LEN 4U
#define MOTOR_ACK_OK 0x02U
#define MOTOR_ACK_PARAM_ERROR 0xE2U
#define MOTOR_ACK_FRAME_ERROR 0xEEU

static bool origin_valid;
static double accumulated_target;
static float target_phase;
static double measured_pulse;
static const char *diagnostic = "TRAY ORIGIN INVALID";
static void Tray_ClearRxFrame(void);
static bool Tray_WaitRxFrame(uint32_t timeout_ms);
static bool Tray_FindFrame(uint8_t code, uint8_t len, uint8_t *offset);
static Tray_Result_t Tray_WaitCommandStatus(uint8_t code, uint32_t timeout_ms);
static bool Tray_ReadStatus(uint8_t *status);
static bool Tray_ReadPositionPulse(double *pulse);

void Tray_InvalidateOrigin(void) { origin_valid = false; }
bool Tray_IsOriginValid(void) { return origin_valid; }
const char *Tray_GetDiagnosticText(void) { return diagnostic; }
float Tray_GetAccumulatedAngleDeg(void)
{
  return (float)(measured_pulse * 360.0 / (TRAY_PULSE_PER_REV * TRAY_MOTOR_REVS_PER_TRAY_REV));
}
float Tray_GetAngleDeg(void)
{
  double phase = fmod((double)Tray_GetAccumulatedAngleDeg(), 360.0);
  if (phase < 0.0) { phase += 360.0; }
  return (float)phase;
}
static Tray_Result_t Tray_Abort(Tray_Result_t result)
{
  Tray_Stop();
  origin_valid = false;
  diagnostic = Tray_ResultText(result);
  return result;
}

Tray_Result_t Tray_Init(void)
{
  Tray_Result_t result;
  origin_valid = false;
  Tray_ClearRxFrame();
  Emm_V5_En_Control(TRAY_MOTOR_ADDR, true, false);
  result = Tray_WaitCommandStatus(MOTOR_ENABLE_CMD, TRAY_UART_TIMEOUT_MS);
  if (result != TRAY_RESULT_OK) { diagnostic = "TRAY ENABLE FAIL"; return result; }
  HAL_Delay(TRAY_CMD_GAP_MS);
  return Tray_SetZero();
}

Tray_Result_t Tray_SetZero(void)
{
  Tray_Result_t result;
  origin_valid = false;
  Tray_ClearRxFrame();
  Emm_V5_Reset_CurPos_To_Zero(TRAY_MOTOR_ADDR);
  result = Tray_WaitCommandStatus(MOTOR_ZERO_CMD, TRAY_UART_TIMEOUT_MS);
  if (result != TRAY_RESULT_OK) { diagnostic = "TRAY ZERO NOACK"; return result; }
  HAL_Delay(TRAY_CMD_GAP_MS);
  if (!Tray_ReadPositionPulse(&measured_pulse)) {
    diagnostic = "TRAY ZERO READ FAIL";
    return TRAY_RESULT_POSITION_READ_ERROR;
  }
  if (fabs(measured_pulse) > TRAY_POS_TOLERANCE_PULSE) {
    diagnostic = "TRAY ZERO OFFSET";
    return TRAY_RESULT_ORIGIN_ERROR;
  }
  accumulated_target = 0.0;
  target_phase = 0.0f;
  origin_valid = true;
  diagnostic = "OK";
  return TRAY_RESULT_OK;
}

Tray_Result_t Tray_MoveToAngle(float angle_deg)
{
  double delta, next, pulse_target, previous_pulse, travel;
  uint32_t pulse, start, timeout;
  Tray_Result_t result;
  bool feedback = false;
  if (!(angle_deg >= 0.0f && angle_deg <= 360.0f)) { return TRAY_RESULT_INVALID_ANGLE; }
  if (!origin_valid) { return TRAY_RESULT_ORIGIN_ERROR; }
  if (!(TRAY_MOTOR_REVS_PER_TRAY_REV > 0.0)) { return Tray_Abort(TRAY_RESULT_OVERFLOW); }
  if (AppRuntime_Expired()) { return Tray_Abort(TRAY_RESULT_TIMEOUT); }
  if (!Tray_ReadPositionPulse(&measured_pulse)) { return Tray_Abort(TRAY_RESULT_POSITION_READ_ERROR); }
  if (angle_deg == 360.0f) { angle_deg = 0.0f; }
  delta = (double)angle_deg - target_phase;
  if (delta < 0.0) { delta += 360.0; }
  previous_pulse = floor(accumulated_target * TRAY_PULSE_PER_REV * TRAY_MOTOR_REVS_PER_TRAY_REV / 360.0 + 0.5);
  if (fabs(measured_pulse - previous_pulse) > TRAY_POS_TOLERANCE_PULSE) {
    return Tray_Abort(TRAY_RESULT_POSITION_MISMATCH);
  }
  {
    uint8_t status;
    if (!Tray_ReadStatus(&status)) { return Tray_Abort(TRAY_RESULT_POSITION_READ_ERROR); }
    if ((status & 0x0CU) != 0U) { return Tray_Abort(TRAY_RESULT_MOTOR_FAULT); }
  }
  if (delta == 0.0) { return TRAY_RESULT_OK; }
  next = accumulated_target + delta;
  pulse_target = floor(next * TRAY_PULSE_PER_REV * TRAY_MOTOR_REVS_PER_TRAY_REV / 360.0 + 0.5);
  /* Signed feedback has only 31 magnitude bits. Check both wire formats. */
  if (!(pulse_target >= 0.0 && pulse_target <= 4294967295.0) ||
      pulse_target * TRAY_POS_UNITS_PER_REV / TRAY_PULSE_PER_REV > 2147483647.0) {
    return Tray_Abort(TRAY_RESULT_OVERFLOW);
  }
  if (measured_pulse > pulse_target) { return Tray_Abort(TRAY_RESULT_POSITION_MISMATCH); }
  pulse = (uint32_t)pulse_target;
  travel = pulse_target - measured_pulse;
  timeout = (uint32_t)(travel * 60000.0 / (TRAY_PULSE_PER_REV * TRAY_DEFAULT_RPM) + 0.5)
            + 2U * ((TRAY_DEFAULT_RPM * (256U - TRAY_DEFAULT_ACC) * 50U + 999U) / 1000U)
            + TRAY_MOVE_TIME_MARGIN_MS;
  if (AppRuntime_Expired()) { return Tray_Abort(TRAY_RESULT_TIMEOUT); }
  Tray_ClearRxFrame();
  Emm_V5_Pos_Control(TRAY_MOTOR_ADDR, TRAY_DIR_POSITIVE, TRAY_DEFAULT_RPM,
                     TRAY_DEFAULT_ACC, pulse, true, false);
  result = Tray_WaitCommandStatus(MOTOR_POS_CMD, TRAY_UART_TIMEOUT_MS);
  if (result != TRAY_RESULT_OK) { return Tray_Abort(result); }
  start = HAL_GetTick();
  while ((uint32_t)(HAL_GetTick() - start) < timeout && !AppRuntime_Expired()) {
    uint8_t status = 0U;
    bool status_ok = Tray_ReadStatus(&status);
    bool pos_ok;
    if (status_ok && (status & 0x0CU) != 0U) { return Tray_Abort(TRAY_RESULT_MOTOR_FAULT); }
    pos_ok = Tray_ReadPositionPulse(&measured_pulse);
    if (status_ok && pos_ok) {
      feedback = true;
      if ((status & 0x02U) != 0U && fabs(measured_pulse - pulse_target) <= TRAY_POS_TOLERANCE_PULSE) {
        accumulated_target = next;
        target_phase = angle_deg;
        diagnostic = "OK";
        return TRAY_RESULT_OK;
      }
    }
    if (!AppRuntime_Wait(TRAY_MOVE_UPDATE_MS)) { break; }
  }
  return Tray_Abort(AppRuntime_Expired() || feedback ? TRAY_RESULT_TIMEOUT : TRAY_RESULT_POSITION_READ_ERROR);
}

void Tray_Stop(void)
{
  Tray_ClearRxFrame();
  Emm_V5_Stop_Now(TRAY_MOTOR_ADDR, false);
  (void)Tray_WaitCommandStatus(MOTOR_STOP_CMD, TRAY_UART_TIMEOUT_MS);
}

const char *Tray_ResultText(Tray_Result_t result)
{
  switch (result) {
    case TRAY_RESULT_OK: return "OK";
    case TRAY_RESULT_TIMEOUT: return "TRAY TIMEOUT";
    case TRAY_RESULT_CMD_ERROR: return "TRAY CMD ERR";
    case TRAY_RESULT_MOTOR_FAULT: return "TRAY STALL";
    case TRAY_RESULT_POSITION_READ_ERROR: return "TRAY POS READ";
    case TRAY_RESULT_POSITION_MISMATCH: return "TRAY POS MISMATCH";
    case TRAY_RESULT_INVALID_ANGLE: return "TRAY ANGLE RANGE";
    case TRAY_RESULT_ORIGIN_ERROR: return "TRAY ORIGIN INVALID";
    case TRAY_RESULT_OVERFLOW: return "TRAY OVERFLOW";
    default: return "TRAY UNKNOWN";
  }
}

static void Tray_ClearRxFrame(void)
{
  UART_RxQueueReset();
}

static bool Tray_WaitRxFrame(uint32_t timeout_ms)
{
  return UART_RxPopFrame(timeout_ms);
}

static bool Tray_FindFrame(uint8_t code, uint8_t frame_len, uint8_t *offset)
{
  uint16_t i;

  for (i = 0U; i + frame_len <= rxCount; i++)
  {
    if (rxCmd[i] == TRAY_MOTOR_ADDR &&
        rxCmd[i + 1U] == code &&
        rxCmd[i + frame_len - 1U] == MOTOR_CHECK_BYTE)
    {
      *offset = (uint8_t)i;
      return true;
    }
  }

  return false;
}

static Tray_Result_t Tray_WaitCommandStatus(uint8_t code, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;
  uint8_t offset;

  while ((elapsed = HAL_GetTick() - start) < timeout_ms)
  {
    if (!Tray_WaitRxFrame(timeout_ms - elapsed))
    {
      break;
    }
    if (!Tray_FindFrame(code, 4U, &offset))
    {
      continue;
    }
    if (rxCmd[offset + 2U] == MOTOR_ACK_OK)
    {
      return TRAY_RESULT_OK;
    }
    if (rxCmd[offset + 2U] == MOTOR_ACK_PARAM_ERROR ||
        rxCmd[offset + 2U] == MOTOR_ACK_FRAME_ERROR)
    {
      return TRAY_RESULT_CMD_ERROR;
    }
  }

  return TRAY_RESULT_TIMEOUT;
}

static bool Tray_ReadStatus(uint8_t *status)
{
  uint32_t start;
  uint32_t elapsed;
  uint8_t offset;

  Tray_ClearRxFrame();
  Emm_V5_Read_Sys_Params(TRAY_MOTOR_ADDR, S_FLAG);
  start = HAL_GetTick();
  while ((elapsed = HAL_GetTick() - start) < TRAY_POS_READ_TIMEOUT_MS)
  {
    if (!Tray_WaitRxFrame(TRAY_POS_READ_TIMEOUT_MS - elapsed))
    {
      break;
    }
    if (Tray_FindFrame(MOTOR_STATUS_READ_CMD, MOTOR_STATUS_RSP_LEN, &offset))
    {
      *status = rxCmd[offset + 2U];
      return true;
    }
  }

  return false;
}

static bool Tray_ReadPositionPulse(double *position_pulse)
{
  uint32_t value;
  int32_t position_units;
  uint32_t start;
  uint32_t elapsed;
  uint8_t offset;

  Tray_ClearRxFrame();
  Emm_V5_Read_Sys_Params(TRAY_MOTOR_ADDR, S_CPOS);
  start = HAL_GetTick();
  while ((elapsed = HAL_GetTick() - start) < TRAY_POS_READ_TIMEOUT_MS)
  {
    if (!Tray_WaitRxFrame(TRAY_POS_READ_TIMEOUT_MS - elapsed))
    {
      break;
    }
    if (Tray_FindFrame(MOTOR_POS_READ_CMD, MOTOR_POS_READ_RSP_LEN, &offset))
    {
      value = ((uint32_t)rxCmd[offset + 3U] << 24) |
              ((uint32_t)rxCmd[offset + 4U] << 16) |
              ((uint32_t)rxCmd[offset + 5U] << 8) |
              ((uint32_t)rxCmd[offset + 6U]);
      if (value > 0x7FFFFFFFUL || rxCmd[offset + 2U] > 1U) { return false; }
      position_units = rxCmd[offset + 2U] != 0U ? -(int32_t)value : (int32_t)value;
      *position_pulse = (double)position_units * TRAY_PULSE_PER_REV / TRAY_POS_UNITS_PER_REV;
      if (TRAY_DIR_POSITIVE != 0U) { *position_pulse = -*position_pulse; }
      return true;
    }
  }

  return false;
}
