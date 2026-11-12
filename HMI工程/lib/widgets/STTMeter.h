/*
 * STTMeter.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 仪表盘、指针、刻度盘
 */
class STTMeter: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建仪表盘控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTMeter(IRuntime* rt, STTWidget* par, const string& id = "");
    /**
     * @HideCA
     * @brief 析构
     */
	virtual ~STTMeter();
    /**
     * @LEVEL2
     * @brief 增加一个刻度尺，仅增加刻度尺没有图像，需要设置其刻度线、主刻度线、
     * 		  或增加指针、或增加Arc
     * @note scale中有刻度线的属性、文字属性等
     */
    LvMeterScale* addScale();
    /**
     * @LEVEL2
     * @brief 设置刻度尺的刻度线样式
     * @param scale     指定刻度尺
     * @param cnt       刻度线的数量, 大于2
     * @param width     刻度线的粗细
     * @param len       刻度线的长短
     * @param color     刻度线的颜色
     */
    STTMeter* setScaleTicks(LvMeterScale* scale, int cnt = 21, int width = 5, int len = 12, int color = 0xA0A0A0);
    /**
     * @LEVEL2
     * @brief 将指定的刻度线设为主刻度线，并设置样式。标签将显示在相应的主刻度下
     * @param scale         指定刻度尺
     * @param nth           每nth个刻度的第1个设为主刻度线
     * @param width         主刻度线粗细
     * @param len           主刻度线长短
     * @param color         主刻度线的颜色
     * @param label_gap     主刻度线与标签之间的间隙
     */
    STTMeter* setScaleMajorTicks(LvMeterScale* scale, int nth = 4, int width = 7, int len = 18
        , int color = 0xA0A0A0, int label_gap = 15);
    /**
     * @LEVEL2
     * @brief 设置量程及角度范围
     * @param scale         指定刻度尺
     * @param min           最小量程值
     * @param max           最大量程值
     * @param angle_range   从min到max之间的角度(顺时针)，范围0~360
     * @param rotation      整体旋转角度。一般设为-angle_range/2-90+360
     * 						(先逆时针旋转angle_range/2度，再逆时针旋转90度，再取正值。即min在左侧 max在右侧)
     */
    STTMeter* setScaleRange(LvMeterScale* scale, int min=0, int max=100, int angle_range=270, int rotation=135);
    /**
     * @LEVEL2
     * @brief 设置可变量程
     * @param lowVar 下限变量名
     * @param upVar  上限变量名
     */
    STTMeter* setVariableLimit(const string& lowVar = "", const string& upVar = "");
    /**
     * @LEVEL0
     * @brief 设置下限值
     * @param min 		下限,取值范围-32768~32767，默认0
     */
    STTMeter* setRange(int min,int max);
    /**
     * @LEVEL2
     * @brief 设置可变报警值
     * @param lowVar 报警变量名
     */
    STTMeter * setVariableAlarm(const string& alarmVar);
    /**
     * @LEVEL2
     * @brief 增加一个指针指示器
     * @param scale         指定刻度尺
     * @param width         指针指示器粗细
     * @param color         指针指示器颜色
     * @param r_mod         指针指示器半径(这是一个相对于scale半径的值。如scale的半径为10, r_mod设为-1，则指针的长度为9)
     */
    LvMeterIndicator* addNeedleLine(LvMeterScale* scale, int width = 7, int color = 0x0, int r_mod = 0);
    /**
     * @LEVEL2
     * @brief 增加一个图片指针指示器
     * 图片是一个指向右侧的指针，如-O----->。轴心应设在O处中心。
     * @param scale         指定刻度尺。
     * @param src           图片路径。
     * @param pivot_x       轴心x坐标，默认0。
     * @param pivot_y       轴心y坐标，默认0。
     */
    LvMeterIndicator* addNeedleImg(LvMeterScale* scale, const void* src, int pivot_x=0, int pivot_y=0);
    /**
     * @LEVEL2
     * @brief 增加一个圆弧指示器/add an arc indicator the scale
     * @param scale         指定刻度尺/pointer to scale (added to `meter`)
     * @param width         圆弧的粗细/width of the arc
     * @param color         圆弧的颜色/color of the arc
     * @param r_mod         圆弧外径(这是一个相对于scale半径的值。如scale的半径为10, r_mod设为-1，则指针的长度为9)/the radius modifier (added to the scale's radius) to get the outer radius of the arc
     */
    LvMeterIndicator* addArc(LvMeterScale* scale, int width=10, int color=0xff00, int r_mod=0);
    /**
     * @LEVEL2
     * @brief 增加刻度线指示器，这会修改原刻度线/add a scale line indicator the scale. It will modify the ticks.
     * 		  即可在原来的刻度线中，更改其中某一段属性(累似style效果，多个叠加)
     * @param scale         指定刻度尺/pointer to scale (added to `meter`)
     * @param color_start   渐变起始色/the start color
     * @param color_end     渐变终止色/the end color
     * @param local         渐变色计算方式. true是本指示器内渐变; false是整个个scale内渐变/tell how to map start and end color. true: the indicator's start and end_value; false: the scale's min max value
     * @param width_mod     刻度线的粗细修正值，如-1，则粗细=原刻度线粗细-1 / add this the affected tick's width
     */
    LvMeterIndicator* addScaleLines(LvMeterScale* scale, int color_start, int color_end, bool local, int width_mod);
    /**
     * @LEVEL1
     * @brief 设置指示器的起始、终止值
     * 注意：用于指针指示器(不适用于arc\tick)，会同时将起始值和终止值设为同一值/set the value of the indicator. It will set start and and value to the same value
     * @param indic         指定指示器，一般用于指针指示器/pointer to an indicator
     * @param value         值/the new value
     */
    STTMeter* setIndicatorValue(LvMeterIndicator* indic, int value);
    /**
     * @LEVEL1
     * @brief 设置指示器的起始值/set the start value of the indicator.
     * @param indic         指定指示器，一般用于圆弧、刻度线指示器/pointer to an indicator
     * @param value         值/the new value
     */
    STTMeter* setIndicatorStartValue(LvMeterIndicator* indic, int value);
    /**
     * @LEVEL1
     * @brief 设置指示器的终止值/set the end value of the indicator.
     * @param indic         指定指示器，一般用于圆弧、刻度线指示器/pointer to an indicator
     * @param value         值/the new value
     */
    STTMeter* setIndicatorEndValue(LvMeterIndicator* indic, int value);
    /**
     * @LEVEL2
     * @brief 设置Gauge(仪表盘)基本参数
     * @param x,y,w,h		位置及尺寸
     * @param v_crati		量程报警值
     * @param v_min			最小量程值
     * @param v_max			最大量程值
     * @param a_range		量程范围的角度
     * @param show_first	是否显示第一个标签文本
     * @param show_last		是否显示最后一个标签文本
     * @param labelScale	标签值的放大倍数。如该标签所指示的值为100，则显示在标签上的文本为100/labelScale
     * @param a_rotation	整体旋转角度。一般设为-angle_range/2-90+360
     * 						(先逆时针旋转angle_range/2度，再逆时针旋转90度，再取正值。即min在左侧 max在右侧)
     * @param w_circle		中心圆直径
     */
    void setGaugeMainArg(int x=0, int y=0, int w=200, int h=200, int v_crati=80, int v_min=0, int v_max=100, int a_range=270, bool show_first=true
        , bool show_last=true, int labelScale=1, int a_rotation=135, int w_circle=16);
    /**
     * @LEVEL2
     * @brief 设置Gauge(仪表盘)基本样式
     * @param r 		背景圆角
     * @param c1 		背景主色
     * @param c2		背景渐变终止色
     * @param dir		背景渐变方向
     * @param opa		背景不透明度
     * @param w_border	边框线粗细
     * @param c_border 	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
     */
    void setGaugeMainStyle(int r=9999, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=255, int w_border=2, int c_border=0x1a2b3, int w_shadow=0, int c_shadow=0x0);
    /**
     * @LEVEL2
     * @brief 设置Gauge(仪表盘)指针样式
     * @param c				指针颜色
     * @param v				指向的值
     * @param w				指针的粗线
     * @param img			指针图片(NULL时为纯色)
     * @param recolor		替换色(对img有效, 图片中的颜色将变为参数recolor，为0时不替换)
     * @param pivotx		轴心x(对img有效，默认在左上角为0,0)
     * @param pivoty		轴心y(对img有效，默认在左上角为0,0)
     * @param r_mod			指针长度修正值
     */
    void setGaugeNeedleStyle(int c=0xff0000, int v=50, int w=8, const void* img=NULL, int recolor=0, int pivotx=0, int pivoty=0, int r_mod=-10);
    /**
     * @LEVEL2
     * @brief 设置Gauge(仪表盘)标签字体
     * @param c 字体颜色
     * @param size 字体大小
     * @param fontname 字体名称。为""时，取默认字体
     */
    void setGaugeFont(int c=0x0, int size=12, const char* fontname="", int fontstyle=0);
    /**
     * @LEVEL2
     * @brief 设置Gauge(仪表盘)刻度线样式
     * @param c1 指示条颜色(在criticalValue~MaxValue之间的刻度颜色)
     * @param c2 刻度的起始颜色(在MinValue~criticalValue之间的刻度颜色)
     * @param c3 刻度的终止颜色(在MinValue~criticalValue之间的刻度颜色)
     * @param w1 刻度线的宽度(在MinValue~criticalValue之间的刻度线宽度)
     * @param w2 指示条的粗细(在criticalValue~MaxValue之间的刻度线宽度)
     * @param len 刻度线的长度
     * @param cnt_tick 刻度线总数
     */
    void setGaugeTickStyle(int c1=0xff0000, int c2=0x0, int c3=0x0, int w1=4, int w2=4, int len=7, int cnt_tick=21);
    /**
     * @LEVEL2
     * 设置Gauge(仪表盘)长刻度线样式
     * @param w1		长刻度线粗细
     * @param len		长刻度线长短
     * @param cnt_label	标签个数
     * @param gap_label	长刻度到标签之间的间隙
     */
    void setGaugeMajorTickStyle( int w1=6, int len=10, int cnt_label=6, int gap_label = 15);
    /**
     * @LEVEL2
     * 设置Gauge(仪表盘)中心圆样式
     * @param r				中心圆的圆角半径(等0时是正方形)
     * @param c1			中心圆的主色
     * @param c2			中心圆的次色
     * @param dir			中心圆的渐变方向
     * @param w_border		中心圆的边框线粗
     * @param c_border		中心圆的边框线色
     * @param w_shadow		中心圆的阴影宽
     * @param c_shadow		中心圆的阴影色
     */
    void setGaugeCircleStyle(int r=9999, int c1=0x0, int c2=0x0, int dir=0, int w_border=0, int c_border=0x0, int w_shadow=0, int c_shadow=0x0);
    /**
     * @LEVEL2
     * @brief 设置指针控件(Needle)的基本参数
     * @param x,y,w,h 		位置及尺寸
     * @param v_min			量程最小值
     * @param v_max			量程最大值
     * @param a_range		量程范围角度
     * @param a_rotation	整体旋转角度。一般设为-angle_range/2-90+360
     * 						(先逆时针旋转angle_range/2度，再逆时针旋转90度，再取正值。即min在左侧 max在右侧)
     * @param w_circle		中心圆直径
     */
    void setNeedleMainArg(int x=0, int y=0, int w=200, int h=200, int v_min=0, int v_max=100, int a_range=270, int a_rotation=135, int w_circle=15);
    /**
     * @LEVEL2
     * @brief 设置指针控件(Needle)的指针样式
     * @param c				指针颜色
     * @param v				指向的值
     * @param w				指针的粗线
     * @param img			指针图片(NULL时为纯色)
     * @param recolor		替换色(对img有效, 图片中的颜色将变为参数recolor，为0时不替换)
     * @param pivotx		轴心x(对img有效，默认在左上角为0,0)
     * @param pivoty		轴心y(对img有效，默认在左上角为0,0)
     * @param r_mod			指针长度修正值
     */
    void setNeedleNeedleStyle(int c=0xff0000, int v=50, int w=8, const void* img=NULL, int recolor=0, int pivotx=0, int pivoty=0, int r_mod=-10);
    /**
     * @LEVEL2
     * @brief 设置指针控件(Needle)的中心圆样式
     * @param r				中心圆的圆角半径(等0时是正方形)
     * @param c1			中心圆的主色
     * @param c2			中心圆的次色
     * @param dir			中心圆的渐变方向
     * @param w_border		中心圆的边框线粗
     * @param c_border		中心圆的边框线色
     * @param w_shadow		中心圆的阴影宽
     * @param c_shadow		中心圆的阴影色
     */
    void setNeedleCircleStyle(int r=9999, int c1=0x0, int c2=0x0, int dir=0, int w_border=0, int c_border=0x0);
    /**
     * @LEVEL2
     * @brief 设置仪表控件(LineMeter)的基本参数
     * @param x,y,w,h		位置及尺寸
     * @param v_crati		量程报警值
     * @param v_min			最小量程值
     * @param v_max			最大量程值
     * @param a_range		量程范围的角度
     * @param a_rotation	整体旋转角度。一般设为-angle_range/2-90+360
     * 						(先逆时针旋转angle_range/2度，再逆时针旋转90度，再取正值。即min在左侧 max在右侧)
     */
    void setLineMeterMainArg(int x=0, int y=0, int w=200, int h=200, int v_min=0, int v_max=100, int a_range=270, int a_rotation=135);
    /**
     * @LEVEL2
	 * @brief 设置仪表控件(LineMeter)的基本样式
     * @param r 		背景圆角
     * @param c1 		背景主色
     * @param c2		背景渐变终止色
     * @param dir		背景渐变方向
     * @param opa		背景不透明度
     * @param w_border	边框线粗细
     * @param c_border 	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
	 */
    void setLineMeterMainStyle(int r=9999, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=2, int c_border=0x1a2b3, int w_shadow=0, int c_shadow=0x0);
    /**
     * @LEVEL2
     * @brief 设置仪表控件(LineMeter)的刻度线样式
     * @param c1 0~value中0处的颜色
     * @param c2 0~value中value处的颜色
     * @param c3 value之后的颜色
     * @param w1 0~value中刻度线的宽度
     * @param w2 value之后的刻度线的宽度
     * @param len 刻度线的长度
     * @param cnt_tick 刻度线的数量
     * @param v value值
     */
    void setLineMeterTickStyle(int c1, int c2, int c3, int w1, int w2, int len, int cnt_tick, int v);
    /**
     * @LEVEL0
     * @brief 设置值
     * 注意：如为范围型指示器(圆弧、刻度)，则设置终止值
     */
    void setValue(int val);
    int getValue();
    /**@HideCA
     * @brief 变量触发方法
     */
    void onDataChanged(SttObject * org, IVariable* data);
	/**@HideCA
	 * @brief 内部指针
	 */
    void* mMeter;
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
    /**
     * @HideCA
     * @brief 响应变量变化，改变控件报警值
     * @param org		来源控件。如果来源控件为当前控件，则跳过
     * @param data		变量
     */
    void onAlamrValueChanged(SttObject * org, IVariable* data);
    /**
     * @LEVEL0
     * @brief 设置报警值
     */
    STTMeter *  setAlarmValue(int val);
public:

    /**
     * @LEVEL0
     * @brief 设置动画持续时长(ms)
     */
    void setAnimTimeMs(int ms);

public:
    /**
     * @brief 获取指示针
     */
    LvMeterIndicator* getNeedle();
    /**
     * @brief 获取内置的标尺
     */
    LvMeterScale * getScale();
    /**
     * @brief 获取标尺起始角
     */
    int getStartAngle();
    /**
     * @brief 获取标尺终止角
     */
    int getEndAngle();
    /**
     * @brief 获取当前值对应的角度
     */
    int getValueAngle();
};

typedef STTMeter* SttMeter;

}
