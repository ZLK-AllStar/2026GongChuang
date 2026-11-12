#include "app_display.h"
#include "screen.h"
#include "tft_lcd.h"
#include "host_link.h"
#include "hmi_link.h"
#include "app_mode.h"
#include "app_controller.h"
#include "app_config.h"
#include "app_runtime.h"
#include "xy_stage.h"
#include "tray_motor.h"
#include "aux_servo.h"
#include <stdio.h>
#include <string.h>
static float display_x_mm, display_y_mm, display_target_x_mm, display_target_y_mm;
static char display_command[32] = "INIT", display_state[24] = "IDLE";
static uint8_t display_host_sequence, display_host_command;
static uint16_t display_host_x, display_host_y;
static bool display_initialized, display_service_active, dirty = true;
static uint32_t last_draw;
static void AppDisplay_DrawTaskPage(void);
static void AppDisplay_DrawMm(uint16_t x, uint16_t y, char axis, float value, uint16_t color, uint8_t scale);
static void AppDisplay_DrawText(uint16_t x, uint16_t y, const char *text, uint16_t color, uint8_t scale);
void AppDisplay_Init(void) { TFT_Init(); display_initialized = true; dirty = true; }
void AppDisplay_SetPosition(float x, float y) { display_x_mm = x; display_y_mm = y; dirty = true; }
void AppDisplay_SetMotionTarget(float x, float y) { display_target_x_mm = x; display_target_y_mm = y; dirty = true; }
static void CopyText(char *dest, unsigned size, const char *text)
{
  if (!text) { text = ""; }
  if (strncmp(dest, text, size) != 0) { (void)snprintf(dest, size, "%s", text); dirty = true; }
}
void AppDisplay_SetCommand(const char *cmd) { CopyText(display_command, sizeof(display_command), cmd); }
void AppDisplay_SetVacuumState(const char *state) { CopyText(display_state, sizeof(display_state), state); }
void AppDisplay_SetHostFrame(uint8_t seq, uint8_t cmd, uint16_t x, uint16_t y)
{
  display_host_sequence = seq; display_host_command = cmd;
  display_host_x = x; display_host_y = y; dirty = true;
}
void AppDisplay_Refresh(void) { dirty = true; }
void AppDisplay_Service(void)
{
  uint16_t y;
  uint32_t now;
  if (!display_initialized || display_service_active) { return; }
  now = HAL_GetTick();
  if ((uint32_t)(now - last_draw) < APP_DISPLAY_PERIOD_MS) { return; }
  /* Diagnostic status also changes without setters (tray/origin/online/errors).
   * Refresh that cache at most twice a second. */
  if (!dirty && (uint32_t)(now - last_draw) < 500U) { return; }
  display_service_active = true;
  for (y = 0U; y < TFT_H; y += TFT_BUFFER_H) {
    TFT_SetViewport(y);
    AppDisplay_DrawTaskPage();
    Screen_Flush();
  }
  dirty = false;
  last_draw = HAL_GetTick();
  display_service_active = false;
}
/* Single strong HAL delay definition. Shell parsing is never serviced here. */
void HAL_Delay(uint32_t ms)
{
  uint32_t start = HAL_GetTick();
  while ((uint32_t)(HAL_GetTick() - start) < ms) { AppRuntime_Service(); }
}

static void AppDisplay_DrawTaskPage(void)
{
  char line[32];
  TFT_Fill(0U, 0U, TFT_W, TFT_H, TFT_BLACK);
  AppDisplay_DrawText(8U, 6U, AppMode_Name(), TFT_YELLOW, 2U);
  TFT_DrawLine(0U, 28U, TFT_W - 1U, 28U, TFT_CYAN);
  AppDisplay_DrawText(8U, 36U, "POSITION", TFT_CYAN, 1U);
  AppDisplay_DrawMm(8U, 50U, 'X', display_x_mm, TFT_WHITE, 2U);
  AppDisplay_DrawMm(8U, 70U, 'Y', display_y_mm, TFT_WHITE, 2U);
  AppDisplay_DrawText(170U, 36U, "TARGET", TFT_CYAN, 1U);
  AppDisplay_DrawMm(170U, 50U, 'X', display_target_x_mm, TFT_GRAY, 1U);
  AppDisplay_DrawMm(170U, 64U, 'Y', display_target_y_mm, TFT_GRAY, 1U);
  AppDisplay_DrawText(8U, 100U, "CMD", TFT_CYAN, 1U);
  AppDisplay_DrawText(64U, 100U, display_command, TFT_GREEN, 1U);
  AppDisplay_DrawText(8U, 118U, "VAC", TFT_CYAN, 1U);
  AppDisplay_DrawText(64U, 118U, display_state, TFT_GREEN, 1U);
  TFT_DrawLine(0U, 142U, TFT_W - 1U, 142U, TFT_CYAN);
  AppDisplay_DrawText(8U, 150U, "HOST FRAME", TFT_CYAN, 1U);
  (void)snprintf(line, sizeof(line), "SEQ:%u CMD:%u", display_host_sequence,
                 display_host_command);
  AppDisplay_DrawText(8U, 166U, line, TFT_WHITE, 1U);
  (void)snprintf(line, sizeof(line), "X:%uMM Y:%uMM", display_host_x, display_host_y);
  AppDisplay_DrawText(8U, 182U, line, TFT_WHITE, 1U);
  (void)snprintf(line, sizeof(line), "TRAY:%d ID:%u ORIGIN:%u/%u", (int)Tray_GetAngleDeg(),
                 AppController_GetId(), XY_IsOriginValid(), Tray_IsOriginValid());
  AppDisplay_DrawText(8U, 202U, line, TFT_WHITE, 1U);
  (void)snprintf(line, sizeof(line), "PC:%u HMI ERR:%lu LIFT:%u", HostLink_GetHostReady(),
                 (unsigned long)HMI_GetErrorCount(), AuxServo_GetLiftAngle());
  AppDisplay_DrawText(8U, 220U, line, TFT_GRAY, 1U);
}
static void AppDisplay_DrawMm(uint16_t x, uint16_t y, char axis, float value,
                              uint16_t color, uint8_t scale)
{
  char line[24];
  int32_t scaled = (int32_t)(value * 10.0f + (value >= 0.0f ? 0.5f : -0.5f));
  (void)snprintf(line, sizeof(line), "%c:%ld.%ldMM", axis, (long)(scaled / 10),
                 (long)(scaled < 0 ? -scaled % 10 : scaled % 10));
  AppDisplay_DrawText(x, y, line, color, scale);
}

static void AppDisplay_DrawText(uint16_t x, uint16_t y, const char *text,
                                 uint16_t color, uint8_t scale)
{
  TFT_ShowString(x, y, text, color, TFT_BLACK, scale);
}
