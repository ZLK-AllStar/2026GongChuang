#ifndef TEST_STM32F4XX_HAL_H
#define TEST_STM32F4XX_HAL_H
#include <stdint.h>
#define __IO volatile
typedef struct { uint32_t unused; } UART_HandleTypeDef;
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t delay);
#endif
