#include <assert.h>
#include <stdio.h>
#include "aux_servo.h"

/* Mock only TIM3 registers and the runtime wait; exercise the real servo code. */
typedef struct { uint32_t prescaler, period, counter, compare; } TIM_HandleTypeDef;
TIM_HandleTypeDef htim3;
static unsigned pwm_writes, waits;
static bool expired, wait_ok = true;
#define TIM_EVENTSOURCE_UPDATE 1U
#define __HAL_TIM_SET_PRESCALER(timer, value) ((timer)->prescaler = (value))
#define __HAL_TIM_SET_AUTORELOAD(timer, value) ((timer)->period = (value))
#define __HAL_TIM_SET_COUNTER(timer, value) ((timer)->counter = (value))
#define __HAL_TIM_SET_COMPARE(timer, channel, value) \
  ((void)(channel), (timer)->compare = (value), pwm_writes++)
void HAL_TIM_GenerateEvent(TIM_HandleTypeDef *timer, uint32_t event) { (void)timer; (void)event; }
void HAL_TIM_PWM_Start(TIM_HandleTypeDef *timer, uint32_t channel) { (void)timer; (void)channel; }
void HAL_Delay(uint32_t ms) { (void)ms; }
bool AppRuntime_Expired(void) { return expired; }
bool AppRuntime_Wait(uint32_t ms) { assert(ms == AUX_SERVO_LIFT_MOVE_MS); waits++; return wait_ok; }
#include "../../Core/Src/aux_servo.c"

int main(void)
{
  unsigned before;
  assert(AuxServo_LiftTo(0) && htim3.compare == 500 && AuxServo_GetLiftAngle() == 0);
  assert(AuxServo_LiftTo(270) && htim3.compare == 2500 && AuxServo_GetLiftAngle() == 270);
  assert(AuxServo_LiftTo(120) && htim3.compare == 1388 && AuxServo_GetLiftAngle() == 120);
  assert(pwm_writes == 3 && waits == 3);
  assert(AuxServo_LiftTo(120) && pwm_writes == 3 && waits == 3);
  assert(!AuxServo_LiftTo(271) && !AuxServo_LiftTo(65535));
  assert(AuxServo_GetLiftAngle() == 120 && pwm_writes == 3 && waits == 3);
  assert(AuxServo_LiftUp() && AuxServo_GetLiftAngle() == AUX_SERVO_LIFT_UP_ANGLE);
  assert(AuxServo_LiftTo(120));
  before = pwm_writes;
  assert(AuxServo_LiftUp() && pwm_writes == before + 1);
  assert(AuxServo_GetLiftAngle() == AUX_SERVO_LIFT_UP_ANGLE);
  assert(AuxServo_LiftDown() && AuxServo_GetLiftAngle() == AUX_SERVO_LIFT_DOWN_ANGLE);
  assert(AuxServo_LiftDownFull() && AuxServo_GetLiftAngle() == AUX_SERVO_LIFT_DOWN_ANGLE_FULL);
  before = waits;
  assert(AuxServo_LiftTo(AUX_SERVO_LIFT_DOWN_ANGLE_CYLINDER));
  assert(AuxServo_GetLiftAngle() == 130 && htim3.compare == 1462 && waits == before + 1);
  assert(AuxServo_LiftTo(AUX_SERVO_LIFT_DOWN_ANGLE_CYLINDER) && waits == before + 1);
  assert(AuxServo_LiftUp() && AuxServo_GetLiftAngle() == AUX_SERVO_LIFT_UP_ANGLE);
  wait_ok = false;
  assert(!AuxServo_LiftTo(90) && AuxServo_GetLiftAngle() == 0xFFFFU);
  wait_ok = true;
  assert(AuxServo_LiftTo(90) && AuxServo_GetLiftAngle() == 90);
  expired = true;
  before = pwm_writes;
  assert(!AuxServo_LiftTo(90) && pwm_writes == before);
  puts("PASS lift: real PWM endpoints, cylinder 130deg/1462us/cache/lift, arbitrary angles, invalid targets, wait failure/recovery");
  return 0;
}
