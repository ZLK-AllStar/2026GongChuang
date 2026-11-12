#include "Frm02.h"
#include "cstdio"

Frm02::Frm02(HmiApp* app, STTWidget *par, const string& id, const string& desc) : STTPage(app->getRuntime(), par, id, desc) {
	hmiApp = app;
}

Frm02::~Frm02() {
}

void Frm02::onInit() {

}

void Frm02::createParts() {
	setMainArg(0, 0, 1024, 600);
	setMainStyle(0, 0xe9457, 0xffffff, 0, 255, 0, 0xc0c0c0);

	/*----------------------------- Button : wMButton2                   -----------------------------*/
	wMButton2 = new STTButton(p_rt, this, "wMButton2", 0);
	wMButton2->setMainArg(23, 13, 54, 27, false);
	wMButton2->setPressedArg(" 返回", 0x0, "文泉驿微米黑", 16, 0);
	wMButton2->setPressedStyle(8, 255, 0xebebeb, 0x999999, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton2->setReleasedArg(" 返回", 0x0, "文泉驿微米黑", 16, 0);
	wMButton2->setReleasedStyle(5, 255, 0x20aff2, 0x79cae0, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton2->bindOpenFormHandler(Event::RELEASED, "Frm03", PageMode::NEWWIN, NULL);

	/*----------------------------- List : wMList4                       -----------------------------*/
	wMList4 = new STTList(p_rt, this, "wMList4");
	wMList4->setMainArg(22, 57, 971, 507, false);
	wMList4->setFonts(0x0, 0xffffff, 16, "文泉驿微米黑", 0);
	wMList4->setMainStyle(10, 0xffffff, 0xffffff, 0, 2, 0xc0c0c0);
	wMList4->setButtonStyleSelected(0, 0x1a2b1, 0xffffff, 0, 0);
	/*called after widget created.*/

	return; //createParts END
}

void Frm02::onLoad() {
	syncCommandHistory();
}

/*CUSTOM_CODE_BEGIN*/
void Frm02::syncCommandHistory() {
  if (!wMList4 || !hmiApp) return;
  // Rebuild from the sole bounded history, including eviction/session clear.
  // Reopening the page therefore never allocates IDs or duplicates records.
  wMList4->clearOptions();
  // SDK 弃用行：clearOptions() 之后创建的第一个子行不会套用列表文字样式，
  // 标签保持白色，白底列表上等于看不见。这里用只带图片的表头行吸收该状态：
  // 表头文字已画进 ./images/HDR-RECORDS.png，真实记录从第二行起正常显示。
  if (!hmiApp->commandHistory.empty() || hmiApp->previewCode) {
    wMList4->addButton("./images/HDR-RECORDS.png", "");
  }
  for (const HmiRecord &record : hmiApp->commandHistory) appendRecord(record);
  if (hmiApp->previewCode) { HmiRecord preview; preview.code = hmiApp->previewCode; appendRecord(preview); }
}

void Frm02::appendRecord(const HmiRecord &record) {
  int cmd = record.code;
	if (!wMList4 || !HmiCode_IsValid(cmd)) return;

	static const char* colorNames[] = {"红", "橙", "黄", "绿", "青", "蓝", "紫", "灰", "黑", "白"};
	static const char* colorFiles[] = {"RED", "ORANGE", "YELLOW", "GREEN", "CYAN", "BLUE", "PURPLE", "GRAY", "BLACK", "WHITE"};
	static const char* shapeNames[] = {"正四面体", "三棱柱", "五棱柱", "六棱柱", "圆柱形", "长方体", "正方体", "球体", "圆锥"};
	static const char* shapeFiles[] = {"RT", "TP", "PP", "HP", "CY", "CB", "CU", "SP", "CO"};

	int shape = HmiCode_Shape(cmd);
	int color = HmiCode_Color(cmd);

	char imagePath[64];
	char text[160];
	std::sprintf(imagePath, "./images/%s-%s.png", shapeFiles[shape], colorFiles[color]);
	std::snprintf(text, sizeof(text), "ID:%u  指令:%d %s-%s  同类总数:%llu", record.id, cmd, shapeNames[shape], colorNames[color],
                static_cast<unsigned long long>(hmiApp->classifiedCounts[cmd]));
	if (record.id == 0) std::snprintf(text, sizeof(text), "调试预览 / 无生产 ID 指令:%d %s-%s", cmd, shapeNames[shape], colorNames[color]);
	wMList4->addButton(imagePath, text);

	// STTList的滚动接口在样式对象中；大Y值会自动限制到内容底部。
	STTStyle* style = wMList4->getStyle();
	if (style && style->p_scrollbar) {
		style->p_scrollbar->scrollToY(32767, 1); // 1: 立即滚动
	}
}

/*CUSTOM_CODE_END*/

bool Frm02::onClosing() {
	return true;
}

void Frm02::onDispose() {

}
