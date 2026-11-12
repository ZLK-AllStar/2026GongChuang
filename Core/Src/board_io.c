#include "board_io.h"
#include "button.h"

void BoardIO_Init(void)
{
  GPIO_InitTypeDef gpio = {0};
  __HAL_RCC_GPIOF_CLK_ENABLE();
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9 | GPIO_PIN_10, GPIO_PIN_SET);
  gpio.Pin = GPIO_PIN_9 | GPIO_PIN_10;
  gpio.Mode = GPIO_MODE_OUTPUT_PP;
  gpio.Pull = GPIO_NOPULL;
  gpio.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &gpio);
  Button_Init();
}

void BoardIO_SetLeds(bool led0, bool led1)
{
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9, led0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10, led1 ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
