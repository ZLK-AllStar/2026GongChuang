#ifndef HMI_PROTOCOL_H
#define HMI_PROTOCOL_H
#include <stdint.h>
#define HMI_RECORD_FRAME_SIZE 22U
static uint16_t HmiProtocol_Crc(const uint8_t *bytes, unsigned length)
{
  uint16_t crc = 0xFFFFU;
  unsigned i;
  while (length-- != 0U) {
    crc ^= (uint16_t)*bytes++ << 8;
    for (i = 0U; i < 8U; i++) {
      crc = (uint16_t)((crc & 0x8000U) ? ((uint32_t)crc << 1) ^ 0x1021U : (uint32_t)crc << 1);
    }
  }
  return crc;
}
#endif
