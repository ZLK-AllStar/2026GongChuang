#ifndef MECHANICAL_HAL_H
#define MECHANICAL_HAL_H
#include <stdint.h>
#include <stdbool.h>
#define __IO volatile
typedef struct { uintptr_t Instance; uint32_t ErrorCode; } UART_HandleTypeDef;
typedef struct { unsigned unused; } DMA_HandleTypeDef;
typedef struct { unsigned Pin, Mode, Pull, Speed; } GPIO_InitTypeDef;
typedef int GPIO_PinState;
#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET 1
#define GPIO_PIN_2 4U
#define GPIO_PIN_9 512U
#define GPIO_PIN_10 1024U
#define GPIOE 5
#define GPIOF 6
#define GPIO_MODE_INPUT 0U
#define GPIO_MODE_OUTPUT_PP 1U
#define GPIO_PULLUP 1U
#define GPIO_NOPULL 0U
#define GPIO_SPEED_FREQ_LOW 0U
#define __HAL_RCC_GPIOE_CLK_ENABLE() ((void)0)
#define __HAL_RCC_GPIOF_CLK_ENABLE() ((void)0)
#define USART1 1U
#define USART2 2U
#define UART5 5U
#define USART2_IRQn 2U
#define UART5_IRQn 5U
#define __get_PRIMASK() 0U
#define __disable_irq() ((void)0)
#define __set_PRIMASK(value) ((void)(value))
#define HAL_OK 0
#define HAL_UART_ERROR_NONE 0
#define __HAL_UART_CLEAR_OREFLAG(u) ((void)(u))
#define __HAL_UART_CLEAR_FEFLAG(u) ((void)(u))
#define __HAL_UART_CLEAR_NEFLAG(u) ((void)(u))
#define __HAL_UART_CLEAR_PEFLAG(u) ((void)(u))
#define TIM_CHANNEL_1 0U
#define TIM_CHANNEL_2 4U
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t ms);
GPIO_PinState HAL_GPIO_ReadPin(int port, unsigned pin);
void HAL_GPIO_Init(int port, GPIO_InitTypeDef *gpio);
void HAL_GPIO_WritePin(int port, unsigned pin, GPIO_PinState state);
void HAL_NVIC_SetPriority(unsigned irq, unsigned priority, unsigned sub);
void HAL_NVIC_EnableIRQ(unsigned irq);
int HAL_UART_Receive_IT(UART_HandleTypeDef *uart, uint8_t *data, unsigned n);
int HAL_UART_AbortReceive(UART_HandleTypeDef *uart);
int HAL_UART_Transmit(UART_HandleTypeDef *uart, uint8_t *data, unsigned n, uint32_t timeout);
#endif
