#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "../../Core/Src/tray_motor.c"
__IO bool rxFrameFlag;
__IO uint8_t rxCmd[CMD_LEN], rxCount, rxDmaBuffer[CMD_LEN];
static uint8_t reply[8], len;
static uint32_t ticks, commands, last_pulse;
static double raw_pulse;
static bool reject, stall, stuck, reverse_feedback, no_position, expired;
static uint8_t ack = 2;
uint32_t HAL_GetTick(void) { return ticks++; }
void HAL_Delay(uint32_t ms) { ticks += ms; }
bool AppRuntime_Expired(void) { return expired; }
bool AppRuntime_Wait(uint32_t ms) { ticks += ms; return !expired; }
void UART_RxQueueReset(void) { len = rxCount = 0; }
bool UART_RxPopFrame(uint32_t ms) {
  if (!len) { ticks += ms; return false; }
  rxCount = len; memcpy((void *)rxCmd, reply, len); len = 0; return true;
}
static void Reply(uint8_t command, uint8_t value) {
  reply[0] = 3; reply[1] = command; reply[2] = value; reply[3] = 0x6B; len = 4;
}
void Emm_V5_En_Control(uint8_t addr, bool state, bool sync) {
  assert(addr == 3 && state && !sync); Reply(0xF3, reject ? 0xE2 : 2);
}
void Emm_V5_Reset_CurPos_To_Zero(uint8_t addr) { assert(addr == 3); raw_pulse = 0; Reply(0x0A, reject ? 0xE2 : 2); }
void Emm_V5_Stop_Now(uint8_t addr, bool sync) { assert(addr == 3 && !sync); Reply(0xFE, 2); }
void Emm_V5_Pos_Control(uint8_t addr, uint8_t dir, uint16_t rpm, uint8_t acc, uint32_t pulse, bool absolute, bool sync) {
  (void)rpm; (void)acc;
  assert(addr == 3 && dir == TRAY_DIR_POSITIVE && absolute && !sync);
  assert(pulse >= last_pulse);
  last_pulse = pulse; commands++;
  if (!stuck) raw_pulse = pulse;
  Reply(0xFD, ack);
}
void Emm_V5_Read_Sys_Params(uint8_t addr, SysParams_t param) {
  uint32_t magnitude;
  assert(addr == 3);
  if (param == S_FLAG) { Reply(0x3A, stall ? 4 : 2); return; }
  assert(param == S_CPOS);
  if (no_position) return;
  magnitude = (uint32_t)(raw_pulse * TRAY_POS_UNITS_PER_REV / TRAY_PULSE_PER_REV + .5);
  reply[0]=3; reply[1]=0x36; reply[2] = reverse_feedback ? 0 : TRAY_DIR_POSITIVE;
  reply[3]=(uint8_t)(magnitude>>24); reply[4]=(uint8_t)(magnitude>>16);
  reply[5]=(uint8_t)(magnitude>>8); reply[6]=(uint8_t)magnitude; reply[7]=0x6B; len=8;
}
static void Reset(void) {
  reject=stall=stuck=reverse_feedback=no_position=expired=false; ack=2;
  last_pulse=commands=0; raw_pulse=0;
  assert(Tray_Init() == TRAY_RESULT_OK && Tray_IsOriginValid());
}
int main(void) {
  unsigned i, previous;
  Reset();
  assert(Tray_MoveToAngle(300)==0 && last_pulse==2667);
  assert(Tray_MoveToAngle(0)==0 && last_pulse==3200);
  previous=commands; assert(Tray_MoveToAngle(360)==0 && commands==previous);
  Reset(); assert(Tray_MoveToAngle(120)==0); assert(Tray_MoveToAngle(60)==0 && last_pulse==3733);
  Reset();
  for (i=1; i<=600; i++) {
    assert(Tray_MoveToAngle((float)((i%6)*60))==0);
    if (i%6==0) assert(last_pulse==(i/6)*3200U);
  }
  previous=commands;
  assert(Tray_MoveToAngle(NAN)==TRAY_RESULT_INVALID_ANGLE);
  assert(Tray_MoveToAngle(INFINITY)==TRAY_RESULT_INVALID_ANGLE);
  assert(Tray_MoveToAngle(-1)==TRAY_RESULT_INVALID_ANGLE);
  assert(Tray_MoveToAngle(361)==TRAY_RESULT_INVALID_ANGLE && commands==previous);
  Reset(); raw_pulse=100; assert(Tray_MoveToAngle(0)==TRAY_RESULT_POSITION_MISMATCH && !origin_valid && !commands);
  Reset(); raw_pulse=600; assert(Tray_MoveToAngle(60)==TRAY_RESULT_POSITION_MISMATCH && !commands);
  Reset(); assert(Tray_MoveToAngle(60)==0); reverse_feedback=true;
  assert(Tray_MoveToAngle(120)==TRAY_RESULT_POSITION_MISMATCH && !origin_valid);
  Reset(); ack=0x9F; assert(Tray_MoveToAngle(60)==TRAY_RESULT_TIMEOUT && !origin_valid);
  Reset(); stall=true; assert(Tray_MoveToAngle(60)==TRAY_RESULT_MOTOR_FAULT && !origin_valid);
  Reset(); stuck=true; assert(Tray_MoveToAngle(60)==TRAY_RESULT_TIMEOUT && !origin_valid);
  Reset(); no_position=true; assert(Tray_MoveToAngle(60)==TRAY_RESULT_POSITION_READ_ERROR && !origin_valid);
  Reset(); expired=true; assert(Tray_MoveToAngle(60)==TRAY_RESULT_TIMEOUT && !origin_valid);
  Reset(); reject=true; assert(Tray_SetZero()!=0 && !origin_valid);
  Reset(); accumulated_target=11796420.0; target_phase=300.0f; raw_pulse=floor(accumulated_target*3200.0/360.0+.5);
  assert(Tray_MoveToAngle(0)==TRAY_RESULT_OVERFLOW && !origin_valid);
  puts("PASS tray: 100 revolutions, no rounding drift; direction, limits and faults");
  return 0;
}
