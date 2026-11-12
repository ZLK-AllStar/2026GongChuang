#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H
#include <stdint.h>
void AppController_Init(void);
void AppController_Service(void);
void AppController_Poll(void);
uint8_t AppController_GetId(void);
#endif
