#ifndef APP_MODE_H
#define APP_MODE_H
typedef enum { APP_MODE_HOST = 0, APP_MODE_DEBUG } App_Mode_t;
void AppMode_SelectAtBoot(void);
App_Mode_t AppMode_Get(void);
const char *AppMode_Name(void);
#endif
