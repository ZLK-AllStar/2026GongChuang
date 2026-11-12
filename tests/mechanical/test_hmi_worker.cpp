// Host test for the Wcmn receive path: real Wcmn.cpp method bodies plus the
// SDK stand-ins from hmi_worker_mock.h.
//
// Covered: no I/O before load, zero-wait read, parse/CRC/split/timeout, one UI
// hop per batch, UI-thread-only state writes, bounded history, list refresh
// count, missing page, dispose, and async start failure.
#include "Wcmn.h"
#include <cstdio>

MockPages testPages;
MockVars testVars;
uint32_t testNow = 0;

static vector<uint8_t> Frame(uint8_t type, uint32_t seq, uint8_t code, uint8_t id)
{
  uint8_t data[10] = {1, type, uint8_t(seq), uint8_t(seq >> 8),
    uint8_t(seq >> 16), uint8_t(seq >> 24), code, id, 0, 0};
  uint16_t crc = HmiProtocol_Crc(data, 8);
  data[8] = uint8_t(crc); data[9] = uint8_t(crc >> 8);
  vector<uint8_t> wire(1, 0xF0);
  for (uint8_t b : data) { wire.push_back(0x80 | (b >> 4)); wire.push_back(0x80 | (b & 15)); }
  wire.push_back(0xF1);
  return wire;
}

static void Notify(Wcmn &page) { page.wMCom3_vch_cb(31, LvEvent()); }
static unsigned Pending() { return unsigned(MockWorkerState::pending().size()); }

