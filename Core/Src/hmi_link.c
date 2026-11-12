#include "hmi_link.h"
#include "hmi_protocol.h"
#include "usart.h"
static bool ready;
static uint32_t sequence, errors;
void HMI_LinkInit(void) { ready = true; sequence = 0U; }
uint32_t HMI_GetErrorCount(void) { return errors; }
static bool HMI_Transmit(uint8_t *bytes, uint16_t length)
{
  if (!ready || HAL_UART_Transmit(&huart3, bytes, length, HMI_LINK_TX_TIMEOUT_MS) != HAL_OK) {
    errors++;
    return false;
  }
  return true;
}
uint8_t HMI_MakeCode(HMI_Shape_t shape, HMI_Color_t color)
{
  return HmiCode_Make((unsigned)shape, (unsigned)color);
}

HMI_Shape_t HMI_CodeShape(uint8_t code)
{
  if (code < HMI_LINK_CODE_MIN || code > HMI_LINK_CODE_MAX)
  {
    return HMI_SHAPE_TETRAHEDRON;
  }

  return (HMI_Shape_t)HmiCode_Shape(code);
}

bool HMI_SendCode(uint8_t code)
{
  if (!HmiCode_IsValid(code)) { return false; }
  return HMI_Transmit(&code, 1U);
}
bool HMI_SendShapeColor(HMI_Shape_t shape, HMI_Color_t color)
{
  if ((unsigned)shape >= HMI_LINK_SHAPE_COUNT || (unsigned)color >= HMI_LINK_COLOR_COUNT) { return false; }
  return HMI_SendCode(HMI_MakeCode(shape, color));
}
static bool HMI_SendExtended(uint8_t type, uint32_t seq, uint8_t code, uint8_t id)
{
  uint8_t data[10], wire[HMI_RECORD_FRAME_SIZE];
  uint16_t crc;
  unsigned i;
  data[0] = 1U; data[1] = type;
  for (i = 0U; i < 4U; i++) { data[i + 2U] = (uint8_t)(seq >> (8U * i)); }
  data[6] = code; data[7] = id;
  crc = HmiProtocol_Crc(data, 8U);
  data[8] = (uint8_t)crc; data[9] = (uint8_t)(crc >> 8);
  wire[0] = 0xF0U; wire[21] = 0xF1U;
  for (i = 0U; i < 10U; i++) {
    wire[1U + 2U * i] = 0x80U | (data[i] >> 4);
    wire[2U + 2U * i] = 0x80U | (data[i] & 15U);
  }
  /* One attempt: UART success is local TX completion, without an HMI ACK. */
  return HMI_Transmit(wire, sizeof(wire));
}
bool HMI_SendSessionReset(void) { sequence = 0U; return HMI_SendExtended(2U, 0U, 0U, 0U); }
bool HMI_SendRecord(uint8_t code, uint8_t id)
{
  if (!HmiCode_IsValid(code) || id < 1U || id > 6U) { return false; }
  return HMI_SendExtended(1U, ++sequence, code, id);
}
