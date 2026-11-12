#ifndef TRAY_MOTOR_H
#define TRAY_MOTOR_H
#include "main.h"
#include "app_config.h"
#include <stdbool.h>
#define TRAY_MOTOR_ADDR 3U
#define TRAY_DEFAULT_RPM 300U
#define TRAY_DEFAULT_ACC 20U
#define TRAY_MOVE_UPDATE_MS 100U
#define TRAY_MOVE_TIME_MARGIN_MS 300U
#define TRAY_CMD_GAP_MS 10U
#define TRAY_UART_TIMEOUT_MS 100U
#define TRAY_POS_READ_TIMEOUT_MS 50U
typedef enum {
  TRAY_RESULT_OK = 0, TRAY_RESULT_TIMEOUT, TRAY_RESULT_CMD_ERROR,
  TRAY_RESULT_MOTOR_FAULT, TRAY_RESULT_POSITION_READ_ERROR,
  TRAY_RESULT_POSITION_MISMATCH, TRAY_RESULT_INVALID_ANGLE,
  TRAY_RESULT_ORIGIN_ERROR, TRAY_RESULT_OVERFLOW
} Tray_Result_t;
Tray_Result_t Tray_Init(void);
Tray_Result_t Tray_SetZero(void);
Tray_Result_t Tray_MoveToAngle(float angle_deg);
void Tray_Stop(void);
void Tray_InvalidateOrigin(void);
bool Tray_IsOriginValid(void);
float Tray_GetAngleDeg(void);
float Tray_GetAccumulatedAngleDeg(void);
const char *Tray_GetDiagnosticText(void);
const char *Tray_ResultText(Tray_Result_t result);
#endif
