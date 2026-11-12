/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    aux_servo.c
  * @brief   Auxiliary servo control implementation (TIM3).
  ******************************************************************************
  */
/* USER CODE END Header */

#include "aux_servo.h"
#include "tim.h"
#include "app_runtime.h"

/* Cached cup angle: starts at the power-on default (down). Caching the angle
 * instead of a named position lets a command that is already satisfied return
 * without the move delay, even when two positions share one angle. */
static uint16_t lift_angle = AUX_SERVO_LIFT_DOWN_ANGLE;

bool AuxServo_LiftTo(uint16_t angle)
{
  if (angle > AUX_SERVO_MAX_ANGLE) { return false; }

  if (lift_angle == angle)
  {
    return !AppRuntime_Expired();
  }

  AuxServo_SetAngle(AUX_SERVO_LIFT_CHANNEL, angle);
  if (!AppRuntime_Wait(AUX_SERVO_LIFT_MOVE_MS)) { lift_angle = 0xFFFFU; return false; }
  lift_angle = angle;
  return true;
}

void AuxServo_Init(void)
{
  /* TIM3 is on APB1 (timer clock = 84 MHz on F407 at 168 MHz SYSCLK).
   * CubeMX leaves it with Prescaler=0 / Period=65535. Reconfigure to
   * 1 us/tick (84 MHz / 84 = 1 MHz) and 20 ms period (ARR = 20000). */
  __HAL_TIM_SET_PRESCALER(&htim3, 83U);
  __HAL_TIM_SET_AUTORELOAD(&htim3, 19999U);
  __HAL_TIM_SET_COUNTER(&htim3, 0U);
  HAL_TIM_GenerateEvent(&htim3, TIM_EVENTSOURCE_UPDATE);

  /* Power-on sequence: the mechanics leave the cup down, so drive that default
   * position first, then raise the cup 80 degrees. Every move afterwards starts
   * from the raised position. */
  AuxServo_SetAngle(AUX_SERVO_LIFT_CHANNEL, AUX_SERVO_LIFT_DOWN_ANGLE);
  HAL_TIM_PWM_Start(&htim3, AUX_SERVO_LIFT_CHANNEL);
  HAL_Delay(AUX_SERVO_LIFT_SETTLE_MS);
  AuxServo_LiftUp();
}

void AuxServo_SetAngle(uint32_t channel, uint16_t angle)
{
  uint32_t pulse = AUX_SERVO_MIN_PULSE_US;

  if (angle > AUX_SERVO_MAX_ANGLE)
  {
    angle = AUX_SERVO_MAX_ANGLE;
  }

  pulse += ((uint32_t)angle * (AUX_SERVO_MAX_PULSE_US - AUX_SERVO_MIN_PULSE_US)) / AUX_SERVO_MAX_ANGLE;
  __HAL_TIM_SET_COMPARE(&htim3, channel, pulse);
}

bool AuxServo_LiftUp(void)
{
  return AuxServo_LiftTo(AUX_SERVO_LIFT_UP_ANGLE);
}

bool AuxServo_LiftDown(void)
{
  return AuxServo_LiftTo(AUX_SERVO_LIFT_DOWN_ANGLE);
}

bool AuxServo_LiftDownFull(void)
{
  return AuxServo_LiftTo(AUX_SERVO_LIFT_DOWN_ANGLE_FULL);
}

bool AuxServo_LiftRelease(void)
{
  return AuxServo_LiftTo(AUX_SERVO_RELEASE_ANGLE);
}

uint16_t AuxServo_GetLiftAngle(void) { return lift_angle; }
