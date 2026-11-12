/****************************************************************
 * 应用程序头文件
 * 注意: 用户手动修改
 ****************************************************************/
#ifndef STT_HMIAPP_H_
#define STT_HMIAPP_H_

/**********************
 * 包含文件
 **********************/
#include <iostream>
#include <vector>
#include "type/Types.h"
#include "core/SttObject.h"
#include "base/Interface.h"
#include "pages/Wcmn.h"
/*CUSTOM_HEAD_BEGIN*/
#include "HmiRecord.h"

/*CUSTOM_HEAD_END*/

/**********************
 * 命名空间声明
**********************/
using namespace stt;
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * 程序入口类
 **********************/
class HmiApp: public HmiAppBase {
public:
	HmiApp(IRuntime * rt);
	virtual ~HmiApp();
	void onInit();
	void construct();
	void onReady();
	void onQuit();
	IPagePtr buildPage(string id, uint8_t mode, STTWidget* target);
	STTPage* cWnd;

/*CUSTOM_CODE_BEGIN*/
	// 当前几何体状态 (UART 指令码 1~72 保留原表；73~80 为原8形状的白色；81~90 为圆锥各色，见 hmi_codes.h)
	int currentShapeIndex = 0;     // 0~8: 正四面体/三棱柱/五棱柱/六棱柱/圆柱形/长方体/正方体/球体/圆锥
	int currentColorIndex = -1;    // 0~9: 红橙黄绿青蓝紫灰黑白, -1: 无指令
	// 固件发送的生产记录历史（最多256条），预览不写入。
	vector<HmiRecord> commandHistory;
  // 本次生产会话累计，同一 code 即同形同色；不受256条历史淘汰影响。
  uint64_t classifiedCounts[HMI_CODE_MAX + 1U] = {};
  uint8_t previewCode = 0; // transient display only; never production history
/*CUSTOM_CODE_END*/

};

/**********************
 * APPMain
 *********************/
IApplication * AppMain(IRuntime * env, char **argv);
string getAppArgs();

#ifdef __cplusplus
}
#endif

#endif /* STT_HMIAPP_H_ */

