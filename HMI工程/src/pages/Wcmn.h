#ifndef SRC_PAGES_WCMN_H
#define SRC_PAGES_WCMN_H

#include <cstdint>
/*CUSTOM_HEAD_BEGIN*/
#include <atomic>
#include "../HmiRecord.h"
/*CUSTOM_HEAD_END*/
#include "../HmiApp.h"
#include "widgets/Widgets.h"
#include "type/Types.h"
#include "misc/misc.h"
#include "op/DataSample.h"
#include "op/EventMgr.h"
#include "op/RCP.h"
#include "op/OpLog.h"
#include "util/DateUtil.h"
#include "util/StringUtil.h"

using namespace std;
using namespace stt;
using namespace stt::op;
using namespace stt::db;
using namespace stt::misc;
using namespace stt::data;
using namespace stt::util;
using namespace stt::constant;

/********************************
 *
 *  页面类
 *
 *  注意：用户可在指定位置添加修改内容
 *

 ********************************/
class HmiApp;

class Wcmn: public STTPage {
public:
	STTCom *wMCom3;
	void wMCom3_vch_cb(uint16_t code, LvEvent e);

public:
	Wcmn(HmiApp *app, STTWidget *par, const string &id, const string &desc = "");
	virtual ~Wcmn();
	void onInit();
	void createParts();
	void onLoad();
	bool onClosing();
	void onDispose();
	/*CUSTOM_CODE_BEGIN*/
  // COM 接收线程：只解析，不碰变量/页面。
  void consumeSerial(const vector<uint8_t> &bytes);
  // UI 线程：应用一帧 / 一整批，并重建一次列表。
  void applyEvent(const HmiEvent &event);
  void applyEvents(const vector<HmiEvent> &events);
  HmiParser parser;                  // 只由 COM 接收线程访问，无需加锁
  std::atomic<bool> active{false};   // onLoad 置位，onDispose 清零
  unsigned droppedUpdates = 0;       // 仅接收线程写：异步跳转启动失败的批次数
	/*CUSTOM_CODE_END*/

protected:
	HmiApp *hmiApp;
};

#endif
