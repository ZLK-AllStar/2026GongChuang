#include "Wcmn.h"
#include "Frm02.h"
#include "cstdio"
#include <string>
/*CUSTOM_HEAD_BEGIN*/
#include <time.h>
/*CUSTOM_HEAD_END*/

Wcmn::Wcmn(HmiApp* app, STTWidget *par, const string& id, const string& desc) : STTPage(app->getRuntime(), par, id, desc), wMCom3(nullptr) {
	hmiApp = app;
}

Wcmn::~Wcmn() {
}

void Wcmn::onInit() {

}

void Wcmn::createParts() {
	setMainArg(0, 0, 1024, 600);
	setMainStyle(0, 0xffffff, 0xffffff, 0, 255, 0, 0xc0c0c0);

	/*----------------------------- Com : wMCom3                         -----------------------------*/
	wMCom3 = new STTCom(p_rt, this, "wMCom3", true);
	wMCom3->set(R"({"b":"115200","d":"15","e":"1","f":"1024","g":"1000","h":"","l":"","m":"0","o":"1","p":"COM2","r":"0","s":"8","t":""})");
	wMCom3->open();
	wMCom3->onEventHandler(Event::VALUE_CHANGED, this, (EHandler) &Wcmn::wMCom3_vch_cb);

	/*called after widget created.*/

	return; //createParts END
}

void Wcmn::onLoad() {
	active = true;
}

bool Wcmn::onClosing() {
	return true;
}

void Wcmn::onDispose() {
	active = false; // 已排队的接收/异步回调不得再碰界面。
	if (wMCom3) {
		wMCom3->close();
	}
}

/*CUSTOM_CODE_BEGIN*/
static uint32_t monotonicMs() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return uint32_t(uint64_t(ts.tv_sec) * 1000U + ts.tv_nsec / 1000000U);
}

void Wcmn::wMCom3_vch_cb(uint16_t code, LvEvent e) {
  // 运行在 COM2 接收线程：只读取和解析，界面更新一律交给 applyEvents。
  (void)code; (void)e;
  if (!active || !wMCom3) return;
  vector<uint8_t> bytes;
  // 必须零等待：正数超时会在接收线程里进入 SttLooper::exec，重入 SDK 事件循环。
  if (!wMCom3->readData(bytes, 0) || bytes.empty()) return;
  consumeSerial(bytes);
}

void Wcmn::consumeSerial(const vector<uint8_t> &bytes) {
  const uint32_t now = monotonicMs();
  vector<HmiEvent> events;
  for (size_t i = 0; i < bytes.size(); ++i) {
    HmiEvent event;
    if (parser.feed(bytes[i], now, event)) events.push_back(event);
  }
  if (events.empty()) return;
  // 每批只跳一次 UI 线程；invoke 留空，界面操作只在 onFinished 里做。
  IAsynWorker *worker = new AsynWorker(hmiApp);
  worker->invoke([]() {});
  worker->onFinished([this, events]() { applyEvents(events); });
  if (!worker->start()) ++droppedUpdates; // 不做跨线程直接刷新的降级。
}

void Wcmn::applyEvents(const vector<HmiEvent> &events) {
  if (!active) return;
  bool showRecords = false;
  for (size_t i = 0; i < events.size(); ++i) {
    applyEvent(events[i]);
    if (events[i].type != 2) showRecords = true;
  }
  Frm02 *page = static_cast<Frm02 *>(pm->getPage("Frm02"));
  if (page) page->syncCommandHistory();
  else if (showRecords) pm->open("Frm02", PageMode::NEWWIN);
}

void Wcmn::applyEvent(const HmiEvent &event) {
  if (event.type == 2) {
    hmiApp->commandHistory.clear();
    std::memset(hmiApp->classifiedCounts, 0, sizeof(hmiApp->classifiedCounts));
    hmiApp->previewCode = 0;
    vm->setChar("串口", "新生产会话 / 历史已清空", 0);
    return;
  }
  const HmiRecord &record = event.record;
  if (!HmiCode_IsValid(record.code)) return;
  int shape = HmiCode_Shape(record.code);
  int color = HmiCode_Color(record.code);
  hmiApp->currentShapeIndex = shape;
  hmiApp->currentColorIndex = color;
  static const char* colors[] = {"红","橙","黄","绿","青","蓝","紫","灰","黑","白"};
  static const char* shapes[] = {"正四面体","三棱柱","五棱柱","六棱柱","圆柱形","长方体","正方体","球体","圆锥"};
  char text[128];
  if (event.type == 0) {
    hmiApp->previewCode = record.code;
    std::snprintf(text, sizeof(text), "调试预览 / 无生产 ID 指令:%u %s-%s", record.code, shapes[shape], colors[color]);
  } else {
    hmiApp->previewCode = 0;
    std::snprintf(text, sizeof(text), "ID:%u 指令:%u %s-%s", record.id, record.code, shapes[shape], colors[color]);
    ++hmiApp->classifiedCounts[record.code];
    hmiApp->commandHistory.push_back(record);
    if (hmiApp->commandHistory.size() > 256) hmiApp->commandHistory.erase(hmiApp->commandHistory.begin());
  }
  vm->setChar("串口", text, 0);
}
/*CUSTOM_CODE_END*/
