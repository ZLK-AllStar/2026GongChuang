/*
 * 圆弧指示条
 *
 *  Created on: 2023年2月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 弧形指示条/进度条
 * @image html image/arc_main.png
 */
class STTArc: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 弧形指示条/进度条构造
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTArc(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 弧形指示条/进度条析构
	 */
    virtual ~STTArc();
    /**
     * @LEVEL2
      * @brief 设置基本参数
      * @param x		X坐标
      * @param y		Y坐标
      * @param w		宽度
      * @param h		高度
      * @param v		当前值
      * @param min 		最小值(默认0，与start位置对应)
      * @param max 		最大值(默认100，与end位置对应)
      * @param start 	弧的起始角度(默认135)(右0下90左180上270)
      * @param end 		弧的终止角度(默认45)(从start开始，顺时针旋转，直到终止角度)
      */
     void setMainArg(int x=0, int y=0, int w=200, int h=200, int v=50, int min=0, int max=100, int start=135, int end=45);
     /**
      * @LEVEL2
      * @brief 设置整个背景及背景弧样式参数
      * @param r 		背景圆角半径(默认9999)
      * @param c1 		背景主色(默认白色)
      * @param c2 		背景渐变次色(默认白色)
      * @param dir 		渐变方向(默认0~不渐变) [1~从上到下 2~从左到右]
      * @param opa 		背景不透明度，范围0~255(默认0)
      * @param w_bord 	背景边框宽度(默认0)
      * @param c_bord 	背影边框颜色(默认黑)
      * @param w_arc 	背景弧的宽度(默认20)
      * @param c_arc 	背景弧的颜色(默认0xdddddd)
      * @param rnd 		背景弧两端是否圆头(默认true)
      * @param img 		背景弧的图片(默认无)
      */
     void setMainStyle(int r=9999, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=0, int w_bord=0, int c_bord=0x0, int w_arc=20, int c_arc=0xdddddd, bool rnd=true, const string& img = "");
     /**
      * @LEVEL2
      * @brief 设置指示弧样式参数
      * @param c1 		指示弧颜色，默认0x1a2b3
      * @param rnd 		指示弧两端是否圆头，默认true
      * @param w 		指示弧宽度
      * @param img 		指示弧的图片(默认无)
      */
     void setIndicStyle(int c1=0x1a2b3, bool rnd=true, int w=20, const string& img = "");
     /**
      * @LEVEL2
      * @brief 设置"拖动按钮"样式参数
      * @param showknob	是否显示拖动圆钮,默认false
      * @param r_mod	"拖动按钮"半径，为相对于指示弧宽度的一半。默认0，即直径=指示弧宽度
      * @param c1		"拖动按钮"主色，默认白色
      * @param c2		"拖动按钮"渐变次色，默认白色
      * @param dir		渐变方向，(默认0~不渐变) [1~从上到下 2~从左到右]
      * @param cbord	"拖动按钮"边框颜色，默认黑色
      * @param wbord	"拖动按钮"边框宽度，默认2
      * @param r		"拖动按钮"圆角半径，默认9999，即圆形
      * @param img
      */
     void setKnobStyle(bool showknob=false, int r_mod=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int cbord=0x0, int wbord=2, int r=9999, const string img = "");
    /**
     * @LEVEL1
     * @brief 设置指示条的起始角度。
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param start 	起始角度，范围0~360，默认135
     */
    STTArc* setStartAngle(int start=135);
    /**
     * @LEVEL1
     * @brief 设置指示条的终止角度。
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param end  		终止角度，范围0~360，默认45
     */
    STTArc* setEndAngle(int end=45);
    /**
     * @LEVEL1
     * @brief 设置指示条的起始、终止角度
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param start 	起始角度，范围0~360，默认135
     * @param end 		终止角度，范围0~360，默认45
     */
    STTArc* setAngles(int start=135, int end=45);
    /**
     * @LEVEL1
     * @brief 设置背景弧起始角度
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param start 	起始角度，范围0~360，默认135
     */
    STTArc* setBgStartAngle(int start=135);
    /**
     * @LEVEL1
     * @brief 设置背景弧截止角度
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param end 		终止角度，范围0~360，默认45
     */
    STTArc* setBgEndAngle(int end=45);
    /**
     * @LEVEL1
     * @brief 设置背景弧起止角度
     * 注：右是0度(x正向)，下是90度(y负向)，顺时针增大
     *
     * @param start 	起始角度，范围0~360，默认135
     * @param end 		终止角度，范围0~360，默认45
     */
    STTArc* setBgAngles(int start=135, int end=45);
    /**
     * @LEVEL1
     * @brief 设置整体旋转角度，顺时针
     * @param rotation 	范围0~360，默认0
     */
    STTArc* setRotation(int rotation=0);
    /**
     * @LEVEL1
     * @brief 设置模式
     * 注：上位机设计界面只支持模式0
     *
     * @param type 		0~常规，正向，1~对称,2~反向(反向模式不兼容点击修改值，见Constant::ARC_Mode)
     */
    STTArc* setMode(int type = 0);
    /**
     * @LEVEL0
     * @brief 设置当前值
     * @param value 	取值范围-32768~32767，默认0
     */
    STTArc* setValue(int value = 0);
    /**
     * @LEVEL0
     * @brief 设置上下限值
     * @param min 		下限,取值范围-32768~32767，默认0
     * @param max 		上限,取值范围-32768~32767，默认100
     */
    STTArc* setRange(int min=0, int max=100);
    /**
     * @LEVEL2
     * @brief 设置可变上下限
     * @param lowVar 下限变量名
     * @param upVar  上限变量名
     */
    STTArc* setVariableLimit(const string& lowVar = "", const string& upVar = "");
    /**
     * @LEVEL0
     * @brief 设置下限值
     * @param min 		下限,取值范围-32768~32767，默认0
     */
    STTArc* setMinValue(int min=0);
    /**
     * @LEVEL0
     * @brief 设置上限值
     * @param max 		上限,取值范围-32768~32767，默认100
     */
    STTArc* setMaxValue(int max=100);
    /**
     * @LEVEL1
     * @brief 设置绘制速率
     * 当值改变时，指示弧从当前值到按下处的
     * @param rate		比如： 90=绘制速率：90°/秒，取值范围0~65535，默认90
     */
    STTArc* setChangeRate(int rate=90);
    /**
     * @LEVEL1
     * @brief 获取旋转起始角度
     */
    int getAngleStart();
    /**
     * @LEVEL1
     * @brief 获取旋转截止角度
     */
    int getAngleEnd();
    /**
     * @LEVEL1
     * @brief 获取背景旋转起始角度
     */
    int getBgAngleStart();
    /**
     * @LEVEL1
     * @brief 获取背景旋转截止角度
     */
    int getBgAngleEnd();
    /**
     * @LEVEL0
     * @brief 获取当前值
     */
    int getValue();
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
     * @brief 获取ARC工作模式
     * 见constant::ARC_Mode
     */
    int getMode();
    /**
     * @LEVEL2
     * @brief 响应变量变化，改变当前弧的指示值
     * @param org	来源控件。如果来源控件为当前控件，则跳过
     * @param data	变量
     */
    void onDataChanged(SttObject * org, IVariable* data);
private:
    /**
     * @HideCA
     * @brief 内部指针
     **/
    void* mArc;
    /**
     * @HideCA
     * @brief 响应变量变化，改变当前弧的最小值
     * @param org	来源控件。如果来源控件为当前控件，则跳过
     * @param data	变量
     */
    void onMinDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 响应变量变化，改变当前弧的最大值
     * @param org	来源控件。如果来源控件为当前控件，则跳过
     * @param data	变量
     */
    void onMaxDataChanged(SttObject * org, IVariable* data);
};

typedef STTArc* SttArc;

}
