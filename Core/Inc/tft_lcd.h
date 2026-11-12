/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tft_lcd.h
  * @brief   ST7735 TFT LCD hardware SPI driver.
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __TFT_LCD_H__
#define __TFT_LCD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define TFT_W                   320U
#define TFT_H                   240U
#define TFT_BUFFER_H            (TFT_H / 2U)

#define TFT_WHITE               0xFFFF
#define TFT_BLACK               0x0000
#define TFT_BLUE                0x001F
#define TFT_RED                 0xF800
#define TFT_GREEN               0x07E0
#define TFT_CYAN                0x7FFF
#define TFT_YELLOW              0xFFE0
#define TFT_GRAY                0x8430
#define TFT_DARKBLUE            0x01CF

void TFT_Init(void);
void TFT_SetViewport(uint16_t y_origin);
uint16_t TFT_GetViewportY(void);
void TFT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void TFT_ShowString(uint16_t x, uint16_t y, const char *text, uint16_t fg, uint16_t bg, uint8_t scale);
void TFT_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                   const uint16_t *pixels);
void TFT_DrawImageScaled(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                         const uint16_t *pixels, uint8_t scale);
void TFT_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                  uint16_t color);
void TFT_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                  uint16_t color);

#ifdef __cplusplus
}
#endif

#endif /* __TFT_LCD_H__ */
