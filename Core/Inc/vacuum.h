/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    vacuum.h
  * @brief   Vacuum pump and valve control interface.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __VACUUM_H__
#define __VACUUM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>

#define VACUUM_PUMP_CHANNEL     TIM_CHANNEL_1   /* PE9  TIM1_CH1 */
#define VACUUM_VALVE_CHANNEL    TIM_CHANNEL_2   /* PE11 TIM1_CH2 */

#define SERVO_CLOSE_ANGLE       0U
#define SERVO_OPEN_ANGLE        180U
#define SERVO_MIN_PULSE_US      500U
#define SERVO_MAX_PULSE_US      2500U

#define VACUUM_PICK_MS          1500U
#define VACUUM_RELEASE_MS       2000U
#define VACUUM_CYCLE_DELAY_MS   1000U

typedef void (*Vacuum_StatusCallback_t)(const char *state);

void Vacuum_Init(void);
bool Vacuum_IsPumpOn(void);
bool Vacuum_IsValveOn(void);
void Vacuum_Pick(void);
bool Vacuum_PickAndHold(void);
bool Vacuum_Release(void);
void Vacuum_SetPump(bool open);
void Vacuum_SetValve(bool open);
void Vacuum_SetStatusCallback(Vacuum_StatusCallback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* __VACUUM_H__ */
