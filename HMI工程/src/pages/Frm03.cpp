#include "Frm03.h"

Frm03::Frm03(HmiApp* app, STTWidget *par, const string& id, const string& desc) : STTPage(app->getRuntime(), par, id, desc) {
	hmiApp = app;
}

Frm03::~Frm03() {
}

void Frm03::onInit() {

}

void Frm03::createParts() {
	setMainArg(0, 0, 1024, 600);
	setMainStyle(0, 0xffffff, 0xffffff, 0, 255, 0, 0xc0c0c0);

	/*----------------------------- Button : wMButton2                   -----------------------------*/
	wMButton2 = new STTButton(p_rt, this, "wMButton2", 0);
	wMButton2->setMainArg(810, 163, 150, 54, false);
	wMButton2->setPressedArg("重启", 0x0, "文泉驿微米黑", 16, 0);
	wMButton2->setPressedStyle(8, 255, 0xebebeb, 0x999999, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton2->setReleasedArg("重启", 0x0, "文泉驿微米黑", 16, 0);
	wMButton2->setReleasedStyle(5, 255, 0xebebeb, 0x999999, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton2->opDataObj(Event::PRESSED, nullptr, "11");

	/*----------------------------- Button : wMButton9                   -----------------------------*/
	wMButton9 = new STTButton(p_rt, this, "wMButton9", 0);
	wMButton9->setMainArg(807, 71, 155, 50, false);
	wMButton9->setPressedArg("主菜单", 0x0, "文泉驿微米黑", 16, 0);
	wMButton9->setPressedStyle(8, 255, 0xebebeb, 0x999999, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton9->setReleasedArg("主菜单", 0x0, "文泉驿微米黑", 16, 0);
	wMButton9->setReleasedStyle(5, 255, 0xebebeb, 0x999999, 1, 1, 0x999999, 1, 0xebebeb);
	wMButton9->bindOpenFormHandler(Event::PRESSED, "Frm02", PageMode::NEWWIN, NULL);

	/*----------------------------- Label : wMLabel1                     -----------------------------*/
	wMLabel1 = new STTLabel(p_rt, this, "wMLabel1");
	wMLabel1->setMainArg(333, 287, 388, 116, "R.C.C.O", TextAlign::LEFT, LabelLongMode::CLIP, 30);
	wMLabel1->setFonts(0x875daa, 60, "文泉驿微米黑", 0);
	wMLabel1->setMainStyle(0, 0xffffff, 0xffffff, GradDir::NONE, 0, 0x0, 0);
	wMLabel1->setFormatText(TextAlign::LEFT, LabelLongMode::CLIP, 30);

	/*----------------------------- Label : wMLabel2                     -----------------------------*/
	wMLabel2 = new STTLabel(p_rt, this, "wMLabel2");
	wMLabel2->setMainArg(275, 121, 329, 200, "智能分拣", TextAlign::CENTER, LabelLongMode::CLIP, 30);
	wMLabel2->setFonts(0xd2effd, 60, "文泉驿微米黑", 0);
	wMLabel2->setMainStyle(0, 0xffffff, 0xffffff, GradDir::NONE, 0, 0x0, 0);
	wMLabel2->setFormatText(TextAlign::CENTER, LabelLongMode::CLIP, 30);

	/*called after widget created.*/

	return; //createParts END
}

void Frm03::onLoad() {
}

bool Frm03::onClosing() {
	return true;
}

void Frm03::onDispose() {

}
