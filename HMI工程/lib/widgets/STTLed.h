/*
 * STTLed.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief LED
 */
class STTLed: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建LED灯控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTLed(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTLed();

    /**
     * @Level2
     * @brief 设置基本参数
     * @param x		x坐标
     * @param y		y坐标
     * @param w		宽
     * @param h		高
     * @param ison	初始是否点亮
     * @param c		颜色
     * @param r		圆角半径
     */
    void setMainArg(int x=0, int y=0, int w=50, int h=50, bool ison=true, int c=0x1a2b3, int r = 9999);
    /**
     * @Level0
     * @brief 设置LED颜色
     * @param color
     */
    STTLed* setColor(int color=0xff00);
    /**
     * @Level0
     * @brief 设置亮度
     * @param bright	亮度，有效范围80...255
     */
    STTLed* setBrightness(int bright=255);
    /**
     * @Level0
     * @brief 点亮
     */
    STTLed* on();
    /**
     * @Level0
     * @brief 熄灭
     */
    STTLed* off();
    /**
     * @Level0
     * @brief 切换点亮或熄灭
     */
    STTLed* toggle();
    /**
     * @Level0
     * @brief 获得当前亮度值
     */
    uint8_t getBrightness();
public:
    /**
     * @HideCA
     * @brief 变量变化处理方法
     */
    void onDataChanged(SttObject * org, IVariable* data);
};

typedef STTLed* SttLed;

}
