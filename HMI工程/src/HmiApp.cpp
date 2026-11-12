/**
 * 应用程序源码文件
 */
#ifndef STT_HMIAPP_CPP_
#define STT_HMIAPP_CPP_

/*********************
 *   头文件
 *********************/
#include "HmiApp.h"
/*PAGES_HEAD_BEGIN*/
#include "pages/Frm02.h"
#include "pages/Frm03.h"
#include "pages/Wcmn.h"
/*PAGES_HEAD_END*/

using namespace stt;
using namespace stt::data;

/********************************
 *
 *  应用程序入口
 *
 *  注意：IDE自动生成，用户勿修改
 *
 ********************************/
IApplication * AppMain(IRuntime* env, char **argv){
	return new HmiApp(env);
}

string getAppArgs() {
	return "{\"r\":0}";
}

/********************************
 *
 *  应用程序类
 *
 *  注意：用户可添加代码
 *
 ********************************/
HmiApp::HmiApp(IRuntime * rt) : HmiAppBase(rt) {
}

HmiApp::~HmiApp() {
}

/********************************
 *
 *  应用构建前调用此函数
 *
 *  注意：用户可添加代码
 *
 ********************************/
void HmiApp::onInit(){
	//(空 — 几何体形状/颜色通过 HmiApp::currentShapeIndex/currentColorIndex 传递)
}

/********************************
 *
 *  应用构建函数
 *
 *  注意：IDE自动生成，用户勿修改
 *
 ********************************/
void HmiApp::construct() {
	//注册字体文件
	fm->registerFontFile("文泉驿微米黑", "./fnt/wenquanyiweimihei.ttf");
	//常规变量
	vm->add("接收指令", DataType::T_VCHAR, R"({"id":"v2"})");
	vm->add("串口", DataType::T_VCHAR, R"({"id":"v3"})");
	vm->add("Signal", DataType::T_INT, R"({"id":"v4"})");
	vm->add("完成任务", DataType::T_INT, R"({"id":"v5"})");
	vm->add("满载情况", DataType::T_INT, R"({"id":"v6"})");
	vm->add("正四面体-红", DataType::T_INT, R"({"id":"v7"})");
	vm->add("正四面体-橙", DataType::T_INT, R"({"id":"v8"})");
	vm->add("正四面体-黄", DataType::T_INT, R"({"id":"v9"})");
	vm->add("正四面体-绿", DataType::T_INT, R"({"id":"v10"})");
	vm->add("正四面体-青", DataType::T_INT, R"({"id":"v11"})");
	vm->add("正四面体-蓝", DataType::T_INT, R"({"id":"v12"})");
	vm->add("正四面体-紫", DataType::T_INT, R"({"id":"v13"})");
	vm->add("瓷片", DataType::T_INT, R"({"id":"v14"})");
	vm->add("鹅卵石", DataType::T_INT, R"({"id":"v15"})");
	vm->add("砖块", DataType::T_INT, R"({"id":"v16"})");
	vm->add("有害垃圾", DataType::T_INT, R"({"id":"v17"})");
	vm->add("可回收垃圾", DataType::T_INT, R"({"id":"v18"})");
	vm->add("厨余垃圾", DataType::T_INT, R"({"id":"v19"})");
	vm->add("其他垃圾", DataType::T_INT, R"({"id":"v20"})");
	vm->add("状态", DataType::T_VCHAR, R"({"id":"v21"})");
	vm->add("变量_22", DataType::T_INT, R"({"id":"v22"})");
	vm->add("分拣记录列表", DataType::T_INT, R"({"id":"v23"})");
	//背光、屏保
	pm->setBackLightHoldingTime(0, 50);
	//公共窗体
	regCmdWindow(cWnd, new Wcmn(this, nullptr, "Wcmn"));
}

IPagePtr HmiApp::buildPage(string id, uint8_t mode, STTWidget *target) {
	if (id == "Frm03")
		return new Frm03(this, target, "Frm03", "Frm03");
	return new Frm02(this, target, "Frm02", "Frm02");
}

/********************************
 *
 *  构建应用后调用此函数
 *
 *  注意：用户可添加代码
 *
 ********************************/
void HmiApp::onReady(){
	//用户在此添加客户化代码
}

/********************************
 *
 *  应用退出前调用此函数
 *
 *  注意：用户可添加代码
 *
 ********************************/
void HmiApp::onQuit(){
	//用户在此添加客户化代码
}


#endif /* STT_HMIAPP_CPP_ */


