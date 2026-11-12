/*
 * STTSpinner.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 圆盘
 */
class STTSpinner: public STTWidget {
public:
	/**
	 * @HideCA
     * @brief 转盘控件构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
	 */
	STTSpinner(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTSpinner();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h		位置、尺寸
	 * @param time			转一圈的时间，单位毫秒
	 * @param arc_length	指示条圆弧长度，单位度(1~360)
	 * @param clockwise		是否顺时针
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=100, int time=2000, int arc_length=60, bool clockwise=true);
    /**
     * @LEVEL2
     * @brief 设置主(圆环)样式
     * @param w_arc		背景圆环宽度
     * @param c_arc		背景圆环颜色
     * @param img		背景圆环底图
     */
    void setMainStyle(int w_arc=20, int c_arc = 0xdddddd, const void* img = NULL);
    /**
     * @LEVEL2
     * @brief 设置指示条(圆弧)样式
     * @param w_arc		圆弧宽度
     * @param c_arc		圆弧颜色
     * @param img		圆弧底图
     */
    void setIndicStyle(int w_arc=20, int c_arc=0x1a2b3, const void* img = NULL);
    /**
     * @LEVEL0
     * @brief 	设置旋转速度及方向。
     * 			*如绑定了变量，则根据范围计算time，time大于0为顺时针，小于0为逆时针
     *			*控件显示的转速curS(圈/10秒) = (变量值-minV)/(maxV-minV) * (maxS-minS) + minS
	 *			*控件显示的转一圈耗时(ms) = 10 * 1000 / curS
     * @param angle 	指示条圆弧长度，单位度(1~360)
     * @param time 		转一圈的时间，单位毫秒
     * @param clockwise	是否顺时针
     * @param repeat	重复次数 -1:无限 0:不重复 >0次数
     * @param startangle 旋转起始角度。在停止后，再次启动时设置这个值，避免又从0开始。
    */
    void setAnim(int angle=60, int time=2000, bool clockwise=true, int repeat=-1, int startangle = 0);
    /**
     * @brief 停止动画，如需重新开始，调用setAnim
     */
    void stopAnim();
	/**
	 * @LEVEL2
	 * @brief 获取动画指针
	 */
    void* GetAnim();
    /**
     * @brief 获取当前角度值(右0，下90，顺时针为正)
     */
    int getCurrentAngle();
    /**
     * @LEVEL0
     * @brief 修改转一圈的时间，单位毫秒。
     */
    void setValue(int time);
    /**
     * @LEVEL0
     * @brief	设置变量的值范围、及对应的转速范围
     * @param minv	变量的最小值
     * @param maxv	变量的最大值
     * @param mins	minv对应的最小速度(圈/10秒)
     * @param maxs	maxv对应的最大速度(圈/10秒)
     */
    void setRanges(int minv,int maxv,int mins,int maxs);
    /**
     * @HideCA
     */
    void onDataChanged(SttObject * org, IVariable* data);
public:
    /**
     * @HideCA
     */
    void* mSpinner;
};

typedef STTSpinner* SttSpinner;

}
