#include "tft_lcd.h"
#include "ext_sram.h"
#include "screen.h"

#define TFT_MAX_TEXT_SCALE 4U

static const uint8_t FONT_SPACE[5] = {0, 0, 0, 0, 0};
static const uint8_t FONT_UNKNOWN[5] = {0x7F, 0x41, 0x5D, 0x41, 0x7F};
static uint16_t tft_viewport_y;

void TFT_SetViewport(uint16_t y_origin)
{
  tft_viewport_y = (y_origin <= (uint16_t)(TFT_H - TFT_BUFFER_H))
                 ? y_origin : 0U;
}

uint16_t TFT_GetViewportY(void)
{
  return tft_viewport_y;
}

static void TFT_SetPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if (x < TFT_W && y >= tft_viewport_y &&
      y < (uint16_t)(tft_viewport_y + TFT_BUFFER_H))
  {
    TFT_FrameBuffer[(uint32_t)(y - tft_viewport_y) * TFT_W + x] = color;
  }
}

void TFT_Init(void)
{
  Screen_Init();
  TFT_SetViewport(0U);
  TFT_Fill(0U, 0U, TFT_W, TFT_H, TFT_WHITE);
  Screen_Flush();
  TFT_SetViewport(TFT_BUFFER_H);
  TFT_Fill(0U, 0U, TFT_W, TFT_H, TFT_WHITE);
  Screen_Flush();
}

void TFT_Fill(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
  uint16_t x;
  uint16_t y;

  if (x1 > TFT_W) x1 = TFT_W;
  if (y1 > TFT_H) y1 = TFT_H;
  for (y = y0; y < y1; y++)
  {
    for (x = x0; x < x1; x++)
    {
      TFT_SetPixel(x, y, color);
    }
  }
}

void TFT_DrawImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                   const uint16_t *pixels)
{
  uint16_t row;
  uint16_t col;

  if (pixels == NULL) return;
  for (row = 0U; row < height && (uint16_t)(y + row) < TFT_H; row++)
  {
    for (col = 0U; col < width && (uint16_t)(x + col) < TFT_W; col++)
    {
      TFT_SetPixel((uint16_t)(x + col), (uint16_t)(y + row),
                   pixels[(uint32_t)row * width + col]);
    }
  }
}

/* 最近邻放大：源图像素直接复制为 scale*scale 色块，不插值，适合小素材放大。 */
void TFT_DrawImageScaled(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                         const uint16_t *pixels, uint8_t scale)
{
  uint16_t row;
  uint16_t col;
  uint16_t sx;
  uint16_t sy;
  uint16_t dx;
  uint16_t dy;

  if (pixels == NULL || scale == 0U) return;
  for (row = 0U; row < height; row++)
  {
    for (sy = 0U; sy < scale; sy++)
    {
      dy = (uint16_t)(y + (uint16_t)(row * scale) + sy);
      if (dy >= TFT_H) break;
      for (col = 0U; col < width; col++)
      {
        uint16_t color = pixels[(uint32_t)row * width + col];
        for (sx = 0U; sx < scale; sx++)
        {
          dx = (uint16_t)(x + (uint16_t)(col * scale) + sx);
          if (dx >= TFT_W) break;
          TFT_SetPixel(dx, dy, color);
        }
      }
    }
  }
}

void TFT_DrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height,
                  uint16_t color)
{
  uint16_t i;
  if (width == 0U || height == 0U) return;
  for (i = 0U; i < width; i++)
  {
    TFT_SetPixel((uint16_t)(x + i), y, color);
    TFT_SetPixel((uint16_t)(x + i), (uint16_t)(y + height - 1U), color);
  }
  for (i = 0U; i < height; i++)
  {
    TFT_SetPixel(x, (uint16_t)(y + i), color);
    TFT_SetPixel((uint16_t)(x + width - 1U), (uint16_t)(y + i), color);
  }
}

void TFT_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,
                  uint16_t color)
{
  uint16_t y;
  if (y0 == y1)
  {
    uint16_t x;
    if (x1 < x0) { x = x0; x0 = x1; x1 = x; }
    for (x = x0; x <= x1; x++) TFT_SetPixel(x, y0, color);
    return;
  }
  if (x0 == x1)
  {
    if (y1 < y0) { y = y0; y0 = y1; y1 = y; }
    for (y = y0; y <= y1; y++) TFT_SetPixel(x0, y, color);
  }
}

void TFT_ShowString(uint16_t x, uint16_t y, const char *text, uint16_t fg,
                    uint16_t bg, uint8_t scale)
{
  static const uint8_t digits[10][5] = {
    {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10}, {0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}
  };
  static const uint8_t letters[26][5] = {
    {0x7E,0x11,0x11,0x11,0x7E}, {0x7F,0x49,0x49,0x49,0x36},
    {0x3E,0x41,0x41,0x41,0x22}, {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01},
    {0x3E,0x41,0x49,0x49,0x7A}, {0x7F,0x08,0x08,0x08,0x7F},
    {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40},
    {0x7F,0x02,0x0C,0x02,0x7F}, {0x7F,0x04,0x08,0x10,0x7F},
    {0x3E,0x41,0x41,0x41,0x3E}, {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46},
    {0x46,0x49,0x49,0x49,0x31}, {0x01,0x01,0x7F,0x01,0x01},
    {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F},
    {0x3F,0x40,0x38,0x40,0x3F}, {0x63,0x14,0x08,0x14,0x63},
    {0x07,0x08,0x70,0x08,0x07}, {0x61,0x51,0x49,0x45,0x43}
  };
  static const uint8_t minus[5] = {8,8,8,8,8};
  static const uint8_t dot[5] = {0,0x60,0x60,0,0};
  static const uint8_t colon[5] = {0,0x36,0x36,0,0};
  static const uint8_t slash[5] = {0x20,0x10,8,4,2};
  uint16_t cursor = x;

  if (scale == 0U) scale = 1U;
  if (scale > TFT_MAX_TEXT_SCALE) scale = TFT_MAX_TEXT_SCALE;
  while (*text != '\0' && cursor < TFT_W)
  {
    const uint8_t *bitmap = FONT_UNKNOWN;
    char ch = *text++;
    uint8_t col;
    uint8_t row;
    if (ch >= '0' && ch <= '9') bitmap = digits[ch - '0'];
    else if (ch >= 'a' && ch <= 'z') bitmap = letters[ch - 'a'];
    else if (ch >= 'A' && ch <= 'Z') bitmap = letters[ch - 'A'];
    else if (ch == ' ') bitmap = FONT_SPACE;
    else if (ch == '-') bitmap = minus;
    else if (ch == '.') bitmap = dot;
    else if (ch == ':') bitmap = colon;
    else if (ch == '/') bitmap = slash;

    for (row = 0U; row < 8U * scale && (uint16_t)(y + row) < TFT_H; row++)
    {
      for (col = 0U; col < 6U * scale && (uint16_t)(cursor + col) < TFT_W; col++)
      {
        uint8_t src_col = (uint8_t)(col / scale);
        uint8_t src_row = (uint8_t)(row / scale);
        uint8_t bits = src_col < 5U ? bitmap[src_col] : 0U;
        TFT_SetPixel((uint16_t)(cursor + col), (uint16_t)(y + row),
                     (bits & (uint8_t)(1U << src_row)) ? fg : bg);
      }
    }
    cursor = (uint16_t)(cursor + 6U * scale);
  }
}
