#pragma once
// Host-only SDK stand-ins. The test compiles the real Wcmn.cpp method bodies.
//
// The mock models the two SDK thread roles that matter here:
//   * the COM receive thread runs wMCom3_vch_cb()
//   * the UI thread runs IAsynWorker::onFinished() callbacks
// Every UI-touching mock asserts it is inside onFinished, so a regression that
// writes variables/pages from the receive callback fails the test.
#include <atomic>
#include <cassert>
#include <cstdint>
#include <deque>
#include <functional>
#include <string>
#include <vector>
#include <time.h>
#include "HmiRecord.h"
using namespace std;

extern uint32_t testNow;
inline int HmiMockClock(int, struct timespec *ts) {
  ts->tv_sec = testNow / 1000;
  ts->tv_nsec = (testNow % 1000) * 1000000;
  return 0;
}
#undef clock_gettime
#define clock_gettime HmiMockClock

struct LvEvent {};
struct SttObject { virtual ~SttObject() {} };
struct STTWidget : SttObject {};
typedef void (SttObject::*EHandler)(uint16_t, LvEvent);
struct Event { enum { VALUE_CHANGED = 31 }; };
struct PageMode { enum { NEWWIN = 0 }; };

// ---- UI-thread bookkeeping -------------------------------------------------
struct MockWorkerState {
  static vector<function<void()> > &pending() { static vector<function<void()> > q; return q; }
  static bool &inUi() { static bool ui = false; return ui; }
  static bool &startAllowed() { static bool allowed = true; return allowed; }
  static unsigned &created() { static unsigned n = 0; return n; }
  static unsigned &started() { static unsigned n = 0; return n; }
};

inline void RunPendingUi() {
  vector<function<void()> > jobs;
  jobs.swap(MockWorkerState::pending());
  MockWorkerState::inUi() = true;
  for (size_t i = 0; i < jobs.size(); ++i) if (jobs[i]) jobs[i]();
  MockWorkerState::inUi() = false;
}

// ---- widgets used by Wcmn --------------------------------------------------
struct Frm02 : STTWidget {
  unsigned refreshes = 0;
  void syncCommandHistory() { assert(MockWorkerState::inUi()); ++refreshes; }
};
struct MockPages {
  Frm02 *page = nullptr;
  unsigned opens = 0;
  STTWidget *getPage(const string &) { return page; }
  void open(const string &, int) { assert(MockWorkerState::inUi()); ++opens; }
};
struct MockVars {
  string last;
  void setChar(const string &, const string &s, int) {
    assert(MockWorkerState::inUi()); // variables are UI-thread state here
    last = s;
  }
};
extern MockPages testPages;
extern MockVars testVars;

struct HmiApp {
  vector<HmiRecord> commandHistory;
  uint64_t classifiedCounts[HMI_CODE_MAX + 1U] = {};
  uint8_t previewCode = 0;
  int currentShapeIndex = 0, currentColorIndex = -1;
  void *getRuntime() { return nullptr; }
};

struct STTPage : STTWidget {
  void *p_rt;
  MockPages *pm = &testPages;
  MockVars *vm = &testVars;
  STTPage(void *rt, STTWidget *, const string &, const string &) : p_rt(rt) {}
  template<class... T> void setMainArg(T...) {}
  template<class... T> void setMainStyle(T...) {}
};

struct STTCom {
  deque<vector<uint8_t> > packets;
  unsigned reads = 0;
  bool closed = false;
  function<void()> beforeRead;
  STTCom(void *, SttObject *, const string &, bool) {}
  bool set(const string &) { return true; }
  bool open() { return true; }
  void onEventHandler(uint16_t, SttObject *, EHandler) {}
  bool close() { closed = true; return true; }
  bool readData(vector<uint8_t> &bytes, int timeout) {
    assert(timeout == 0);              // A waiting read would pump the SDK event loop.
    assert(!MockWorkerState::inUi());  // Never read the port from the UI callback.
    ++reads;
    if (beforeRead) beforeRead();
    if (packets.empty()) return false;
    bytes = packets.front(); packets.pop_front();
    return !bytes.empty();
  }
};

// ---- IAsynWorker / AsynWorker ---------------------------------------------
struct IAsynWorker {
  virtual ~IAsynWorker() {}
  virtual IAsynWorker *invoke(function<void()> f) = 0;
  virtual IAsynWorker *onFinished(function<void()> f) = 0;
  virtual bool start() = 0;
  virtual int flag() = 0;
};

struct AsynWorker : IAsynWorker {
  function<void()> work, done;
  explicit AsynWorker(HmiApp *) { ++MockWorkerState::created(); }
  IAsynWorker *invoke(function<void()> f) { work = f; return this; }
  IAsynWorker *onFinished(function<void()> f) { done = f; return this; }
  int flag() { return 1; }
  bool start() {
    ++MockWorkerState::started();
    if (!MockWorkerState::startAllowed()) return false;
    if (work) work();                                  // background part: no UI allowed
    MockWorkerState::pending().push_back(done);        // UI part runs on RunPendingUi()
    return true;
  }
};