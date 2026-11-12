#ifndef APP_RUNTIME_H
#define APP_RUNTIME_H
#include "main.h"
#include <stdbool.h>
void AppRuntime_Service(void);
void AppRuntime_BeginTask(void);
void AppRuntime_EndTask(void);
bool AppRuntime_Expired(void);
bool AppRuntime_Wait(uint32_t ms);
#endif
