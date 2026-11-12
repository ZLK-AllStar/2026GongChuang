/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    vacuum.c
  * @brief   Vacuum pump and valve control implementation.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "vacuum.h"
#include "tim.h"
#include "app_runtime.h"
static bool pump_on, valve_on;

static Vacuum_StatusCallback_t status_callback = 0;

static void Servo_WriteAngle(uint32_t channel, uint8_t angle);
static void Vacuum_ReportStatus(const char *state);

void Vacuum_Init(void)
{
  /* Reconfigure TIM1 for 50 Hz / 1 us-per-tick servo PWM at runtime.
   * TIM1 is on APB2 (timer clock = 168 MHz on F407 at 168 MHz SYSCLK).
   * CubeMX leaves it with Prescaler=0 / Period=65535. We need
   * 1 us/tick (168 MHz / 168 = 1 MHz) and 20 ms period (ARR = 20000). */
  __HAL_TIM_SET_PRESCALER(&htim1, 167U);
  __HAL_TIM_SET_AUTORELOAD(&htim1, 19999U);

  __HAL_TIM_SET_COUNTER(&htim1, 0U);
  Vacuum_SetValve(false);
  Vacuum_SetPump(false);
  HAL_TIM_GenerateEvent(&htim1, TIM_EVENTSOURCE_UPDATE);
  HAL_TIM_PWM_Start(&htim1, VACUUM_PUMP_CHANNEL);
  HAL_TIM_PWM_Start(&htim1, VACUUM_VALVE_CHANNEL);

  Vacuum_SetValve(false);
  Vacuum_SetPump(false);
  Vacuum_ReportStatus("IDLE");
  HAL_Delay(500);
}

void Vacuum_Pick(void)
{
  Vacuum_ReportStatus("VALVE OFF");
  Vacuum_SetValve(false);
  HAL_Delay(200);
  Vacuum_ReportStatus("PUMP ON");
  Vacuum_SetPump(true);
  Vacuum_ReportStatus("PICKING");
  HAL_Delay(VACUUM_PICK_MS);
  Vacuum_SetPump(false);
  Vacuum_ReportStatus("HOLD");
}

bool Vacuum_PickAndHold(void)
{
  Vacuum_ReportStatus("VALVE OFF");
  Vacuum_SetValve(false);
  if (!AppRuntime_Wait(200U)) { return false; }
  Vacuum_ReportStatus("PUMP ON");
  Vacuum_SetPump(true);
  Vacuum_ReportStatus("PICKING");
  if (!AppRuntime_Wait(VACUUM_PICK_MS)) { return false; }
  Vacuum_ReportStatus("HOLD");
  return true;
}

bool Vacuum_Release(void)
{
  Vacuum_ReportStatus("PUMP OFF");
  Vacuum_SetPump(false);
  if (!AppRuntime_Wait(200U)) { return false; }
  Vacuum_ReportStatus("VALVE ON");
  Vacuum_SetValve(true);
  Vacuum_ReportStatus("UNLOAD");
  if (!AppRuntime_Wait(VACUUM_RELEASE_MS)) { Vacuum_SetValve(false); return false; }
  Vacuum_SetValve(false);
  Vacuum_ReportStatus("IDLE");
  return true;
}

void Vacuum_SetPump(bool open)
{
  pump_on = open;
  Servo_WriteAngle(VACUUM_PUMP_CHANNEL, open ? SERVO_OPEN_ANGLE : SERVO_CLOSE_ANGLE);
}

void Vacuum_SetValve(bool open)
{
  valve_on = open;
  Servo_WriteAngle(VACUUM_VALVE_CHANNEL, open ? SERVO_OPEN_ANGLE : SERVO_CLOSE_ANGLE);
}

void Vacuum_SetStatusCallback(Vacuum_StatusCallback_t callback)
{
  status_callback = callback;
}

static void Vacuum_ReportStatus(const char *state)
{
  if (status_callback != 0)
  {
    status_callback(state);
  }
}

static void Servo_WriteAngle(uint32_t channel, uint8_t angle)
{
  uint32_t pulse = SERVO_MIN_PULSE_US;

  if (angle > SERVO_OPEN_ANGLE)
  {
    angle = SERVO_OPEN_ANGLE;
  }

  pulse += ((uint32_t)angle * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US)) / SERVO_OPEN_ANGLE;
  __HAL_TIM_SET_COMPARE(&htim1, channel, pulse);
}

bool Vacuum_IsPumpOn(void) { return pump_on; }
bool Vacuum_IsValveOn(void) { return valve_on; }
