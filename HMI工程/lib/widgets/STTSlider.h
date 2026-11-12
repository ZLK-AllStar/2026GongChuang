/*
 * STTSlider.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 滑块
 */
class STTSlider: public STTWidget {
public:
	/**
	 * @HideCA
     * @brief 滑块控件构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
	 */
	STTSlider(IRuntime* rt, STTWidget* par, const string& id="");
	/**
	 * @HideCA
	 */
	virtual ~STTSlider();
	/**
	 * @HideCA
	 */
	void setValueChangeMode(int8_t mode=0);
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h	位置、大小
     * @param v			初始值
     * @param minv		最小值
     * @param maxv		最大值
     * @param beep 		是否启用蜂鸣器
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=32, int v=50, int minv=0, int maxv=100, bool beep=false);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色
     * @param c2		背景渐变终止色
     * @param dir		渐变方向 0~不渐变 1~上下 2~左右
     * @param w_border	边框线粗
     * @param c_border	边框线色
     * @param w_shadow	阴影宽
     * @param c_shadow	阴影色
     * @param img		背景图片
     */
    void setMainStyle(int r=9999, int c1=0xdddddd, int c2=0xdddddd, int dir=0, int w_border=0, int c_border=0xdddddd, int w_shadow=0, int c_shadow=0x0, const void* img = NULL);
    /**
     * @LEVEL2
     * @brief 设置指示条样式
     * @param r			圆角半径
     * @param c1		背景主色
     * @param c2		背景渐变终止色
     * @param dir		渐变方向 0~不渐变 1~上下 2~左右
     * @param w_border	边框线粗
     * @param c_border	边框线色
     * @param w_shadow	阴影宽
     * @param c_shadow	阴影色
     * @param img		背景图片
     */
    void setIndicStyle(int r=9999, int c1=0x1a2b3, int c2=0x1a2b3, int dir=0, int w_border=0, int c_border=0xdddddd, int w_shadow=0, int c_shadow=0x0, const void* img = NULL);
    /**
     * @LEVEL2
     * @brief 设置Knob按钮样式参数
     * @param r			圆角半径
     * @param c1		背景主色
     * @param c2		背景渐变终止色
     * @param dir		渐变方向 0~不渐变 1~上下 2~左右
     * @param w_border	边框线粗
     * @param c_border	边框线色
     * @param w_shadow	阴影宽
     * @param c_shadow	阴影色
     * @param img		背景图片
     * @param r_mod		按钮的宽(高)。相对于指示条高度(竖向时则为宽度)的修正值(为0时，按钮与指示条等高)
     */
    void setKnobStyle(int r=9999, int c1=0x1a2b3, int c2=0xffffff, int dir=1, int w_border=0, int c_border=0x1a2b3, int w_shadow=0, int c_shadow=0x0, const void* img = NULL, int r_mod = 0);
    /**
     * @LEVEL0
     * @brief 修改slider值/set a new value on the slider
     * @param value     修改的值/the new value
     * @param anim      是否动画显示 1~是 0~否(立即改变)/LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
     *
     * 如需显示动画，先设置动画时间getStyle()->p_anim->setTime(1000);
     * 如果控件绑定了变量，最好不要设置动画效果。因为动画过程中，控件值一直在变化，就会导致变量不停的变化
     */
    STTSlider* setValue(int value, bool anim = true);
    /**@LEVEL2
     * @brief 修改指示条左值(Mode=2时有效) / set a new value for the left knob of a slider
     * @param value     修改的值/new value
     * @param anim      是否动画显示 1~是 0~否(立即改变)/LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
     */
    STTSlider* setLeftValue(int value, bool anim = true);
    /**
     * @LEVEL0
     * @brief 修改最小最大值 / set minimum and the maximum values of a bar
     * @param min       最小值/minimum value
     * @param max       最大值/maximum value
     */
    STTSlider* setRange(int min, int max);
    /**
     * @LEVEL2
     * @brief 设置可变上下限
     * @param lowVar 下限变量名
     * @param upVar  上限变量名
     */
    STTSlider* setVariableLimit(const string& lowVar = "", const string& upVar = "");
    /**
     * @LEVEL2
     * @brief 修改模式/set the mode of slider.
     * @param mode      0~普通模式
     * 					1~对称模式(最小值设为负，最大值设为正，指示条从0画到当前值,负值向左,正值向右)
     * 					2~范围模式(指示条从左值画到当前值)/the mode of the slider. See ::lv_slider_mode_t
     */
    STTSlider* setMode(int mode = 0);
    /**
     * @LEVEL0
     * @brief 获取当前值/get the value of the main knob of a slider
     */
    int getValue();
    /**
     * @LEVEL2
     * @brief 获取左值/get the value of the left knob of a slider
     */
    int getLeftValue();
    /**
     * @LEVEL0
     * @brief 获取最小值/get the minimum value of a slider
     */
    int getMinValue();
    /**
     * @LEVEL0
     * 获取最大值/get the maximum value of a slider
     */
    int getMaxValue();
    /**
     * @LEVEL2
     * @brief 是否正在被拖动 / Give the slider is being dragged or not
     * @return          是则返回true / true: drag in progress false: not dragged
     */
    bool isDragged();
    /**
     * @LEVEL2
     * @brief 获取slider的模式/get the mode of the slider.
     * @return  0~普通模式 1~对称模式 2~范围模式
     */
    int getMode();
	/**
	 * @HideCA
	 */
    void onDataChanged(SttObject * org, IVariable* data);
public:
	/**@HideCA*/
    void onConfirm(uint16_t code, LvEvent e);
    /**@HideCA*/
    void* mSlider;
    /**@HideCA*/
    void onMinDataChanged(SttObject * org, IVariable* data);
    /**@HideCA*/
    void onMaxDataChanged(SttObject * org, IVariable* data);
};

typedef STTSlider* SttSlider;

}

