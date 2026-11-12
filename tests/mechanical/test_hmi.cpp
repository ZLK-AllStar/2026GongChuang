#include <cassert>
#include <cstdio>
#include <vector>
#include "hmi_link.h"
#include "HmiRecord.h"
UART_HandleTypeDef huart3 = {3,0};
static std::vector<uint8_t> wire;
int HAL_UART_Transmit(UART_HandleTypeDef *, uint8_t *p, unsigned n, uint32_t) { wire.assign(p,p+n); return 0; }
static bool Send(HmiParser &parser, HmiEvent &event, uint32_t start=0) {
  bool result=false;
  for (uint8_t b : wire) if (parser.feed(b,start++,event)) { assert(!result); result=true; }
  return result;
}
static void Encode(uint8_t version,uint8_t type,uint8_t code,uint8_t id) {
  uint8_t d[10]={version,type,1,0,0,0,code,id,0,0};
  uint16_t crc=HmiProtocol_Crc(d,8); d[8]=uint8_t(crc);d[9]=uint8_t(crc>>8);
  wire.assign(22,0);wire[0]=0xF0;wire[21]=0xF1;
  for(unsigned i=0;i<10;i++) { wire[1+2*i]=0x80|(d[i]>>4);wire[2+2*i]=0x80|(d[i]&15); }
}
int main() {
  HmiParser parser; HmiEvent event;
  assert(HmiProtocol_Crc(reinterpret_cast<const uint8_t *>("123456789"),9)==0x29B1);
  HMI_LinkInit(); assert(HMI_SendRecord(19,3));
  const uint8_t sample[]={0xF0,0x80,0x81,0x80,0x81,0x80,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x83,0x80,0x83,0x86,0x8F,0x8E,0x8D,0xF1};
  assert(wire==std::vector<uint8_t>(sample,sample+22));
  assert(Send(parser,event) && event.type==1 && event.record.id==3 && event.record.code==19 && event.record.sequence==1);
  assert(HMI_SendCode(1) && wire.size()==1 && Send(parser,event) && event.type==0);
  assert(HMI_SendRecord(72,6) && Send(parser,event) && event.record.id==6 && event.record.code==72 && event.record.sequence==2);
  assert(HMI_SendRecord(1,1) && Send(parser,event) && event.record.id==1);
  assert(HMI_SendSessionReset() && Send(parser,event) && event.type==2 && event.record.sequence==0);
  assert(!HMI_SendCode(0) && !HMI_SendRecord(91,1) && !HMI_SendRecord(1,7));
  Encode(1,1,91,1); assert(!Send(parser,event));
  assert(!parser.feed(91,0,event));
  Encode(1,1,19,3); wire[6]^=1; assert(!Send(parser,event)); // CRC
  for(unsigned which=0;which<5;which++) {
    Encode(which==0?2:1,which==1?3:1,which==2?0:19,which==3?7:3);
    if(which==4) wire.insert(wire.begin()+5,19); // never feed low invalid byte back as preview
    assert(!Send(parser,event));
  }
  Encode(1,1,19,3);
  for(unsigned i=0;i<10;i++) assert(!parser.feed(wire[i],i,event));
  for(unsigned i=10;i<wire.size();i++) assert(!parser.feed(wire[i],300+i,event));
  assert(parser.feed(72,400,event) && event.type==0);
  assert(Send(parser,event,0xFFFFFFF5U)); // tick wrap while receiving a split frame
  for(unsigned split=1;split<22;split++) {
    HmiParser splitParser;
    for(unsigned i=0;i<split;i++) assert(!splitParser.feed(wire[i],i,event));
    for(unsigned i=split;i<22;i++) assert(splitParser.feed(wire[i],i+20,event)==(i==21));
  }
  // Back-to-back frames preserve arrival order, including repeated categories.
  for(unsigned i=0;i<20;i++) {
    assert(HMI_SendRecord(uint8_t(i%72+1),uint8_t(i%6+1)) && Send(parser,event,i*30));
    assert(event.record.code==i%72+1 && event.record.id==i%6+1);
  }
  // Every original pair retains its byte; white occupies the appended range.
  bool seen[91] = {};
  for (unsigned shape = 0; shape < 9; ++shape) {
    for (unsigned color = 0; color < 10; ++color) {
      uint8_t code = HMI_MakeCode(HMI_Shape_t(shape), HMI_Color_t(color));
      unsigned expected = shape == 8 ? 81 + color :
                          color == 9 ? 73 + shape : shape * 9 + color + 1;
      assert(code == expected && !seen[code]); seen[code] = true;
      assert(unsigned(HMI_CodeShape(code)) == shape && HmiCode_Color(code) == color);
      assert(HMI_SendShapeColor(HMI_Shape_t(shape), HMI_Color_t(color)));
      assert(wire.size() == 1 && wire[0] == code && Send(parser,event) && event.type == 0);
      assert(HMI_SendRecord(code, uint8_t(shape % 6 + 1)) && Send(parser,event));
      assert(event.type == 1 && event.record.code == code);
    }
  }
  for (unsigned code = 1; code <= 90; ++code) assert(seen[code]);
  assert(!HMI_SendShapeColor(HMI_Shape_t(9), HMI_COLOR_WHITE));
  assert(!HMI_SendShapeColor(HMI_SHAPE_CUBE, HMI_Color_t(10)));
  assert(HMI_MakeCode(HMI_Shape_t(-1), HMI_COLOR_WHITE) == 0);
  assert(HMI_CodeShape(91) == HMI_SHAPE_TETRAHEDRON);
  std::puts("PASS HMI: real FW encoder + specified HMI parser, CRC, split/noise/timeout/preview/order");
}
