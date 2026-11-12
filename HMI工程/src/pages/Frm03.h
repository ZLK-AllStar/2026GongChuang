#ifndef SRC_PAGES_FRM03_H
#define SRC_PAGES_FRM03_H

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

class Frm03: public STTPage {
public:
	STTButton *wMButton2;
	STTButton *wMButton9;
	STTLabel *wMLabel1;
	STTLabel *wMLabel2;
public:
	Frm03(HmiApp *app, STTWidget *par, const string &id, const string &desc = "");
	virtual ~Frm03();
	void onInit();
	void createParts();
	void onLoad();
	bool onClosing();
	void onDispose();
	/*CUSTOM_CODE_BEGIN*/
	/*CUSTOM_CODE_END*/

protected:
	HmiApp *hmiApp;
};

#endif