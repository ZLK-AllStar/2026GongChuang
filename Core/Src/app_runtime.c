#include "app_runtime.h"
#include "app_config.h"
#include "app_controller.h"
#include "app_display.h"
static uint32_t task_start;
static bool task_active;
static bool servicing;

void AppRuntime_BeginTask(void) { task_start = HAL_GetTick(); task_active = true; }
void AppRuntime_EndTask(void) { task_active = false; }
bool AppRuntime_Expired(void)
{
  return task_active && (uint32_t)(HAL_GetTick() - task_start) >= APP_HOST_TASK_BUDGET_MS;
}

void AppRuntime_Service(void)
{
  if (servicing) { return; }
  servicing = true;
  /* Parses/replies only. Never execute Shell or start a motor transaction here. */
  AppController_Service();
  AppDisplay_Service();
  servicing = false;
}

bool AppRuntime_Wait(uint32_t ms)
{
  uint32_t start = HAL_GetTick();
  while ((uint32_t)(HAL_GetTick() - start) < ms) {
    if (AppRuntime_Expired()) { return false; }
    AppRuntime_Service();
  }
  return !AppRuntime_Expired();
}
