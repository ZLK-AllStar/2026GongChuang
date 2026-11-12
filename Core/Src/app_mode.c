#include "app_mode.h"
#include "app_config.h"
#include "board_io.h"
#include "button.h"
/* 默认进入 HOST；启动窗口内按 PE2 改为 DEBUG。 */
static App_Mode_t mode = APP_MODE_HOST;
static bool locked;

void AppMode_SelectAtBoot(void)
{
  uint32_t start;
  uint32_t elapsed;
  if (locked) { return; }
  start = HAL_GetTick();
  while ((elapsed = (uint32_t)(HAL_GetTick() - start)) < APP_BOOT_SELECT_MS) {
    if (Button_Poll()) { mode = APP_MODE_DEBUG; }
    BoardIO_SetLeds(((elapsed / APP_LED_BLINK_HALF_MS) & 1U) == 0U, false);
  }
  locked = true;
  BoardIO_SetLeds(mode == APP_MODE_HOST, mode == APP_MODE_DEBUG);
}

App_Mode_t AppMode_Get(void) { return mode; }
const char *AppMode_Name(void) { return mode == APP_MODE_HOST ? "HOST" : "DEBUG"; }
