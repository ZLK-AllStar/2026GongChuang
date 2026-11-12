/* Runs the actual application C against a deterministic serial-motor mock.
 * No hardware peripheral, CubeMX file or driver configuration is modified.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../../Core/Src/xy_stage.c"

__IO bool rxFrameFlag;
__IO uint8_t rxCmd[CMD_LEN];
__IO uint8_t rxCount;
__IO uint8_t rxDmaBuffer[CMD_LEN];
UART_HandleTypeDef huart1;

static unsigned checks;
#define CHECK(expr) do { checks++; if (!(expr)) { printf("FAIL line %d: %s\n", __LINE__, #expr); exit(1); } } while (0)

typedef struct {
  uint8_t addr, dir, acc;
  uint16_t rpm;
  uint32_t pulses;
  bool absolute, sync;
} Command;

static Command commands[2];
static uint8_t reply[8];
static uint8_t reply_len;
static int32_t raw_units[3];
static uint32_t ticks;
static unsigned pos_commands, sync_commands, stop_commands, reports;
static uint8_t fail_position_addr, fail_command_addr, fail_zero_addr, stall_addr;
static bool stuck, fail_sync, arrival_only, fail_position_after_sync, no_arrival;
static uint8_t reject_zero_addr, ignore_zero_addr, drop_zero_ack_addr;
static unsigned drop_zero_ack_count, zero_attempts[3];

bool AppRuntime_Expired(void) { return false; }
uint32_t HAL_GetTick(void) { return ticks++; }
void HAL_Delay(uint32_t delay) { ticks += delay; }
void UART_RxQueueReset(void) { reply_len = 0U; rxCount = 0U; }
bool UART_RxPopFrame(uint32_t timeout_ms)
{
  unsigned i;
  if (reply_len == 0U) { ticks += timeout_ms; return false; }
  rxCount = reply_len;
  for (i = 0U; i < reply_len; i++) { rxCmd[i] = reply[i]; }
  reply_len = 0U;
  return true;
}
static void Reply(uint8_t addr, uint8_t cmd, uint8_t status)
{
  reply[0] = addr; reply[1] = cmd; reply[2] = status; reply[3] = 0x6B;
  reply_len = 4U;
}
void Emm_V5_En_Control(uint8_t addr, bool state, bool sync)
{
  (void)state; (void)sync; Reply(addr, 0xF3, 2);
}
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr)
{
  zero_attempts[addr]++;
  if (addr == fail_zero_addr) { return; }
  if (addr == reject_zero_addr) { Reply(addr, 0x0A, 0xE2); return; }
  if (addr != ignore_zero_addr) { raw_units[addr] = 0; }
  if (addr == drop_zero_ack_addr && drop_zero_ack_count != 0U) {
    drop_zero_ack_count--; return;
  }
  Reply(addr, 0x0A, 2);
}
void Emm_V5_Stop_Now(uint8_t addr, bool sync)
{
  (void)sync; stop_commands++; Reply(addr, 0xFE, 2);
}
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t rpm, uint8_t acc,
                        uint32_t pulses, bool absolute, bool sync)
{
  Command *c = &commands[addr - 1U];
  CHECK(addr == 1U || addr == 2U);
  CHECK(rpm >= XY_MIN_RPM && rpm <= XY_DEFAULT_RPM);
  c->addr=addr; c->dir=dir; c->rpm=rpm; c->acc=acc; c->pulses=pulses;
  c->absolute=absolute; c->sync=sync;
  pos_commands++;
  Reply(addr, 0xFD, addr == fail_command_addr ? 0xE2 : (arrival_only ? 0x9F : 2));
}
void Emm_V5_Synchronous_motion(uint8_t addr)
{
  unsigned i;
  CHECK(addr == 0U);
  CHECK(pos_commands == 2U);
  CHECK(commands[0].sync && commands[1].sync);
  CHECK(commands[0].absolute && commands[1].absolute);
  sync_commands++;
  if (!stuck) {
    for (i = 0U; i < 2U; i++) {
      int32_t units = (int32_t)(((uint64_t)commands[i].pulses * XY_POS_UNITS_PER_REV + XY_PULSE_PER_REV/2U) / XY_PULSE_PER_REV);
      raw_units[i+1U] = commands[i].dir == 0U ? units : -units;
    }
  }
  if (!fail_sync) { Reply(1, 0xFF, 2); }
}
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t param)
{
  uint32_t mag;
  if (param == S_FLAG) {
    Reply(addr, 0x3A, addr == stall_addr ? 4 : (no_arrival ? 0 : 2));
    return;
  }
  CHECK(param == S_CPOS);
  if (addr == fail_position_addr || (fail_position_after_sync && sync_commands != 0U)) { return; }
  mag = raw_units[addr] < 0 ? (uint32_t)-(int64_t)raw_units[addr] : (uint32_t)raw_units[addr];
  reply[0]=addr; reply[1]=0x36; reply[2]=raw_units[addr] < 0;
  reply[3]=(uint8_t)(mag >> 24); reply[4]=(uint8_t)(mag >> 16);
  reply[5]=(uint8_t)(mag >> 8); reply[6]=(uint8_t)mag; reply[7]=0x6B;
  reply_len=8U;
}
static void Report(float x, float y) { (void)x; (void)y; reports++; }
static void Reset(void)
{
  memset(commands, 0, sizeof(commands)); memset(raw_units, 0, sizeof(raw_units));
  ticks=0; reply_len=0; pos_commands=0; sync_commands=0; stop_commands=0; reports=0;
  fail_position_addr=0; fail_command_addr=0; fail_zero_addr=0; stall_addr=0;
  stuck=false; fail_sync=false; arrival_only=false; fail_position_after_sync=false; no_arrival=false;
  reject_zero_addr=0; ignore_zero_addr=0; drop_zero_ack_addr=0; drop_zero_ack_count=0;
  memset(zero_attempts,0,sizeof(zero_attempts));
  XY_SetPositionCallback(Report); XY_Init(); XY_SetZero();
  CHECK(origin_valid);
  CHECK(XY_GetX() == 0.0f && XY_GetY() == 0.0f);
}
static void ClearMotionLog(void)
{
  pos_commands=0; sync_commands=0; stop_commands=0;
}
static void At(float x, float y)
{
  CHECK(fabsf(XY_GetX()-x) < 0.013f);
  CHECK(fabsf(XY_GetY()-y) < 0.013f);
}
static void Move(float x, float y, int32_t a, int32_t b)
{
  ClearMotionLog();
  CHECK(XY_MoveTo(x,y) == XY_RESULT_OK);
  CHECK(sync_commands == 1U && stop_commands == 0U);
  CHECK(commands[0].pulses == XY_PulseMagnitude(a));
  CHECK(commands[1].pulses == XY_PulseMagnitude(b));
  CHECK(commands[0].dir == (a < 0 ? (XY_M1_DIR_POSITIVE ^ 1U) : XY_M1_DIR_POSITIVE));
  CHECK(commands[1].dir == (b < 0 ? (XY_M2_DIR_POSITIVE ^ 1U) : XY_M2_DIR_POSITIVE));
  At(x,y);
}
int main(void)
{
  float old_x, old_y;
  unsigned old_reports;
  int32_t a,b;
  unsigned x,y;
  Reset(); CHECK(XY_PULSE_PER_MM == 80.0f);
  Move(10,0,800,800); CHECK(commands[0].rpm == commands[1].rpm);
  Move(0,0,0,0); /* absolute zero must still carry full return travel */
  Move(0,10,800,-800); CHECK(commands[0].dir != commands[1].dir);
  Move(0,0,0,0); Move(10,10,1600,0); /* M2 holds */
  Move(20,0,1600,1600); CHECK(commands[0].rpm == XY_MIN_RPM); /* M1 holds */
  Move(0,0,0,0); Move(45,35,6400,800);
  CHECK(commands[0].rpm == 300U && commands[1].rpm == 38U);
  Move(210,205,33200,400); Move(215,205,33600,800);
  Move(0,205,16400,-16400); Move(215,0,17200,17200); Move(210,0,16800,16800);
  ClearMotionLog(); CHECK(XY_MoveRelative(-10,20) == XY_RESULT_OK); At(200,20);
  ClearMotionLog(); CHECK(XY_MoveTo(200,20) == XY_RESULT_OK); CHECK(pos_commands == 0U);
  CHECK(XY_MoveTo(-0.01f,0) == XY_RESULT_OUT_OF_RANGE);
  CHECK(XY_MoveTo(215.01f,0) == XY_RESULT_OUT_OF_RANGE);
  CHECK(XY_MoveTo(0,205.01f) == XY_RESULT_OUT_OF_RANGE);
  CHECK(XY_MoveTo(0,206) == XY_RESULT_OUT_OF_RANGE);
  CHECK(XY_MoveTo(0,225) == XY_RESULT_OUT_OF_RANGE);
  { union { uint32_t u; float f; } v; v.u=0x7FC00000U;
    CHECK(XY_MoveTo(v.f,0) == XY_RESULT_OUT_OF_RANGE); v.u=0x7F800000U;
    CHECK(XY_MoveTo(0,v.f) == XY_RESULT_OUT_OF_RANGE); }
  CHECK(pos_commands == 0U);

  /* Signed inverse kinematics and rounding, including all workspace corners. */
  for (x=0; x<=215; x+=5) {
    for (y=0; y<=205; y+=9) {
      XY_StageToMotorPulses((float)x,(float)y,&a,&b);
      CHECK(a == (int32_t)(80U*(x+y)));
      CHECK(b == 80*((int32_t)x-(int32_t)y));
    }
  }
  XY_StageToMotorPulses(0,0.00625f,&a,&b); CHECK(a == 1 && b == -1);
  Reset(); Move(0.1f,0.2f,24,-8);

  /* A failed second read cannot publish a half-updated XY position. */
  old_x=XY_GetX(); old_y=XY_GetY(); old_reports=reports;
  raw_units[1] += 65536; fail_position_addr=2;
  CHECK(!XY_UpdatePositionFromMotors());
  CHECK(XY_GetX()==old_x && XY_GetY()==old_y && reports==old_reports);
  ClearMotionLog(); CHECK(XY_MoveTo(10,10)==XY_RESULT_POSITION_READ_ERROR);
  CHECK(pos_commands==0U); fail_position_addr=1;
  CHECK(!XY_UpdatePositionFromMotors()); CHECK(reports==old_reports);

  Reset(); fail_command_addr=2;
  CHECK(XY_MoveTo(10,0)==XY_RESULT_CMD_ERROR); CHECK(sync_commands==0U && stop_commands==2U);
  CHECK(!origin_valid);
  Reset(); fail_command_addr=1;
  CHECK(XY_MoveTo(10,0)==XY_RESULT_CMD_ERROR); CHECK(pos_commands==1U && sync_commands==0U);
  Reset(); arrival_only=true;
  CHECK(XY_MoveTo(10,0)==XY_RESULT_TIMEOUT); CHECK(sync_commands==0U && stop_commands==2U);
  Reset(); fail_sync=true;
  CHECK(XY_MoveTo(10,0)==XY_RESULT_TIMEOUT); CHECK(stop_commands==2U);
  Reset(); stall_addr=2;
  CHECK(XY_MoveTo(10,10)==XY_RESULT_MOTOR_FAULT); CHECK(stop_commands==2U); /* holding motor fault */
  Reset(); stuck=true;
  CHECK(XY_MoveTo(10,10)==XY_RESULT_TIMEOUT); CHECK(stop_commands==2U); /* stale arrived bit */
  Reset(); no_arrival=true;
  CHECK(XY_MoveTo(10,10)==XY_RESULT_TIMEOUT); CHECK(stop_commands==2U); /* position alone insufficient */
  Reset(); fail_position_after_sync=true;
  CHECK(XY_MoveTo(10,0)==XY_RESULT_POSITION_READ_ERROR); CHECK(stop_commands==2U);
  Reset(); fail_zero_addr=2; XY_SetZero(); CHECK(!origin_valid);
  CHECK(XY_MoveTo(10,10)==XY_RESULT_ORIGIN_ERROR); CHECK(pos_commands==0U);
  CHECK(strcmp(XY_GetDiagnosticText(),"M2 ZERO NOACK")==0);
  Reset(); raw_units[1]=1; raw_units[2]=0;
  CHECK(XY_UpdatePositionFromMotors()); CHECK(XY_GetX()<0 && XY_GetY()<0);

  /* Time calculation uses actual motor RPM, including min-RPM clamping. */
  CHECK(XY_MotorRpm(1,34800)==1U);
  CHECK(XY_EstimateMoveTimeMs(800,38)>XY_EstimateMoveTimeMs(800,300)/8U);
  /* Startup diagnostics distinguish acknowledgement, read and offset failures. */
  Reset(); fail_zero_addr=1; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_ORIGIN_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M1 ZERO NOACK")==0);
  CHECK(zero_attempts[1]==1U+XY_ZERO_RETRY && zero_attempts[2]==1U);
  CHECK(pos_commands==0U);
  Reset(); reject_zero_addr=2; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_ORIGIN_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M2 ZERO REJECT")==0);
  CHECK(zero_attempts[2]==2U); /* explicit rejection is not retried */
  Reset(); reject_zero_addr=1; XY_SetZero();
  CHECK(strcmp(XY_GetDiagnosticText(),"M1 ZERO REJECT")==0 && !origin_valid);
  Reset(); drop_zero_ack_addr=1; drop_zero_ack_count=1; XY_SetZero();
  CHECK(origin_valid && strcmp(XY_GetDiagnosticText(),"OK")==0);
  CHECK(zero_attempts[1]==3U);
  Reset(); drop_zero_ack_addr=2; drop_zero_ack_count=XY_ZERO_RETRY; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_ORIGIN_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M2 ZERO NOACK")==0); /* zero read alone never bypasses ack */
  Reset(); fail_position_addr=1; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_POSITION_READ_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M1 POS FAIL")==0);
  CHECK(pos_commands==0U);
  Reset(); fail_position_addr=2; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_POSITION_READ_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M2 POS FAIL")==0);
  Reset(); ignore_zero_addr=1; raw_units[1]=65536; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_ORIGIN_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M1 ZERO OFFSET")==0);
  Reset(); ignore_zero_addr=2; raw_units[2]=65536; XY_SetZero();
  CHECK(!origin_valid && XY_MoveTo(1,0)==XY_RESULT_ORIGIN_ERROR);
  CHECK(strcmp(XY_GetDiagnosticText(),"M2 ZERO OFFSET")==0);

  printf("PASS: %u CoreXY checks (actual C, mocked UART/HAL)\n",checks);
  return 0;
}
