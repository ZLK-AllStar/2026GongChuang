/*
 * STTLine.h
 *	画线
 *  Created on: 2023年9月5日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 直线
 */
class STTLine: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建直线控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTLine(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTLine();
public:
	/**
	 * @HideCA
	 * @brief 端点数组
	 */
	LvPoint * m_points;
public:
	/**
	 * @HideCA
	 * @brief 变量变化处理方法(无效)
	 */
    void onDataChanged(SttObject * org, IVariable* data);
public:
    /**
     * @LEVEL0
     * @brief 设置点数据。
     * 绘制时，先设置颜色、宽度、线型，最后调用setPoints
     * @param x 所有点的x坐标数组
     * @param y 所有点的y坐标数组
     * @param point_num	 点的个数
     */
    STTLine* setPoints(int x[],int y[],int point_num);
    /**
     * @LEVEL0
	 * @brief 设置点数据。
	 * 绘制时，先设置颜色、宽度、线型，最后调用setPoints
	 * @param xy x,y的连续坐标。如两个点(0,1)(2,3)，则xy数组为{0,1,2,3}
	 * @param point_num	 点的个数
	 */
    STTLine* setPoints(int xy[], int point_num);
    /**
     * @LEVEL0
     * @brief 设置点数据。
     * 绘制时，先设置颜色、宽度、线型，最后调用setPoints
     * @param xys x,y的连续坐标。如两个点(0,1)(2,3)，则xys="0,1,2,3"
     */
    STTLine* setPoints(string xys);
    /**
     * @LEVEL1
     * @brief 是否翻转Y值。
     * 相当于按控件中心的水平线镜像
     */
    STTLine* setYInvert(bool en);
    /**
     * @LEVEL0
     * @brief 设置线色
     */
    STTLine* setColor(unsigned int c);
    /**
     * @LEVEL0
     * @brief 设置线的不透明度0~255
     */
    STTLine* setOpa(int opa=255);
    /**
     * @LEVEL0
     * @brief 设置线粗
     */
    STTLine* setWidth(int w);
    /**
     * @LEVEL1
     * @brief 设置虚线。
     * 虚线只能是水平或者垂直方向，不可绘制斜虚线
     * @param len_black 实线部分长度
     * @param len_white 空白部分长度
     */
    STTLine* setDash(int len_black=10, int len_white=10);
    /**
     * @LEVEL0
     * @brief 设置两端是否圆头
     */
    STTLine* setRounded(bool en=false);
public:
    /**
     * @LEVEL0
     * @brief 画1条线段
     */
    void drawLine(int x1, int y1, int x2, int y2);
    /**
     * @LEVEL0
     * @brief 画连续折线
     * @param x 所有点的x坐标数组
     * @param y 所有点的y坐标数组
     * @point_num 点的个数
     */
    void drawLine(int x[],int y[], int point_num, bool close);
//    /**
//     * @LEVEL0
//     * @brief 画连续折线
//     * @param xy x,y的连续坐标。如两个点(0,1)(2,3)，则xy数组为{0,1,2,3}
//     * @point_num 点的个数
//     * @param close 是否封闭线段，是则将第1点与最后1点相连
//     */
//    void drawLine(int xy[], int point_num, bool close);
    /**
     * @LEVEL0
     * @brief 画竖线
     */
    void drawVert(int x, int y1, int y2);
    /**
     * @LEVEL0
     * @brief 画横线
     */
    void drawHori(int x1, int x2, int y);
};

typedef STTLine* SttLine;

class STTLineArray {
public:
	STTWidget* owner;
	int width;
	int color;
	uint8_t opa;
	bool round;
	bool yInvert;
	vector<STTLine*> lines;
public:
	/**
	 * @brief 构造
	 * @param par 父控件，直线将画在这个控件上
	 */
	STTLineArray(STTWidget* par);
	~STTLineArray();
	/**
	 * @brief 画一条线，并加入列表
	 * @param xy x,y的连续坐标。如两个点(0,1)(2,3)，则xy数组为{0,1,2,3}
	 * @param cnt 点数
	 * @return 返回刚加入的直线
	 */
	STTLine* add(int* xy, int cnt);
	/**
	 * @brief 画一条水平线，起点为左边，终点为右边，并加入列表
	 * @param y 水平线的y值
	 */
	STTLine* addHori(int y);
	/**
	 * @brief 画一条竖直线，起点为上边，终点为下边，并加入列表
	 * @param x 垂直线的x值
	 */
	STTLine* addVert(int x);
	/**
	 * @brief 获取第i条直线
	 */
	STTLine* get(int i);
	/**
	 * @brief 删除第i条直线
	 */
	void remove(int i);
	/**
	 * @brief 删除全部
	 */
	void removeAll();
};

}