int main()
{
  HmiApp app;
  Frm02 records;
  testPages.page = &records;
  Wcmn page(&app, nullptr, "Wcmn");
  assert(page.wMCom3 == nullptr);
  assert(!page.active);
  Notify(page);                        // Before onLoad: no I/O at all.
  assert(page.wMCom3 == nullptr);

  page.createParts();
  page.wMCom3->packets.push_back(Frame(1, 1, 19, 3));
  Notify(page);                        // Still not loaded: bytes stay buffered.
  assert(page.wMCom3->reads == 0 && Pending() == 0);

  page.onLoad();
  Notify(page);
  assert(page.wMCom3->reads == 1);
  assert(Pending() == 1);              // One hop queued for the UI thread.
  assert(app.commandHistory.empty() && records.refreshes == 0 && testVars.last.empty());
  RunPendingUi();
  assert(app.commandHistory.size() == 1 && app.commandHistory[0].id == 3);
  assert(app.commandHistory[0].sequence == 1 && records.refreshes == 1);
  assert(!testVars.last.empty());
  assert(app.classifiedCounts[19]==1);

  Notify(page);                        // Idle notification: no extra UI work.
  assert(Pending() == 0 && records.refreshes == 1);

  auto frame = Frame(1, 2, 72, 6);
  page.wMCom3->packets.push_back(vector<uint8_t>(frame.begin(), frame.begin() + 9));
  Notify(page);
  assert(Pending() == 0);
  testNow += 125;
  page.wMCom3->packets.push_back(vector<uint8_t>(frame.begin() + 9, frame.end()));
  Notify(page);
  RunPendingUi();
  assert(app.commandHistory.size() == 2 && app.commandHistory.back().code == 72);
  assert(app.classifiedCounts[19]==1 && app.classifiedCounts[72]==1);

  page.wMCom3->packets.push_back(vector<uint8_t>(frame.begin(), frame.begin() + 9));
  Notify(page);
  testNow += 250;                      // Parser timeout: the fragment is dropped.
  page.wMCom3->packets.push_back(vector<uint8_t>(frame.begin() + 9, frame.end()));
  Notify(page);
  assert(Pending() == 0 && app.commandHistory.size() == 2);

  // A burst yields one UI hop and one list rebuild, applied in arrival order.
  vector<uint8_t> burst = Frame(1, 3, 10, 2);
  for (auto bytes : {Frame(2, 0, 0, 0), Frame(1, 1, 28, 4)})
    burst.insert(burst.end(), bytes.begin(), bytes.end());
  burst.push_back(19);
  unsigned refreshes = records.refreshes;
  page.wMCom3->packets.push_back(burst);
  Notify(page);
  assert(Pending() == 1);
  RunPendingUi();
  assert(app.commandHistory.size() == 1 && app.commandHistory[0].code == 28);
  assert(app.previewCode == 19 && records.refreshes == refreshes + 1);
  assert(app.classifiedCounts[19]==0 && app.classifiedCounts[72]==0 && app.classifiedCounts[28]==1);

  frame = Frame(1, 2, 1, 1); frame[8] ^= 1;
  page.wMCom3->packets.push_back(frame);
  Notify(page);
  assert(Pending() == 0 && app.commandHistory.size() == 1); // Corrupt frame rejected.

  burst.clear();
  for (unsigned n = 1; n <= 300; ++n) {
    frame = Frame(1, n, 1, 1); burst.insert(burst.end(), frame.begin(), frame.end());
  }
  page.wMCom3->packets.push_back(burst);
  Notify(page);
  RunPendingUi();
  assert(app.commandHistory.size() == 256 && app.commandHistory.front().sequence == 45);
  assert(app.classifiedCounts[1]==300 && app.classifiedCounts[28]==1);
  // Same shape/different color and same color/different shape stay separate,
  // even when firmware assigns the same tray ID to those different codes.
  burst.clear();
  for (auto bytes : {Frame(1,301,1,2), Frame(1,302,2,2), Frame(1,303,10,2)})
    burst.insert(burst.end(),bytes.begin(),bytes.end());
  page.wMCom3->packets.push_back(burst); Notify(page); RunPendingUi();
  assert(app.classifiedCounts[1]==301 && app.classifiedCounts[2]==1 && app.classifiedCounts[10]==1);

  // White production and preview are decoded on the same UI-thread path.
  for (unsigned shape=0; shape<8; ++shape) {
    frame = Frame(1, 310+shape, uint8_t(73+shape), uint8_t(shape%6+1));
    frame.push_back(uint8_t(73+shape));
    page.wMCom3->packets.push_back(frame);
    Notify(page);
    assert(Pending()==1);
    RunPendingUi();
    assert(app.commandHistory.back().code==73+shape && app.previewCode==73+shape);
    assert(app.currentShapeIndex==int(shape) && app.currentColorIndex==9);
    assert(app.classifiedCounts[73+shape]==1);
    assert(testVars.last.find("白")!=string::npos);
  }
  // The appended cone codes also use independent shape/color counters.
  for (unsigned color=0;color<10;++color) {
    frame=Frame(1,350+color,uint8_t(81+color),uint8_t(color%6+1));
    frame.push_back(uint8_t(81+color));
    page.wMCom3->packets.push_back(frame); Notify(page); RunPendingUi();
    assert(app.commandHistory.back().code==81+color);
    assert(app.currentShapeIndex==8 && app.currentColorIndex==int(color));
    assert(app.classifiedCounts[81+color]==1 && app.previewCode==81+color);
    assert(testVars.last.find("圆锥")!=string::npos);
  }
  page.wMCom3->packets.push_back(Frame(1,390,90,1)); Notify(page); RunPendingUi();
  assert(app.classifiedCounts[90]==2 && app.classifiedCounts[81]==1);
  assert(app.classifiedCounts[80]==1); // White sphere remains a separate class.
  page.wMCom3->packets.push_back(Frame(1, 400, 91, 1));
  Notify(page); assert(Pending()==0);
  assert(app.classifiedCounts[80]==1);

  testPages.page = nullptr;
  page.wMCom3->packets.push_back(Frame(2, 0, 0, 0));
  Notify(page);
  RunPendingUi();
  assert(testPages.opens == 0 && app.commandHistory.empty()); // Clear alone opens nothing.
  for (unsigned code=0;code<=HMI_CODE_MAX;++code) assert(app.classifiedCounts[code]==0);
  page.wMCom3->packets.push_back(vector<uint8_t>(1, 72));
  Notify(page);
  RunPendingUi();
  assert(testPages.opens == 1 && app.previewCode == 72);
  assert(app.classifiedCounts[72]==0);

  // Dispose drops both a late notification and an already queued UI batch.
  page.wMCom3->packets.push_back(Frame(1, 4, 5, 2));
  Notify(page);
  assert(Pending() == 1);
  refreshes = records.refreshes;
  page.onDispose();
  assert(page.wMCom3->closed && !page.active);
  RunPendingUi();
  assert(app.commandHistory.empty() && records.refreshes == refreshes);
  assert(app.classifiedCounts[5]==0);
  unsigned reads = page.wMCom3->reads;
  Notify(page);
  assert(page.wMCom3->reads == reads);

  // A failed async start must not fall back to touching the UI off-thread.
  MockWorkerState::startAllowed() = false;
  Wcmn failure(&app, nullptr, "Wcmn");
  failure.createParts(); failure.onLoad();
  failure.wMCom3->packets.push_back(Frame(1, 5, 19, 3));
  Notify(failure);
  assert(failure.droppedUpdates == 1 && Pending() == 0);
  assert(app.commandHistory.empty() && MockWorkerState::inUi() == false);
  assert(app.classifiedCounts[19]==0);
  MockWorkerState::startAllowed() = true;
  failure.onDispose();

  std::puts("PASS Wcmn worker: zero-wait read, one UI hop per batch, "
            "parse/CRC/split/timeout, bounded history, dispose, start failure");
  return 0;
}