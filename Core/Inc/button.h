/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    button.h
  * @brief   PE2 实体按键输入（低电平有效）与软件消抖。
  *
  *          按键一端接 PE2，另一端接 3.3V：按下为低电平，松开由内部下拉
  *          电阻拉低。Button_Poll() 在轮询过程中一旦检测到消抖后的上升沿
  *          就返回 true，同一段按下只报告一次，必须松开后再次按下才会
  *          产生下一次事件。
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdbool.h>

/* 轮询间隔与消抖时间（毫秒）。消抖时间必须大于轮询间隔。 */
#define BUTTON_POLL_INTERVAL_MS   5U
#define BUTTON_DEBOUNCE_MS        20U

/* 配置按键引脚。在 MX_GPIO_Init() 之后调用一次。 */
void Button_Init(void);

/* 轮询按键状态；检测到有效的“按下”事件时返回 true，其余情况返回 false。
 * 必须按 BUTTON_POLL_INTERVAL_MS 的节奏持续调用，否则会漏掉短按。 */
bool Button_Poll(void);

/* 当前消抖后的按键状态：按下为 true。界面提示可直接用它显示按压与否。 */
bool Button_IsPressed(void);
bool Button_IsReleasedStable(void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
