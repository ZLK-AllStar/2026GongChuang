/*
 * STTSwitch.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 切换按钮
 */
class STTSwitch: public STTWidget {
public:
	/**
	 * @HideCA
     * @brief 切换按钮构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
	 */
	STTSwitch(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTSwitch();
	/**
	 * @LEVEL0
	 * @brief 开启
	 */
	STTSwitch* on();
	/**
	 * @LEVEL0
	 * @brief 关闭
	 */
	STTSwitch* off();
	/**
	 * @LEVEL0
	 * @brief 开关切换
	 */
	STTSwitch* toggle();
	/**
	 * @LEVEL0
	 * @brief 是否开启状态
	 */
	bool isOn();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x				位置x
     * @param y				位置y
     * @param w				宽度
     * @param h				高度
     * @param ison			初始状态: true~开启 false~关闭
     * @param isbeep 		是否打开蜂鸣器
     */
    void setMainArg(int x=0, int y=0, int w=20, int h=15, bool ison = false, bool isbeep=false);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆前半径
     * @param c1		主色(渐变时的起始色)
     * @param c2		渐变时的终止色
     * @param dir		渐变方向 0~不渐变  1~上下  2~左右
     * @param w_border	边框线粗细
     * @param c_border	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
     */
    void setMainStyle(int r, int c1, int c2, int dir, int w_border, int c_border, int w_shadow, int c_shadow);
    /**
     * @LEVEL2
     * @brief 设置指示条样式
     * @param r			圆前半径
     * @param c1		主色(渐变时的起始色)
     * @param c2		渐变时的终止色
     * @param dir		渐变方向 0~不渐变  1~上下  2~左右
     * @param w_border	边框线粗细
     * @param c_border	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
     */
    void setIndicStyle(int r, int c1, int c2, int dir, int w_border, int c_border, int w_shadow, int c_shadow);

    /**
     * @LEVEL2
     * @brief 设置滑动块样式参数
     * @param r			圆前半径
     * @param c1		主色(渐变时的起始色)
     * @param c2		渐变时的终止色
     * @param dir		渐变方向 0~不渐变  1~上下  2~左右
     * @param w_border	边框线粗细
     * @param c_border	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
     */
    void setKnobStyle(int r, int c1, int c2, int dir, int w_border, int c_border, int w_shadow, int c_shadow);
	/**
	 * @HideCA
	 */
    void onDataChanged(SttObject * org, IVariable* data);
	/**
	 * @HideCA
	 */
    void onConfirm(uint16_t code, LvEvent e);
};

typedef STTSwitch* SttSwitch;

}
