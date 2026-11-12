#include "screen.h"
#include "screen_config.h"
#include "ext_sram.h"
#include "spi.h"
#include "tft_lcd.h"

static bool screen_ready;
static Screen_WriteBackend_t screen_backend;
static SPI_HandleTypeDef *screen_spi;

void Screen_SetWriteBackend(Screen_WriteBackend_t write_backend)
{
  screen_backend = write_backend;
}

bool Screen_Write(const Screen_WriteOp_t *op)
{
  GPIO_PinState dc;

  if (op == NULL || op->data == NULL || op->length == 0U)
  {
    return false;
  }
  if (screen_backend != NULL)
  {
    return screen_backend(op);
  }

  dc = (op->kind == SCREEN_WRITE_COMMAND) ? GPIO_PIN_RESET : GPIO_PIN_SET;
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, dc);
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit((screen_spi != NULL) ? screen_spi : &hspi1,
                       (uint8_t *)op->data,
                       (uint16_t)op->length, 1000U) != HAL_OK)
  {
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
    return false;
  }
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  return true;
}

static void Screen_WriteByte(Screen_WriteKind_t kind, uint8_t value)
{
  Screen_WriteOp_t op = {kind, &value, 1U, 0U, 0U, 0U, 0U};
  (void)Screen_Write(&op);
}

void LCD_Init(SPI_HandleTypeDef *spi)
{
  screen_spi = (spi != NULL) ? spi : &hspi1;

  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin,
                    SCREEN_BLK_DEFAULT_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(120U);
  HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, GPIO_PIN_SET);
  HAL_Delay(120U);

  /* Keep the validated Star Gamer ST7789 initialization sequence verbatim. */
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x36U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x00U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x3AU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x05U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xB2U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0CU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0CU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x00U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x33U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x33U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xB7U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x35U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xBBU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x19U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xC0U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x2CU);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xC2U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x01U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xC3U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x12U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xC4U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x20U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xC6U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0FU);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xD0U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0xA4U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0xA1U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xE0U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0xD0U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x04U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0DU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x11U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x13U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x2BU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x3FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x54U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x4CU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x18U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0DU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0BU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x1FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x23U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0xE1U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0xD0U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x04U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x0CU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x11U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x13U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x2CU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x3FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x44U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x51U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x2FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x1FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x1FU);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x20U);
  Screen_WriteByte(SCREEN_WRITE_DATA, 0x23U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x21U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x11U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x29U);
  Screen_WriteByte(SCREEN_WRITE_COMMAND, 0x36U);
  /* Flip MX and MY from 0x60 to rotate the landscape display by 180 degrees. */
  Screen_WriteByte(SCREEN_WRITE_DATA, 0xA0U);
  HAL_GPIO_WritePin(BLK_GPIO_Port, BLK_Pin,
                    SCREEN_BLK_DEFAULT_ON ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void Screen_Init(void)
{
  LCD_Init(&hspi1);
  screen_ready = true;
}

void Screen_SetWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t args[4];
  uint8_t cmd;
  Screen_WriteOp_t op;

  cmd = 0x2AU;
  op.kind = SCREEN_WRITE_COMMAND; op.data = &cmd; op.length = 1U;
  op.x = x0; op.y = y0; op.width = (uint16_t)(x1 - x0 + 1U); op.height = (uint16_t)(y1 - y0 + 1U);
  (void)Screen_Write(&op);
  args[0] = (uint8_t)(x0 >> 8); args[1] = (uint8_t)x0;
  args[2] = (uint8_t)(x1 >> 8); args[3] = (uint8_t)x1;
  op.kind = SCREEN_WRITE_DATA; op.data = args; op.length = 4U;
  (void)Screen_Write(&op);

  cmd = 0x2BU;
  op.kind = SCREEN_WRITE_COMMAND; op.data = &cmd; op.length = 1U;
  (void)Screen_Write(&op);
  args[0] = (uint8_t)(y0 >> 8); args[1] = (uint8_t)y0;
  args[2] = (uint8_t)(y1 >> 8); args[3] = (uint8_t)y1;
  op.kind = SCREEN_WRITE_DATA; op.data = args; op.length = 4U;
  (void)Screen_Write(&op);

  cmd = 0x2CU;
  op.kind = SCREEN_WRITE_COMMAND; op.data = &cmd; op.length = 1U;
  (void)Screen_Write(&op);
}

void Screen_Flush(void)
{
  uint16_t y;
  uint16_t x;
  uint8_t tx[128U * 2U];
  uint16_t index;

  if (!screen_ready)
  {
    return;
  }
  uint16_t y_origin = TFT_GetViewportY();

  Screen_SetWindow(0U, y_origin, TFT_W - 1U,
                   (uint16_t)(y_origin + TFT_BUFFER_H - 1U));
  for (y = 0U; y < TFT_BUFFER_H; y++)
  {
    for (x = 0U; x < TFT_W; x = (uint16_t)(x + 128U))
    {
      uint16_t count = (uint16_t)((TFT_W - x) > 128U ? 128U : (TFT_W - x));
      for (index = 0U; index < count; index++)
      {
        uint16_t pixel = TFT_FrameBuffer[(uint32_t)y * TFT_W + x + index];
        tx[index * 2U] = (uint8_t)(pixel >> 8);
        tx[index * 2U + 1U] = (uint8_t)pixel;
      }
      {
        Screen_WriteOp_t op = {SCREEN_WRITE_PIXELS, tx, (uint32_t)count * 2U,
                               x, (uint16_t)(y_origin + y), count, 1U};
        (void)Screen_Write(&op);
      }
    }
  }
}
