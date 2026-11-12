/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    xy_stage.c
  * @brief   CoreXY kinematics and synchronized serial point-to-point motion.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "xy_stage.h"
#include "usart.h"
#include "Emm_V5.h"
#include "app_runtime.h"

#define MOTOR_CHECK_BYTE              0x6BU
#define MOTOR_BROADCAST_ADDR          0U
#define MOTOR_ZERO_CMD                0x0AU
#define MOTOR_POS_READ_CMD            0x36U
#define MOTOR_POS_READ_RSP_LEN        8U
#define MOTOR_STATUS_READ_CMD         0x3AU
#define MOTOR_STATUS_RSP_LEN          4U
#define MOTOR_POS_CMD                 0xFDU
#define MOTOR_SYNC_CMD                0xFFU
#define MOTOR_ACK_OK                  0x02U
#define MOTOR_ACK_PARAM_ERROR         0xE2U
#define MOTOR_ACK_FRAME_ERROR         0xEEU
#define MOTOR_STATUS_ARRIVED_BIT      0x02U
#define MOTOR_STATUS_STALL_BIT        0x04U
#define MOTOR_STATUS_STALL_PROTECT_BIT 0x08U
#define MOTOR_ACC_STEP_US             50U

static float current_x_mm = 0.0f;
static float current_y_mm = 0.0f;
static float target_x_mm, target_y_mm;
/* Signed logical belt coordinates, not individual Cartesian axes. */
static float current_m1_pulses = 0.0f;
static float current_m2_pulses = 0.0f;
static bool origin_valid = false;
static XY_Result_t origin_result = XY_RESULT_ORIGIN_ERROR;
static const char *diagnostic_text = "ORIGIN INVALID";
static XY_PositionCallback_t position_callback = 0;

static bool XY_IsCoordinateInRange(float x_mm, float y_mm);
static int32_t XY_RoundPulse(float pulses);
static uint32_t XY_PulseMagnitude(int32_t pulses);
static void XY_StageToMotorPulses(float x_mm, float y_mm, int32_t *m1, int32_t *m2);
static uint16_t XY_MotorRpm(uint32_t travel, uint32_t max_travel);
static uint32_t XY_EstimateMoveTimeMs(uint32_t pulses, uint16_t rpm);
static XY_Result_t XY_WaitMoveComplete(uint32_t move_time_ms, int32_t target_m1,
                                       int32_t target_m2, bool m1_moved, bool m2_moved);
static XY_Result_t XY_AbortMove(XY_Result_t result);
static float XY_Abs(float value);
static void XY_ClearRxFrame(void);
static void XY_ReportPosition(void);
static XY_Result_t XY_WaitCommandStatus(uint8_t addr, uint8_t code, uint32_t timeout_ms);
static bool XY_FindFrame(uint8_t addr, uint8_t code, uint8_t frame_len, uint8_t *offset);
static bool XY_PositionNearTarget(int32_t target_m1, int32_t target_m2);
static XY_Result_t Motor_ResetZeroRetry(uint8_t addr);
static bool Motor_ReadStatus(uint8_t addr, uint8_t *status);
static bool Motor_ReadPositionUnits(uint8_t addr, int32_t *pos_units);
static bool Motor_ReadPositionUnitsRetry(uint8_t addr, int32_t *pos_units);

XY_Result_t XY_Init(void)
{
  XY_Result_t result;
  origin_valid = false;
  origin_result = XY_RESULT_ORIGIN_ERROR;
  diagnostic_text = "ORIGIN INVALID";
  HAL_Delay(XY_MOTOR_READY_DELAY_MS);
  XY_ClearRxFrame();
  Emm_V5_En_Control(XY_M1_ADDR, true, false);
  result = XY_WaitCommandStatus(XY_M1_ADDR, 0xF3U, XY_UART_TIMEOUT_MS);
  if (result != XY_RESULT_OK) { diagnostic_text = "M1 ENABLE FAIL"; return result; }
  HAL_Delay(XY_CMD_GAP_MS);
  XY_ClearRxFrame();
  Emm_V5_En_Control(XY_M2_ADDR, true, false);
  result = XY_WaitCommandStatus(XY_M2_ADDR, 0xF3U, XY_UART_TIMEOUT_MS);
  if (result != XY_RESULT_OK) { diagnostic_text = "M2 ENABLE FAIL"; return result; }
  HAL_Delay(100U);
  return XY_RESULT_OK;
}

