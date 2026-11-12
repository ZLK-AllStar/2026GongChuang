/*
 * 指示条
 *
 *  Created on: 2023年2月18日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 指示条/进度条/柱状图
 * @image html image/bar_main.png
 */
class STTBar: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 指示条/进度条构造
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTBar(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 指示条/进度条析构
	 */
    virtual ~STTBar();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x 		X坐标(默认0)
     * @param y 		Y坐标(默认0)
     * @param w 		宽度(默认200)
     * @param h 		高度(默认50)
     * @param v 		当前值(默认50)
     * @param min 		最小值(默认0)
     * @param max 		最大值(默认100)
     */
    void setMainArg(int x=0, int y=0, int w=200, int h=50, int v=50, int min=0, int max=100);
    /**
     * @LEVEL2
     * @brief 设置背景样式参数
     * @param r 		圆角半径，默认9999
     * @param c1 		背景主色(默认0xdddddd)
     * @param c2 		背景渐变次色(默认0xdddddd)
     * @param dir 		背景渐变方向(默认0~不渐变) [1~从上到下 2~从左到右]
     * @param b_w	 	背景边框宽度(默认0)
     * @param b_c 		背影边框颜色(默认黑)
     * @param s_w	 	阴影宽度(默认0)
     * @param s_c	 	阴影颜色(默认黑)
     * @param image		图片，默认无
     */
    void setMainStyle(int r=9999, int c1=0xdddddd, int c2=0xdddddd, int dir=0, int b_w = 0, int b_c=0x0, int s_w=0, int s_c=0x0, const string& image = "");
    /**
     * @LEVEL2
     * @brief 设置指示条样式参数
     * @param r 		指示条圆角半径，默认9999
     * @param c1  		指示条主色(默认0x1a2b3)
     * @param c2  		指示条渐变次色(默认0x1a2b3)
     * @param dir 		指示条渐变方向(默认0~不渐变) [1~从上到下 2~从左到右]
     * @param b_w		指示条边框宽度(默认0)
     * @param b_c 		指示条边框颜色(默认黑)
     * @param image		指示条图片，默认无
     */
    void setIndicStyle(int r=9999, int c1=0x1a2b3, int c2=0x1a2b3, int dir=0, int b_w=0, int b_c=0x0, const string& img = "");
    /**
     * @LEVEL0
     * @brief 设置当前值
     * @param value 	当前值 默认50
     * @param anim		true:动画  false：直接改变
     *
     * 如需显示动画，先设置动画时间getStyle()->p_anim->setTime(1000);
     */
    STTBar* setValue(int value=50, bool anim=true);
    /**
     * @LEVEL1
     * @brief 设置起始值，范围模式有效
     * @param value		指示条起始位置，默认0
     * @param anim		true:动画  false：直接改变
     * @return
     */
    STTBar* setStartValue(int value=0, bool anim=true);
    /**
     * @LEVEL0
     * @brief 设置上下限
     * @param min 		指示条最左(上)端对应的值 默认0
     * @param max 		指示条最右(下)端对应的值 默认100
     */
    STTBar* setRange(int min=0, int max=100);
    /**
     * @LEVEL2
     * @brief 设置可变上下限
     * @param lowVar 下限变量名
     * @param upVar  上限变量名
     */
    STTBar* setVariableLimit(const string& lowVar = "", const string& upVar = "");
    /**
     * @LEVEL1
     * @brief 设置指示条工作模式
     * @param mode 		默认0~普通模式(最小值在最左端)
     * 					1~对称模式(从正中间往两边画指示条，当前值小于(0)时向左画，当前值大于(0)时向右画。最小值小于0时，才能看出效果)
     * 					2~范围模式(从StartValue画到当前值)
     */
    STTBar* setMode(int mode=0);
    /**
     * @LEVEL0
     * @brief 获取当前值
     */
    int getValue();
    /**
     * @LEVEL1
     * @brief 获取起始值
     */
    int getStartValue();
    /**
     * @LEVEL0
     * @brief 获取下限值
     */
    int getMinValue();
    /**
     * @LEVEL0
     * @brief 获取上限值
     */
    int getMaxValue();
    /**
     * @LEVEL1
     * @brief 获取指示条模式
     */
    int getMode();
    /**
     * @LEVEL2
     * @brief 响应变量变化，改变当前进度值
     * @param org		来源控件。如果来源控件为当前控件，则跳过
     * @param data		变量
     */
    void onDataChanged(SttObject * org, IVariable* data);
private:
    /**
     * @HideCA
     * @brief 内部指针
     **/
    void* mBar;
    /**
     * @HideCA
     * @brief 响应变量变化，改变控件最小值
     * @param org		来源控件。如果来源控件为当前控件，则跳过
     * @param data		变量
     */
    void onMinDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 响应变量变化，改变控件最大值
     * @param org		来源控件。如果来源控件为当前控件，则跳过
     * @param data		变量
     */
    void onMaxDataChanged(SttObject * org, IVariable* data);

};
typedef STTBar* SttBar;

}

