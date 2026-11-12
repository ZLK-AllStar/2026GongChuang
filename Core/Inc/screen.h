#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "main.h"
#include <stdbool.h>

typedef enum
{
  SCREEN_WRITE_COMMAND = 0,
  SCREEN_WRITE_DATA,
  SCREEN_WRITE_PIXELS
} Screen_WriteKind_t;

typedef struct
{
  Screen_WriteKind_t kind;
  const uint8_t *data;
  uint32_t length;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
} Screen_WriteOp_t;

typedef bool (*Screen_WriteBackend_t)(const Screen_WriteOp_t *op);

void Screen_Init(void);
void LCD_Init(SPI_HandleTypeDef *spi);
bool Screen_Write(const Screen_WriteOp_t *op);
void Screen_SetWriteBackend(Screen_WriteBackend_t write_backend);
void Screen_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void Screen_Flush(void);

#endif /* __SCREEN_H__ */
