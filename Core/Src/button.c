/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    button.c
  * @brief   PE2 实体按键输入（低电平有效）与软件消抖。
  ******************************************************************************
  */
/* USER CODE END Header */

#include "button.h"

/* 消抖状态机：
 *   debounced   已确认的电平，空闲为 false（内部上拉），按下为 true。
 *   stable_since 最近一次电平变化的时间戳，用于累计“电平已保持”的时长。
 *   last_sample  上一次采样时间戳，控制采样节奏。 */
static bool button_debounced = false;
static bool button_stable_level = false;
static uint32_t button_stable_since = 0U;
static uint32_t button_last_sample = 0U;

void Button_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* PE2 空闲时由内部上拉保证高电平，按下时接地。 */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  GPIO_InitStruct.Pin = BTN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BTN_GPIO_Port, &GPIO_InitStruct);

  button_stable_level = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET;
  button_debounced = false;
  button_stable_since = HAL_GetTick();
  button_last_sample = button_stable_since;
}

bool Button_Poll(void)
{
  uint32_t now = HAL_GetTick();
  bool level;

  /* 采样节奏由时间戳控制，调用方即使比 BUTTON_POLL_INTERVAL_MS 更频繁地
   * 轮询（例如每 1 ms 的等待循环），也不会改变消抖时长。 */
  if ((uint32_t)(now - button_last_sample) < BUTTON_POLL_INTERVAL_MS)
  {
    return false;
  }
  button_last_sample = now;

  level = HAL_GPIO_ReadPin(BTN_GPIO_Port, BTN_Pin) == GPIO_PIN_RESET;

  if (level != button_stable_level)
  {
    button_stable_level = level;
    button_stable_since = now;
    return false;
  }

  if ((uint32_t)(now - button_stable_since) < BUTTON_DEBOUNCE_MS)
  {
    return false;
  }

  if (level && !button_debounced)
  {
    button_debounced = true;
    return true;   /* 消抖后的按下边沿：报告一次按下事件 */
  }

  button_debounced = level;
  return false;
}

bool Button_IsPressed(void)
{
  return button_debounced;
}

bool Button_IsReleasedStable(void)
{
  return !button_debounced && !button_stable_level &&
         (uint32_t)(HAL_GetTick() - button_stable_since) >= BUTTON_DEBOUNCE_MS;
}
