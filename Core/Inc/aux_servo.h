/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    aux_servo.h
  * @brief   Auxiliary servo control interface (TIM3).
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __AUX_SERVO_H__
#define __AUX_SERVO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>
#define AUX_SERVO_LIFT_CHANNEL   TIM_CHANNEL_1  /* PA6, vertical vacuum cup */

#define AUX_SERVO_MIN_ANGLE      0U
#define AUX_SERVO_MAX_ANGLE      270U
#define AUX_SERVO_MIN_PULSE_US   500U
#define AUX_SERVO_MAX_PULSE_US   2500U

/* Mechanical change: the cup now rests down at power-on, so the down angle is
 * the servo's default position and the raised position is 80 degrees above it. */
#define AUX_SERVO_LIFT_UP_ANGLE    			270U
#define AUX_SERVO_LIFT_DOWN_ANGLE  			100U
#define AUX_SERVO_LIFT_DOWN_ANGLE_FULL  100U
/* All cylinder colors use their own pickup height. */
#define AUX_SERVO_LIFT_DOWN_ANGLE_CYLINDER  130U
/* Cup angle used to drop material onto a release target. The part hangs below
 * the cup, so dropping it needs less travel than reaching its top face for a
 * pick; this depth is therefore tuned on its own. */
#define AUX_SERVO_RELEASE_ANGLE					200U
#define AUX_SERVO_LIFT_MOVE_MS     			1000U

/* Pickup presets: normal for most materials, FULL for cuboids, and CYLINDER
 * for every cylinder color. The current normal/FULL presets share one angle;
 * pick_place.c selects the preset by shape. */

/* Hold at the power-on default (down) position before the cup is raised. */
#define AUX_SERVO_LIFT_SETTLE_MS   2000U

void AuxServo_Init(void);
uint16_t AuxServo_GetLiftAngle(void);
void AuxServo_SetAngle(uint32_t channel, uint16_t angle);
/* Absolute integer degrees, 0..AUX_SERVO_MAX_ANGLE. Rejects out-of-range
 * targets; waits for settling and updates the cached angle on success. */
bool AuxServo_LiftTo(uint16_t angle);
bool AuxServo_LiftUp(void);
bool AuxServo_LiftDown(void);
bool AuxServo_LiftDownFull(void);
bool AuxServo_LiftRelease(void);

#ifdef __cplusplus
}
#endif

#endif /* __AUX_SERVO_H__ */