XY_Result_t XY_SetZero(void)
{
  XY_Result_t result;
  uint8_t retry;

  /* Only call while stationary at the intended physical origin. Retrying a
   * lost zero acknowledgement must never bypass confirmation from BOTH motors.
   */
  origin_valid = false;
  origin_result = XY_RESULT_ORIGIN_ERROR;
  result = Motor_ResetZeroRetry(XY_M1_ADDR);
  if (result != XY_RESULT_OK) {
    diagnostic_text = result == XY_RESULT_TIMEOUT ? "M1 ZERO NOACK" : "M1 ZERO REJECT";
    return origin_result;
  }
  HAL_Delay(XY_CMD_GAP_MS);
  result = Motor_ResetZeroRetry(XY_M2_ADDR);
  if (result != XY_RESULT_OK) {
    diagnostic_text = result == XY_RESULT_TIMEOUT ? "M2 ZERO NOACK" : "M2 ZERO REJECT";
    return origin_result;
  }

  /* Give the controller bounded time to publish its reset position. */
  for (retry = 0U; retry < XY_ZERO_RETRY; retry++) {
    HAL_Delay(XY_ZERO_RETRY_GAP_MS);
    if (!XY_UpdatePositionFromMotors()) {
      origin_result = XY_RESULT_POSITION_READ_ERROR;
      continue;
    }
    if (XY_PositionNearTarget(0, 0)) {
      target_x_mm = target_y_mm = 0.0f;
      origin_valid = true;
      origin_result = XY_RESULT_OK;
      diagnostic_text = "OK";
      return origin_result;
    }
    origin_result = XY_RESULT_ORIGIN_ERROR;
    diagnostic_text = XY_Abs(current_m1_pulses) > XY_POS_TOLERANCE_PULSE ?
                      "M1 ZERO OFFSET" : "M2 ZERO OFFSET";
  }
  return origin_result;
}

bool XY_IsOriginValid(void) { return origin_valid; }
void XY_InvalidateOrigin(void) { origin_valid = false; origin_result = XY_RESULT_ORIGIN_ERROR; }

const char *XY_GetDiagnosticText(void)
{
  return diagnostic_text;
}

