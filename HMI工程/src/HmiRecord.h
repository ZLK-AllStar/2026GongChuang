#ifndef HMI_RECORD_H
#define HMI_RECORD_H
#include "hmi_protocol.h"
#include "hmi_codes.h"
#include <cstring>
struct HmiRecord {
  uint8_t code = 0, id = 0;
  uint32_t sequence = 0;
};
struct HmiEvent {
  uint8_t type = 0; // 0 preview, 1 production, 2 session clear
  HmiRecord record;
};
class HmiParser {
  bool active = false, invalid = false;
  unsigned count = 0;
  uint8_t data[10] = {};
  uint32_t last = 0;
public:
  bool feed(uint8_t byte, uint32_t now, HmiEvent &event) {
    if (active && uint32_t(now - last) >= 200U) {
      active = false;
      if (byte != 0xF0) return false; // discard the timed-out fragment byte
    }
    if (byte == 0xF0) {
      active = true; invalid = false; count = 0; last = now;
      std::memset(data, 0, sizeof(data));
      return false;
    }
    if (!active) {
      if (!HmiCode_IsValid(byte)) return false;
      event = HmiEvent(); event.record.code = byte;
      return true;
    }
    last = now;
    if (byte == 0xF1) {
      active = false;
      if (invalid || count != 20 || data[0] != 1 ||
          HmiProtocol_Crc(data, 8) != uint16_t(data[8] | uint16_t(data[9]) << 8)) return false;
      event = HmiEvent(); event.type = data[1];
      event.record.sequence = uint32_t(data[2]) | uint32_t(data[3]) << 8 |
                              uint32_t(data[4]) << 16 | uint32_t(data[5]) << 24;
      event.record.code = data[6]; event.record.id = data[7];
      if (event.type == 2) return event.record.sequence == 0 && data[6] == 0 && data[7] == 0;
      return event.type == 1 && HmiCode_IsValid(data[6]) && data[7] >= 1 && data[7] <= 6;
    }
    if (invalid || byte < 0x80 || byte > 0x8F || count >= 20) { invalid = true; return false; }
    data[count / 2] |= uint8_t((byte & 15) << ((count % 2) ? 0 : 4));
    ++count;
    return false;
  }
};
#endif
