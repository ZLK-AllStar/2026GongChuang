// Exercise the actual page methods: current per-code totals, preview,
// reopening, eviction, session clear and formatting beyond 32-bit counts.
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include "HmiRecord.h"
using namespace std;
struct HmiApp {
  vector<HmiRecord> commandHistory;
  uint64_t classifiedCounts[HMI_CODE_MAX + 1U] = {};
  uint8_t previewCode = 0;
};
struct MockScroll { void scrollToY(int, int) {} };
struct STTStyle { MockScroll *p_scrollbar = nullptr; };
struct MockList {
  vector<string> images, labels;
  void clearOptions() { images.clear(); labels.clear(); }
  void addButton(const char *image, const char *label) {
    images.push_back(image); labels.push_back(label);
  }
  STTStyle *getStyle() { return nullptr; }
};
struct Frm02 {
  HmiApp *hmiApp;
  MockList *wMList4;
  void syncCommandHistory();
  void appendRecord(const HmiRecord &record);
};
#include "Frm02_custom.inc"

static HmiRecord Record(uint8_t code, uint8_t id = 1)
{
  HmiRecord record; record.code = code; record.id = id; return record;
}
int main()
{
  HmiApp app; MockList list; Frm02 page = {&app, &list};
  page.syncCommandHistory(); assert(list.labels.empty());
  app.commandHistory = {Record(73), Record(80), Record(73)};
  app.classifiedCounts[73] = 2; app.classifiedCounts[80] = 1;
  page.syncCommandHistory(); assert(list.labels.size() == 4);
  assert(list.images[0] == "./images/HDR-RECORDS.png");
  assert(list.images[1] == "./images/RT-WHITE.png");
  assert(list.labels[1].find("正四面体-白  同类总数:2") != string::npos);
  assert(list.labels[2].find("球体-白  同类总数:1") != string::npos);
  assert(list.labels[3].find("同类总数:2") != string::npos);
  auto before = list.labels;
  page.syncCommandHistory(); assert(before == list.labels);
  assert(app.classifiedCounts[73] == 2);
  app.previewCode = 73; page.syncCommandHistory();
  assert(list.labels.back().find("调试预览") != string::npos);
  assert(list.labels.back().find("同类总数") == string::npos);
  assert(app.classifiedCounts[73] == 2);
  // The visible history can contain one item while the session total is 300.
  app.previewCode = 0; app.commandHistory = {Record(1)};
  app.classifiedCounts[1] = 300; page.syncCommandHistory();
  assert(list.labels.back().find("同类总数:300") != string::npos);
  app.classifiedCounts[1] = UINT64_C(4294967300); page.syncCommandHistory();
  assert(list.labels.back().find("同类总数:4294967300") != string::npos);
  // All 90 codes address their own counter, including the appended white set.
  for (unsigned code = 1; code <= 90; ++code) {
    app.commandHistory = {Record(uint8_t(code))};
    app.classifiedCounts[code] = 1000 + code; page.syncCommandHistory();
    assert(list.labels.back().find("同类总数:" + to_string(1000 + code)) != string::npos);
    if (code>=81) {
      static const char* colors[]={"RED","ORANGE","YELLOW","GREEN","CYAN","BLUE","PURPLE","GRAY","BLACK","WHITE"};
      assert(list.images.back()==string("./images/CO-")+colors[code-81]+".png");
      assert(list.labels.back().find("圆锥-")!=string::npos);
    }
  }
  auto size = list.labels.size();
  page.appendRecord(Record(0)); page.appendRecord(Record(91));
  assert(list.labels.size() == size);
  app.commandHistory.clear(); std::memset(app.classifiedCounts, 0, sizeof(app.classifiedCounts));
  page.syncCommandHistory(); assert(list.labels.empty());
  puts("PASS Frm02 totals: current counts for all 90 codes, preview excluded, "
       "reopen, eviction, reset, 64-bit formatting");
}