XY_Result_t XY_MoveTo(float x_mm, float y_mm)
{
  int32_t target_m1;
  int32_t target_m2;
  uint32_t travel_m1;
  uint32_t travel_m2;
  uint32_t max_travel;
  uint32_t time_m1;
  uint32_t time_m2;
  uint16_t rpm_m1;
  uint16_t rpm_m2;
  uint8_t dir_m1;
  uint8_t dir_m2;
  XY_Result_t result;

  /* Comparisons also reject NaN and infinity, before any float-to-int cast. */
  if (!XY_IsCoordinateInRange(x_mm, y_mm)) {
    return XY_RESULT_OUT_OF_RANGE;
  }
  if (!origin_valid) {
    return origin_result;
  }
  if (!XY_UpdatePositionFromMotors()) {
    return XY_AbortMove(XY_RESULT_POSITION_READ_ERROR);
  }

  if (AppRuntime_Expired()) { return XY_AbortMove(XY_RESULT_TIMEOUT); }
  target_x_mm = x_mm; target_y_mm = y_mm;
  XY_StageToMotorPulses(x_mm, y_mm, &target_m1, &target_m2);
  travel_m1 = XY_PulseMagnitude(XY_RoundPulse((float)target_m1 - current_m1_pulses));
  travel_m2 = XY_PulseMagnitude(XY_RoundPulse((float)target_m2 - current_m2_pulses));
  max_travel = travel_m1 > travel_m2 ? travel_m1 : travel_m2;
  if (max_travel == 0U) {
    return XY_RESULT_OK;
  }

  rpm_m1 = XY_MotorRpm(travel_m1, max_travel);
  rpm_m2 = XY_MotorRpm(travel_m2, max_travel);
  time_m1 = XY_EstimateMoveTimeMs(travel_m1, rpm_m1);
  time_m2 = XY_EstimateMoveTimeMs(travel_m2, rpm_m2);
  /* Absolute commands use the sign of the TARGET, not the travel delta.
   * M2 can be negative even though Cartesian X and Y are both nonnegative.
   */
  dir_m1 = target_m1 < 0 ? (uint8_t)(XY_M1_DIR_POSITIVE ^ 1U) : XY_M1_DIR_POSITIVE;
  dir_m2 = target_m2 < 0 ? (uint8_t)(XY_M2_DIR_POSITIVE ^ 1U) : XY_M2_DIR_POSITIVE;

  /* Queue BOTH absolute targets, even on a 45-degree move (one motor holds).
   * This also overwrites previous pending position targets before a trigger.
   */
  if (AppRuntime_Expired()) { return XY_AbortMove(XY_RESULT_TIMEOUT); }
  XY_ClearRxFrame();
  Emm_V5_Pos_Control(XY_M1_ADDR, dir_m1, rpm_m1, XY_DEFAULT_ACC,
                     XY_PulseMagnitude(target_m1), true, true);
  result = XY_WaitCommandStatus(XY_M1_ADDR, MOTOR_POS_CMD, XY_UART_TIMEOUT_MS);
  if (result != XY_RESULT_OK) {
    return XY_AbortMove(result);
  }
  HAL_Delay(XY_CMD_GAP_MS);
  if (AppRuntime_Expired()) { return XY_AbortMove(XY_RESULT_TIMEOUT); }
  XY_ClearRxFrame();
  Emm_V5_Pos_Control(XY_M2_ADDR, dir_m2, rpm_m2, XY_DEFAULT_ACC,
                     XY_PulseMagnitude(target_m2), true, true);
  result = XY_WaitCommandStatus(XY_M2_ADDR, MOTOR_POS_CMD, XY_UART_TIMEOUT_MS);
  if (result != XY_RESULT_OK) {
    return XY_AbortMove(result);
  }
  HAL_Delay(XY_CMD_GAP_MS);
  XY_ClearRxFrame();
  if (AppRuntime_Expired()) { return XY_AbortMove(XY_RESULT_TIMEOUT); }
  Emm_V5_Synchronous_motion(MOTOR_BROADCAST_ADDR);
  /* Retain the existing bus convention: address 1 acknowledges broadcast. */
  result = XY_WaitCommandStatus(XY_M1_ADDR, MOTOR_SYNC_CMD, XY_UART_TIMEOUT_MS);
  if (result != XY_RESULT_OK) {
    return XY_AbortMove(result);
  }

  result = XY_WaitMoveComplete(time_m1 > time_m2 ? time_m1 : time_m2,
                               target_m1, target_m2, travel_m1 != 0U, travel_m2 != 0U);
  if (result != XY_RESULT_OK) {
    return XY_AbortMove(result);
  }
  return XY_RESULT_OK;
}

XY_Result_t XY_MoveRelative(float dx_mm, float dy_mm)
{
  /* Read both motors before resolving a relative target; no stale-axis mix. */
  if (!origin_valid) {
    return origin_result;
  }
  if (!XY_UpdatePositionFromMotors()) {
    return XY_AbortMove(XY_RESULT_POSITION_READ_ERROR);
  }
  return XY_MoveTo(current_x_mm + dx_mm, current_y_mm + dy_mm);
}

void XY_Stop(void)
{
  XY_ClearRxFrame();
  Emm_V5_Stop_Now(XY_M1_ADDR, false);
  (void)UART_RxPopFrame(XY_UART_TIMEOUT_MS);
  HAL_Delay(XY_CMD_GAP_MS);
  XY_ClearRxFrame();
  Emm_V5_Stop_Now(XY_M2_ADDR, false);
  (void)UART_RxPopFrame(XY_UART_TIMEOUT_MS);
}

