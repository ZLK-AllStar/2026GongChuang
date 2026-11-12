#ifndef SOFT_UART_TEST_HAL_H
#define SOFT_UART_TEST_HAL_H
#include "../stubs/stm32f4xx_hal.h"
typedef struct { volatile uint32_t IDR, BSRR; } GPIO_TypeDef;
typedef struct { volatile uint32_t CR1, DIER, SR, PSC, ARR, CNT, EGR; } TIM_TypeDef;
typedef struct { TIM_TypeDef *Instance; } TIM_HandleTypeDef;
typedef struct { volatile uint32_t CTRL, CYCCNT; } TestDWT;
typedef struct { volatile uint32_t DEMCR; } TestCoreDebug;
extern GPIO_TypeDef test_gpio;
extern TIM_TypeDef test_tim8, test_tim13;
extern TestDWT test_dwt;
extern TestCoreDebug test_debug;
extern uint32_t test_primask, test_ipsr, test_hclk, test_pclk;
#undef GPIOF
#define GPIOF (&test_gpio)
#define GPIO_PIN_4 16U
#define TIM8 (&test_tim8)
#define TIM13 (&test_tim13)
#define DWT (&test_dwt)
#define CoreDebug (&test_debug)
#define DWT_CTRL_NOCYCCNT_Msk (1U << 25U)
#define DWT_CTRL_CYCCNTENA_Msk 1U
#define CoreDebug_DEMCR_TRCENA_Msk (1U << 24U)
#define TIM_DIER_UIE 1U
#define TIM_CR1_CEN 1U
#define TIM_CR1_URS 4U
#define TIM_SR_UIF 1U
#define TIM_EGR_UG 1U
#define TIM8_UP_TIM13_IRQn 44U
#define __HAL_RCC_TIM13_CLK_ENABLE() ((void)0)
#undef __get_PRIMASK
#undef __disable_irq
#undef __set_PRIMASK
#define __get_PRIMASK() test_primask
#define __disable_irq() (test_primask = 1U)
#define __set_PRIMASK(value) (test_primask = (value))
#define __get_IPSR() test_ipsr
#define HAL_RCC_GetHCLKFreq() test_hclk
#define HAL_RCC_GetPCLK1Freq() test_pclk
#define HAL_GPIO_Init TestSoftUart_GPIOInit
void TestSoftUart_GPIOInit(GPIO_TypeDef *port, GPIO_InitTypeDef *gpio);
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *timer);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *dma);
void HAL_UART_IRQHandler(UART_HandleTypeDef *uart);
void HAL_IncTick(void);
int HAL_UART_DMAStop(UART_HandleTypeDef *uart);
int HAL_UART_Receive_DMA(UART_HandleTypeDef *uart, uint8_t *data, unsigned n);
#define RESET 0U
#define UART_FLAG_IDLE 1U
#define __HAL_UART_GET_FLAG(u, flag) ((void)(u), (void)(flag), 0U)
#define __HAL_UART_CLEAR_IDLEFLAG(u) ((void)(u))
#define __HAL_DMA_GET_COUNTER(u) ((void)(u), 0U)
#endif
