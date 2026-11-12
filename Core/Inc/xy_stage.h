/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    xy_stage.h
  * @brief   CoreXY two-motor stage control interface.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __XY_STAGE_H__
#define __XY_STAGE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "app_config.h"
#include <stdbool.h>

/* M1 = left/blue motor, M2 = right/red motor in the supplied belt diagram. */
#define XY_M1_ADDR                 1U
#define XY_M2_ADDR                 2U

/* Maximum motor RPM, NOT Cartesian feed rate. Keep existing acceleration.
 * RPM is apportioned by motor travel. Independent ramps / integer RPM mean
 * point-to-point coordination, not exact contour/linear interpolation.
 */
#define XY_DEFAULT_RPM             300U
#define XY_MIN_RPM                 1U
#define XY_DEFAULT_ACC             20U
#define XY_MOVE_UPDATE_MS          100U
#define XY_MOVE_TIME_MARGIN_MS     300U
#define XY_MOTOR_READY_DELAY_MS    500U
#define XY_CMD_GAP_MS              10U
#define XY_UART_TIMEOUT_MS         100U
#define XY_POS_READ_TIMEOUT_MS     50U
#define XY_POS_READ_RETRY          3U
#define XY_POS_READ_GAP_MS         5U
#define XY_POS_TOLERANCE_PULSE     20U
#define XY_ZERO_RETRY              3U
#define XY_ZERO_RETRY_GAP_MS       100U

typedef enum
{
  XY_RESULT_OK = 0,
  XY_RESULT_TIMEOUT,
  XY_RESULT_CMD_ERROR,
  XY_RESULT_MOTOR_FAULT,
  XY_RESULT_POSITION_READ_ERROR,
  XY_RESULT_OUT_OF_RANGE,
  XY_RESULT_ORIGIN_ERROR
} XY_Result_t;

typedef void (*XY_PositionCallback_t)(float x_mm, float y_mm);

XY_Result_t XY_Init(void);
/* Sets the current physical location to (0, 0); does NOT perform homing. */
XY_Result_t XY_SetZero(void);
bool XY_IsOriginValid(void);
void XY_InvalidateOrigin(void);
/* Diagnostic for the last origin/position failure; static string, no allocation. */
const char *XY_GetDiagnosticText(void);
XY_Result_t XY_MoveTo(float x_mm, float y_mm);
XY_Result_t XY_MoveRelative(float dx_mm, float dy_mm);
void XY_Stop(void);
bool XY_UpdatePositionFromMotors(void);
float XY_GetX(void);
float XY_GetY(void);
float XY_GetTargetX(void);
float XY_GetTargetY(void);
const char *XY_ResultText(XY_Result_t result);
void XY_SetPositionCallback(XY_PositionCallback_t callback);

#ifdef __cplusplus
}
#endif

#endif /* __XY_STAGE_H__ */