bool XY_UpdatePositionFromMotors(void)
{
  int32_t m1_units;
  int32_t m2_units;
  float m1_pulses;
  float m2_pulses;

  if (!Motor_ReadPositionUnitsRetry(XY_M1_ADDR, &m1_units)) {
    diagnostic_text = "M1 POS FAIL";
    return false;
  }
  HAL_Delay(XY_POS_READ_GAP_MS);
  if (!Motor_ReadPositionUnitsRetry(XY_M2_ADDR, &m2_units)) {
    diagnostic_text = "M2 POS FAIL";
    return false;
  }

  m1_pulses = (float)m1_units * ((float)XY_PULSE_PER_REV / XY_POS_UNITS_PER_REV);
  m2_pulses = (float)m2_units * ((float)XY_PULSE_PER_REV / XY_POS_UNITS_PER_REV);
  if (XY_M1_DIR_POSITIVE != 0U) {
    m1_pulses = -m1_pulses;
  }
  if (XY_M2_DIR_POSITIVE != 0U) {
    m2_pulses = -m2_pulses;
  }

  /* Commit as a pair: X=(M1+M2)/2, Y=(M1-M2)/2. Readings are sequential,
   * not a simultaneous encoder snapshot; live display is approximate in motion.
   */
  current_m1_pulses = m1_pulses;
  current_m2_pulses = m2_pulses;
  current_x_mm = (m1_pulses + m2_pulses) / (2.0f * XY_PULSE_PER_MM);
  current_y_mm = (m1_pulses - m2_pulses) / (2.0f * XY_PULSE_PER_MM);
  XY_ReportPosition();
  return true;
}

float XY_GetX(void)
{
  return current_x_mm;
}

float XY_GetY(void)
{
  return current_y_mm;
}

void XY_SetPositionCallback(XY_PositionCallback_t callback)
{
  position_callback = callback;
}

static bool XY_IsCoordinateInRange(float x_mm, float y_mm)
{
  return x_mm >= 0.0f && x_mm <= XY_STAGE_X_MAX_MM &&
         y_mm >= 0.0f && y_mm <= XY_STAGE_Y_MAX_MM;
}

static int32_t XY_RoundPulse(float pulses)
{
  return (int32_t)(pulses >= 0.0f ? pulses + 0.5f : pulses - 0.5f);
}

static uint32_t XY_PulseMagnitude(int32_t pulses)
{
  return (uint32_t)(pulses < 0 ? -(int64_t)pulses : pulses);
}

static void XY_StageToMotorPulses(float x_mm, float y_mm, int32_t *m1, int32_t *m2)
{
  *m1 = XY_RoundPulse((x_mm + y_mm) * XY_PULSE_PER_MM);
  *m2 = XY_RoundPulse((x_mm - y_mm) * XY_PULSE_PER_MM);
}

static uint16_t XY_MotorRpm(uint32_t travel, uint32_t max_travel)
{
  uint32_t rpm = (uint32_t)(((uint64_t)travel * XY_DEFAULT_RPM + max_travel / 2U) / max_travel);

  /* A holding motor still receives a valid nonzero RPM in its absolute command. */
  if (rpm < XY_MIN_RPM) {
    rpm = XY_MIN_RPM;
  }
  return (uint16_t)rpm;
}

static uint32_t XY_EstimateMoveTimeMs(uint32_t pulses, uint16_t rpm)
{
  uint64_t numerator;
  uint32_t denominator;
  uint32_t time_ms;
  uint32_t accel_time_ms = 0U;

  if (pulses == 0U) {
    return 0U;
  }
  numerator = (uint64_t)pulses * 60000ULL;
  denominator = XY_PULSE_PER_REV * (uint32_t)rpm;
  time_ms = (uint32_t)((numerator + denominator - 1U) / denominator);
  if (XY_DEFAULT_ACC != 0U) {
    numerator = (uint64_t)rpm * (256U - XY_DEFAULT_ACC) * MOTOR_ACC_STEP_US;
    accel_time_ms = (uint32_t)((numerator + 999ULL) / 1000ULL);
  }
  /* Full-distance cruise plus both ramps is a conservative upper bound.
   * Evaluate BOTH motors: quantized RPM can make the shorter move take longer.
   */
  time_ms += 2U * accel_time_ms;
  return time_ms < XY_MOVE_UPDATE_MS ? XY_MOVE_UPDATE_MS : time_ms;
}

