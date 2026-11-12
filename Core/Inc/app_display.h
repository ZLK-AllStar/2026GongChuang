/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_display.h
  * @brief   TFT display page for XY stage and vacuum state.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __APP_DISPLAY_H__
#define __APP_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void AppDisplay_Init(void);
void AppDisplay_SetPosition(float x_mm, float y_mm);
void AppDisplay_SetMotionTarget(float x_mm, float y_mm);
void AppDisplay_SetCommand(const char *cmd);
void AppDisplay_SetVacuumState(const char *state);
void AppDisplay_SetHostFrame(uint8_t sequence, uint8_t command,
                             uint16_t x_mm, uint16_t y_mm);
void AppDisplay_Service(void);
void AppDisplay_Refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_DISPLAY_H__ */