static XY_Result_t XY_WaitMoveComplete(uint32_t move_time_ms, int32_t target_m1,
                                       int32_t target_m2, bool m1_moved, bool m2_moved)
{
  uint32_t start = HAL_GetTick();
  uint32_t timeout = move_time_ms + XY_MOVE_TIME_MARGIN_MS;
  bool complete_feedback = false;

  while ((HAL_GetTick() - start) < timeout) {
    uint8_t m1_status = 0U;
    if (AppRuntime_Expired()) { return XY_RESULT_TIMEOUT; }
    uint8_t m2_status = 0U;
    bool m1_ok = Motor_ReadStatus(XY_M1_ADDR, &m1_status);
    bool m2_ok;
    bool position_ok;

    HAL_Delay(XY_CMD_GAP_MS);
    m2_ok = Motor_ReadStatus(XY_M2_ADDR, &m2_status);
    /* Monitor BOTH motors, including the holding motor on diagonal moves. */
    if ((m1_ok && (m1_status & (MOTOR_STATUS_STALL_BIT | MOTOR_STATUS_STALL_PROTECT_BIT)) != 0U) ||
        (m2_ok && (m2_status & (MOTOR_STATUS_STALL_BIT | MOTOR_STATUS_STALL_PROTECT_BIT)) != 0U)) {
      return XY_RESULT_MOTOR_FAULT;
    }

    HAL_Delay(XY_CMD_GAP_MS);
    position_ok = XY_UpdatePositionFromMotors();
    if (m1_ok && m2_ok && position_ok) {
      complete_feedback = true;
      /* Fresh status AND coupled position must agree. An old arrival event,
       * or just one Cartesian coordinate being correct, cannot complete a move.
       */
      if ((!m1_moved || (m1_status & MOTOR_STATUS_ARRIVED_BIT) != 0U) &&
          (!m2_moved || (m2_status & MOTOR_STATUS_ARRIVED_BIT) != 0U) &&
          XY_PositionNearTarget(target_m1, target_m2)) {
        return XY_RESULT_OK;
      }
    }
    HAL_Delay(XY_MOVE_UPDATE_MS);
  }
  if (!complete_feedback) {
    diagnostic_text = "MOVE FB FAIL";
  }
  return complete_feedback ? XY_RESULT_TIMEOUT : XY_RESULT_POSITION_READ_ERROR;
}

static XY_Result_t XY_AbortMove(XY_Result_t result)
{
  XY_Stop();
  /* A failed coupled move must not be followed by another blind move. */
  origin_valid = false;
  origin_result = XY_RESULT_ORIGIN_ERROR;
  if (result != XY_RESULT_POSITION_READ_ERROR) {
    diagnostic_text = XY_ResultText(result);
  }
  return result;
}

static float XY_Abs(float value)
{
  return value >= 0.0f ? value : -value;
}

static void XY_ClearRxFrame(void)
{
  UART_RxQueueReset();
}

static void XY_ReportPosition(void)
{
  if (position_callback != 0) {
    position_callback(current_x_mm, current_y_mm);
  }
}

static XY_Result_t XY_WaitCommandStatus(uint8_t addr, uint8_t code, uint32_t timeout_ms)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;
  uint8_t offset;

  while ((elapsed = HAL_GetTick() - start) < timeout_ms) {
    if (!UART_RxPopFrame(timeout_ms - elapsed)) {
      break;
    }
    if (!XY_FindFrame(addr, code, 4U, &offset)) {
      continue;
    }
    if (rxCmd[offset + 2U] == MOTOR_ACK_OK) {
      return XY_RESULT_OK;
    }
    if (rxCmd[offset + 2U] == MOTOR_ACK_PARAM_ERROR ||
        rxCmd[offset + 2U] == MOTOR_ACK_FRAME_ERROR) {
      return XY_RESULT_CMD_ERROR;
    }
    /* 0x9F arrival is NOT a queued-command acceptance acknowledgement. */
  }
  return XY_RESULT_TIMEOUT;
}

static bool XY_FindFrame(uint8_t addr, uint8_t code, uint8_t frame_len, uint8_t *offset)
{
  uint16_t i;

  for (i = 0U; i + frame_len <= rxCount; i++) {
    if (rxCmd[i] == addr && rxCmd[i + 1U] == code &&
        rxCmd[i + frame_len - 1U] == MOTOR_CHECK_BYTE) {
      *offset = (uint8_t)i;
      return true;
    }
  }
  return false;
}

static bool XY_PositionNearTarget(int32_t target_m1, int32_t target_m2)
{
  return XY_Abs(current_m1_pulses - (float)target_m1) <= XY_POS_TOLERANCE_PULSE &&
         XY_Abs(current_m2_pulses - (float)target_m2) <= XY_POS_TOLERANCE_PULSE;
}

static XY_Result_t Motor_ResetZeroRetry(uint8_t addr)
{
  uint8_t retry;
  XY_Result_t result = XY_RESULT_TIMEOUT;

  for (retry = 0U; retry < XY_ZERO_RETRY; retry++) {
    XY_ClearRxFrame();
    Emm_V5_Reset_CurPos_To_Zero(addr);
    result = XY_WaitCommandStatus(addr, MOTOR_ZERO_CMD, XY_UART_TIMEOUT_MS);
    if (result != XY_RESULT_TIMEOUT) {
      /* Explicit rejection is not a transient missing acknowledgement. */
      return result;
    }
    HAL_Delay(XY_ZERO_RETRY_GAP_MS);
  }
  return result;
}

static bool Motor_ReadStatus(uint8_t addr, uint8_t *status)
{
  uint32_t start;
  uint32_t elapsed;
  uint8_t offset;

  XY_ClearRxFrame();
  Emm_V5_Read_Sys_Params(addr, S_FLAG);
  start = HAL_GetTick();
  while ((elapsed = HAL_GetTick() - start) < XY_POS_READ_TIMEOUT_MS) {
    if (!UART_RxPopFrame(XY_POS_READ_TIMEOUT_MS - elapsed)) {
      break;
    }
    if (XY_FindFrame(addr, MOTOR_STATUS_READ_CMD, MOTOR_STATUS_RSP_LEN, &offset)) {
      *status = rxCmd[offset + 2U];
      return true;
    }
  }
  return false;
}

static bool Motor_ReadPositionUnits(uint8_t addr, int32_t *pos_units)
{
  uint32_t value;
  uint32_t start;
  uint32_t elapsed;
  uint8_t offset;

  XY_ClearRxFrame();
  Emm_V5_Read_Sys_Params(addr, S_CPOS);
  start = HAL_GetTick();
  while ((elapsed = HAL_GetTick() - start) < XY_POS_READ_TIMEOUT_MS) {
    if (!UART_RxPopFrame(XY_POS_READ_TIMEOUT_MS - elapsed)) {
      break;
    }
    if (XY_FindFrame(addr, MOTOR_POS_READ_CMD, MOTOR_POS_READ_RSP_LEN, &offset)) {
      value = ((uint32_t)rxCmd[offset + 3U] << 24) |
              ((uint32_t)rxCmd[offset + 4U] << 16) |
              ((uint32_t)rxCmd[offset + 5U] << 8) |
              ((uint32_t)rxCmd[offset + 6U]);
      /* Travel is small; reject corrupt magnitudes rather than signed overflow. */
      if (value > 0x7FFFFFFFUL || rxCmd[offset + 2U] > 1U) {
        return false;
      }
      *pos_units = rxCmd[offset + 2U] != 0U ? -(int32_t)value : (int32_t)value;
      return true;
    }
  }
  return false;
}

static bool Motor_ReadPositionUnitsRetry(uint8_t addr, int32_t *pos_units)
{
  uint8_t retry;

  for (retry = 0U; retry < XY_POS_READ_RETRY; retry++) {
    if (Motor_ReadPositionUnits(addr, pos_units)) {
      return true;
    }
    HAL_Delay(XY_POS_READ_GAP_MS);
  }
  return false;
}

float XY_GetTargetX(void) { return target_x_mm; }
float XY_GetTargetY(void) { return target_y_mm; }
const char *XY_ResultText(XY_Result_t result)
{
  switch (result) {
    case XY_RESULT_OK: return "OK";
    case XY_RESULT_TIMEOUT: return "XY TIMEOUT";
    case XY_RESULT_CMD_ERROR: return "XY CMD ERROR";
    case XY_RESULT_MOTOR_FAULT: return "XY MOTOR STALL";
    case XY_RESULT_POSITION_READ_ERROR: return XY_GetDiagnosticText();
    case XY_RESULT_OUT_OF_RANGE: return "XY RANGE ERROR";
    case XY_RESULT_ORIGIN_ERROR: return XY_GetDiagnosticText();
    default: return "XY UNKNOWN ERROR";
  }
}
