/*
 * STTChart.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "Widgets/STTTable.h"
#include "db/IDataBase.h"
#include <vector>
#include <string>
#include <regex>

using namespace std;
using namespace stt::db;

namespace stt{

/**
 * @ingroup widget
 * @brief 自由曲线图
 * @image html image/chart_main.png
 * 无数据源，通过setNextValue添加数据
 */
class STTChart: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 曲线图控件构造
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTChart(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 曲线图控件析构
	 */
	virtual ~STTChart();
    /**
     * @LEVEL2
     * @brief 1 设置基本参数：位置、大小、类型、点数、分隔数、字体。
     * @param x x坐标
     * @param y y坐标
     * @param w 宽
     * @param h 高
     * @param charttype 图表类型 0~不绘制 1~折线图 2~柱状图 3~散点图
     * @param pc_cnt 数据个数
     * @param hdiv 横向分隔线数量(不含两侧的边线。如hdiv=3，则加上两侧的边线，共有5条)
     * @param vdiv 竖向分隔线数量(不含两侧的边线。如vdiv=3，则加上两侧的边线，共有5条)
     * @param fontsize 字体大小
     * @param fontname 字体名称, ""为默认字体
     * @param fontcolor 字体颜色
    */
    void setMainArg(int x=0, int y=0, int w=300, int h=200, int charttype=1, int pt_cnt=10, int hdiv=3, int vdiv=5
        , int fontsize = 12, const string& fontname = "", int fontcolor = 0x0, int fontstyle=0);
    /**
     * @LEVEL2
     * @brief 2 设置主(下)X轴参数：刻度线的长度、数量、标签
     * @param major_len 主刻度线的长度
     * @param minor_len 次刻度线的长度
     * @param major_cnt 主刻度线的数量
     * @param minor_cnt 次刻度线的数量（是指主两相邻主刻度线之间的分段数，即实际数量+1）
     * @param showlabel 是否显示标签
     * @param labels 用\n分隔(与lvgl7一致)，个数与major_cnt一致，lvgl8要通过事件实现标签的修改
    */
    void setXAxis(int major_len, int minor_len, int major_cnt, int minor_cnt, bool showlabel, const string& labels="");
    /**
     * @LEVEL2
     * @brief 3 设置次(上)X轴参数：刻度线的长度、数量、标签
     * @param major_len 主刻度线的长度
     * @param minor_len 次刻度线的长度
     * @param major_cnt 主刻度线的数量
     * @param minor_cnt 次刻度线的数量（是指主两相邻主刻度线之间的分段数，即实际数量+1）
     * @param showlabel 是否显示标签
     * @param labels 用\n分隔(与lvgl7一致)，个数与major_cnt一致，lvgl8要通过事件实现标签的修改
     */
    void setX2Axis(int major_len, int minor_len, int major_cnt, int minor_cnt, bool showlabel, const string& labels="");
    /**
     * @LEVEL2
     * @brief 4 设置主(左)Y轴参数
     * @param major_len 主刻度线的长度
     * @param minor_len 次刻度线的长度
     * @param major_cnt 主刻度线的数量
     * @param minor_cnt 次刻度线的数量（是指主两相邻主刻度线之间的分段数，即实际数量+1）
     * @param showlabel 是否显示标签
     * @param labels 用\n分隔(与lvgl7一致)，个数与major_cnt一致，lvgl8要通过事件实现标签的修改
     * @param min 最小值
     * @param max 最大值
     */
    void setYAxis(int major_len, int minor_len, int major_cnt, int minor_cnt, bool showlabel, const string& labels="", float min=0, float max=100);
    /**
     * @LEVEL2
     * @brief 5 设置次(右)Y轴参数
	 * @param major_len 主刻度线的长度
     * @param minor_len 次刻度线的长度
     * @param major_cnt 主刻度线的数量
     * @param minor_cnt 次刻度线的数量（是指主两相邻主刻度线之间的分段数，即实际数量+1）
     * @param showlabel 是否显示标签
     * @param labels 用\n分隔(与lvgl7一致)，个数与major_cnt一致，lvgl8要通过事件实现标签的修改
     * @param min 最小值
     * @param max 最大值
     */
    void setY2Axis(int major_len, int minor_len, int major_cnt, int minor_cnt, bool showlabel, const string& labels="", float min=0, float max=100);
    /**
     * @LEVEL2
     * @brief 6 基本样式设置：主背景、边框、阴影、图表到边框的边距
     * @param r		:	圆角半径
     * @param c1	:	背景主色(若渐变，为起始色)
     * @param c2	:	背景次色(渐变时有效，为结束色)
     * @param dir	:	渐变方向(0~不渐变 1~上下 2~左右)
     * @param w_border	:	边框线宽
     * @param c_border	:	边框线色
     * @param w_shadow	:	阴影宽
     * @param c_shadow	:	阴影色
     * @param padx	:	左边距(注意：不是到x轴的距离)
     * @param padx2	:	右边距
     * @param pady	:	上边距
     * @param pady2	:	下边距
    */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=0, int c_border=0x0, int w_shadow=0, int c_shadow=0x0
        , int padx=30, int padx2=30, int pady=30, int pady2=30);
    /**
     * @LEVEL2
     * @brief 7 设置图表区样式。背景、边框、折线图的线宽及点的大小、分隔虚线（线宽、颜色、实长和虚长）
     * @param r:	圆角半径
     * @param c1:	主色
     * @param c2:	渐变终止色(dir==0时无效)
     * @param dir:	渐变方向(0:不渐变 1:上->下 2:左->右)
     * @param w_border:	边框宽度
     * @param c_border: 边框颜色
     * @param w_line:	折线图线宽
     * @param pt_size: 	折线图点的大小
     * @param w_div:	分隔线的宽
     * @param c_div:	分隔线颜色
     * @param w_dash:	分隔线虚线的画线部分长度
     * @param gap_dash:	分隔线虚线的空白部分长度
    */
    void setChartBgStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=2, int c_border=0x0, int w_line=2, int pt_size=2
        , int w_div=1, int c_div=0x0, int w_dash=10, int gap_dash=0);
    /**
     * @LEVEL2
     * @brief 8 设置刻度线样式。宽度和颜色, 所有轴统一设置，如需单独设置，要在begin_draw事件里处理
     * @param w : 刻度线宽。
     * @param c	: 刻度线颜色。
     */
    void setTickStyle(int w,int c);
    /**
     * @LEVEL2
     * @brief 9 增加Series，并设置初值
     * @param yaxis : 	对应的y轴。0~左y轴, 1~右y轴
     * @param c		:	折线或柱的颜色。
     * @praam pts	:	初始数据值，vector中数量不足时，不足部分为0
     * @param minv		系列数据范围的最小值
     * @param maxv		系列数据范围的最大值
     */
    LvChartSeries* setAddSeries(int yaxis, int c, vector<float>* pts, float minv=0, float maxv=100);
    /**
     * @LEVEL0
     * @brief 设置尺寸
     * 将容器设为指定大小，与chart之间的间隙为32 (为了与lvgl7一致)
     */
    STTChart* setSize(int w, int h);
    /**
     * @LEVEL0
     * @brief 居中chart
     */
    STTChart* center();
    /**
     * @LEVEL0
     * @brief 位置
     */
    STTChart* setPos(int x, int y);
    /**
     * @LEVEL0
     * @brief 设置高度(调用SetSize)
    */
    STTChart* setHeight(int h);
    /**
     * @LEVEL0
     * @brief 设置宽度(调用SetSize)
    */
    STTChart* setWidth(int w);
    /**
     * @LEVEL0
     * @brief 设置类型
     * @param type  0~不绘制 1~折线图 2~柱状图 3~散点图
     */
    STTChart* setType(int type = 1);
    /**
     * @LEVEL0
     * @brief 设置数据个数
     * @param cnt  数据个数
     */
    STTChart* setPointCount(int cnt = 10);
    /**
     * @LEVEL0
     * @brief 设置y轴值范围
     * @param axis   0~主Y轴(左) 1~次Y轴(右) 2~主X轴(下) 4~次X轴(上)
     * @param min    最小值
     * @param max    最大值
     */
    STTChart* setRange(int axis = 0, int min = 0, int max = 100);
    /**
     * @brief 图表更新模式
     * @param mode 0~左移模式(增加第一个数据a时，pt[0]=pt[1],pt[1]=pt[2]...pt[n]=a)
     * 			   1~循环模式(增加第一个数据a时，pt[0]=a，第2次增加b时，pt[1]=b，如此循环)
     */
    STTChart* setUpdateMode(int update_mode = 0);
    /**
     * @LEVEL1
     * @brief 横向、竖向分隔虚线
     * @param hdiv  横向分隔线数量，注意这个值是包含两侧边线的。[0..n]，若取1，与0相同，没有分隔线
     * @param vdiv  竖向分隔线数量，注意这个值是包含两侧边线的。[0..n]，若取1，与0相同，没有分隔线
     */
    STTChart* setDivLineCount(uint16_t hdiv=3, uint16_t vdiv=5);
    /**
     * @LEVEL1
     * @brief 取横向分隔线数量值，注意这个值是包含两侧边线的
     */
    int getDivLineCountH();
    /**
     * @LEVEL1
     * @brief 取竖向向分隔线数量值，注意这个值是包含两侧边线的
     */
    int getDivLineCountV();
    /**
     * @LEVEL1
     * @brief 缩放x轴
     * @param zoom_x    取值范围1/256~256
     */
    STTChart* setZoomX(float zoom_x=1);
    /**
     * @LEVEL1
     * @brief 缩放Y轴
     * @param zoom_y    取值范围1/256~256
     */
    STTChart* setZoomY(float zoom_y=1);
    /**
     * @LEVEL1
     * @brief 获取x轴缩放比例
     */
    float getZoomX();
    /**
     * @LEVEL1
     * @brief 获取y轴缩放比例
     */
    float getZoomY();
    /**
     * @LEVEL1
     * @brief 设置轴上的刻度与标签
     * @param axis          指定轴. 0~左Y轴 1~右Y轴 2~下X轴 3~上X轴
     * @param major_len     主刻度线的长度
     * @param minor_len     次刻度线的长度
     * @param major_cnt     主刻度线的数量
     * @param minor_cnt     两个主刻度线之间的次刻度线数量
     * @param label_en      是否绘制主刻度标签
     * @param draw_size     绘制刻度和标签需要额外的区域大小，一般需要20像素，根据字体和线长增加
     */
    STTChart* setAxisTick(int axis=2, int major_len=4, int minor_len=2, int major_cnt=10, int minor_cnt=4, bool label_en=true, int draw_size=20);
    /**
     * @LEVEL0
     * @brief 获取chart类型 0~不绘制 1~折线图 2~柱状图 3~散点图
     */
    int getType();
    /**
     * @LEVEL0
     * @brief 获取chart数据量
     */
    int getPointCount();
    /**
     * @LEVEL1
     * @brief 获取series的起始点索引
     * @param ser       指定series
     */
    int getXStartPoint(LvChartSeries* ser);
    /**
     * @LEVEL1
     * @brief 获取某点坐标
     * @param ser       指定series
     * @param id        指定点
     * @param p_out     输出坐标
     */
    STTChart* getPointPosById(LvChartSeries* ser, uint16_t id, LvPoint* p_out);
    /**
     * @LEVEL0
     * @brief 刷新chart(数据修改后)
     */
    void refresh();
     /**
      * @LEVEL1
      * @brief 分配并增加一个series(请使用setAddSeries)
      * 由于lvchart中往链表头部增加，故与getNextSeries方向不致。若对顺序有要求，可倒序增加
      * @param color     颜色
      * @param axis      指定主Y轴还是次Y轴 0~主Y轴，1~次Y轴
      */
    LvChartSeries* addSeries(int color, int axis = 0);
    /**
     * @LEVEL1
     * @brief 移除series
     * @param series    指定series
     */
    STTChart* removeSeries(LvChartSeries* series);
    /**
     * @LEVEL1
     * @brief 隐藏series
     * @param series    指定series
     * @param hide      true: 隐藏  false:显示
     */
    LvChartSeries* hideSeries(LvChartSeries* series, bool hide=true);
    /**
     * @LEVEL1
     * @brief 修改series颜色
     * @param series    指定series
     * @param color     颜色
     */
    STTChart* setSeriesColor(LvChartSeries* series, int color = 0x0);
    /**
     * @LEVEL1
     * @brief 将数据的第id个点显示为第1个点，后续依次显示
     * @param ser       指定series
     * @param id        第id个数据
     */
    STTChart* setXStartPoint(LvChartSeries* ser, int id);
    /**
     * @LEVEL1
     * @brief 获下一个series.
     * @param ser     当前series.取第1个，则传入NULL。
     * @return 		  ser指向最后一个时，返回NULL
     */
    LvChartSeries* getSeriesNext(const LvChartSeries* ser = NULL);
    /**
     * @LEVEL1
     * @brief 添加光标(不能直接调用，请使用setCursorStyle)
	 *  光标是啥样的？见https://blog.csdn.net/mygod2008ok/article/details/125118596
     * @param color     光标颜色
     * @param dir       光标方向. 0~不显示 1~左 2~右  4~上  8~下 及四种方向的或操作
     */
    LvChartCursor* addCursor(int color, int dir=15);
    /**
     * @LEVEL1
     * @brief 将光标显示指定坐标上
     * @param cursor    光标
     * @param pos       指定坐标
     */
    STTChart* setCursorPos(LvChartCursor* cursor, LvPoint* pos);

    /**
     * @LEVEL1
     * @brief 将光标显示在指定点上
     * @param cursor    光标
     * @param ser       指定series
     * @param point_id  指定点。设为INT32_MAX，则隐藏光标
     */
    STTChart* setCursorPoint(LvChartCursor* cursor, LvChartSeries* ser, int point_id);
    /**
     * @LEVEL1
     * @brief 获取光标所在的点(暂定)
     * @param cursor    光标
     */
    STTChart* getCursorPoint(LvChartCursor* cursor, LvPoint * pt);
     /**
      * @LEVEL1
      * @brief 将指定series的所有点设为指定值
      * @param ser       指定series
      * @param value     值. 设为FLOAT_MAX(0x7f7fffff)时，将不绘制该点
      */
    STTChart* setAllValue(LvChartSeries* ser, float value);
    /**
     * @LEVEL1
     * @brief 设置下一个数据(看上去整个曲线从右往左移动)
     * @param ser       指定series
     * @param value     y值
     */
    STTChart* setNextValue(LvChartSeries* ser, float value);
    /**
     * @LEVEL1
     * @brief 设置上一个数据(看上去整个曲线从左往右移动)
     */
    STTChart* setPrevValue(LvChartSeries* ser, float value);
    /**
     * @LEVEL1
     * @brief 设置散点图下一个数据(只对散点图有效、看上去整个曲线从右往左移动)
     * @param ser       指定series
     * @param x_value   x值
     * @param y_value   y值
     */
    STTChart* setNextValue2(LvChartSeries* ser, float x_value, float y_value);
    /**
     * @LEVEL1
     * @brief 设置散点图上一个数据(只对散点图有效、看上去整个曲线从左往右移动)
     * @param ser       指定series
     * @param x_value   x值
     * @param y_value   y值
     */
    STTChart* setPrevValue2(LvChartSeries* ser, float x_value, float y_value);
    /**
     * @LEVEL1
     * @brief 修改数据
     * @param ser     指定series
     * @param id      被修改的点的索引
     * @param value   y值
     */
    STTChart* setValueById(LvChartSeries* ser, int id, int value);
    /**
     * @LEVEL1
     * @brief 修改散点图的数据(只对散点图有效)
     * @param ser       指定series
     * @param id        被修改的点的索引
     * @param x_value   x值
     * @param y_value   y值
     */
    STTChart* setValueById2(LvChartSeries* ser, int id, int x_value, int y_value);
    /**
     * @LEVEL2
     * @brief 将Y值数组指向一个外部数组(注意：不会将外部数组值赋给chart)。数组个数要与chart的点数一致。
     * @param ser       指定series
     * @param array     指定外部数组
     */
    STTChart* setExtYArray(LvChartSeries* ser, int array[]);
    /**
     * @LEVEL2
     * @brief 将X值数组指向一个外部数组(注意：不会将外部数组值赋给chart)。数组个数要与chart的点数一致。
     * @param ser       指定series
     * @param array     指定外部数组
     * @note x数组仅对散点图有用
     */
    STTChart* setExtXArray(LvChartSeries* ser, int array[]);
    /**
     * @LEVEL1
     * @brief 获取指定series的y值数组
     * @param ser   指定series
     * @return      数组，长度为chart的点数
     */
    int* getYArray(LvChartSeries* ser);
    /**
     * @LEVEL1
     * @brief 获取指定series的x值数组
     * @param ser   指定series
     * @return      数组，长度为chart的点数
     * @note x数组仅对散点图有用
     */
    int* getXArray(LvChartSeries* ser);
    /**
     * @LEVEL1
     * @brief 获取当前点击的数据点
     * @return  数据点的索引值
     */
    int getPressedPoint();
	/**
	 * @LEVEL1
	 * @brief 启用或禁用折线下区域渐变色
	 * @param enable true启用 false禁用
	 */
	void setFadeArea(bool enable);
	/**
	 * @LEVEL1
	 * @brief 启用或禁用折线下区域渐变色
	 * @return true启用 false禁用
	 */
	bool getFadeArea();
    /**
     * @HideCA
     * @brief 内部指针
     */
	void* mChart;
	/**
	 * @HideCA
	 * @brief 隐藏/显示控件
	 */
	STTWidget* setHidden(bool en);
    /**
     * @HideCA
     * @brief 设置series的数据范围
     */
    void setSeriesMinMax(LvChartSeries* ser, float min, float max);
    /**
     * @HideCA
     * @brief 设置所有series的数据范围
     */
    void setAllSeriesMinMax(float min, float max);
    /**
     * @HideCA
     * @brief 取series的数据范围的最小值
     */
    float getSeriesMin(LvChartSeries* ser);
    /**
     * @HideCA
     * @brief 取series的数据范围的最大值
     */
    float getSeriesMax(LvChartSeries* ser);
    /**
     * @LEVEL1
     * @brief 获取Series数
     */
    int getSeriesCount();
    /**
	 * @LEVEL1
	 * @brief 通过索引值取系列指针，以add顺序为准
	 */
	LvChartSeries * getSeries(int idx);
	/**
	 * @LEVEL2
	 * @brief 用于消除第一条竖线。
	 * 当数据量大于控件宽度时，可能会在第1个点上出现一条竖线。
	 * 可调用本方法消除。(将第1个点的值修改为第2个点的值)
	 */
	void fixFirstSeriesLine(LvChartSeries* ser);
	/**
	 * @HideCA
	 * @brief 计算系列的显示值：根据对应的y轴minmax与系列的minmax进行计算
	 */
	float calShowValue(LvChartSeries* ser, float src_v);
	/**
	 * @HideCA
	 * @brief 计算系列的实际值：根据对应的y轴minmax与系列的minmax进行计算
	 */
	float calRealValue(LvChartSeries* ser, float show_v);
	/**
	 * @LEVEL0
	 * @brief 启用或关闭光标
	 */
	void enableCusror(bool b);
	/**
	 * @LEVEL0
	 * @brief 获取光标是否启用
	 */
	bool isEnableCursor();
	/**
	 * @LEVEL0
	 * @brief 指定外部表格控件，显示光标数据
	 * @param stttable 表格控件。会自动解绑旧数据，并隐藏。在点击曲线后显示
	 * @note 对实时曲线、历史曲线有效
	 */
	void setCursorTable(STTTable* stttable);
	/**
	 * @LEVEL0
	 * @brief 获取光标数据显示表的指针
	 * @note 对实时曲线、历史曲线有效
	 */
	STTTable* getCursorTable();
	/**
	 * @LEVEL0
	 * @brief 设置光标的颜色、粗细、点大小、第1个单元格内容
	 * @param c 颜色带透明度，如0xffff0000
	 */
	void setCursorStyle(int c=0, int w=1, int s=1, string str="");
	/**
	 * 启用控件拖拽
	 */
	virtual void enableDrag();
	/**
	 * @brief 获取刻度线的参数
	 * @param axis          指定轴. 0~左Y轴 2~右Y轴 1~下X轴 3~上X轴
	 * @param paramtype		0~主刻度线度 1~次刻度线长 2~主刻度个数 3~次刻度个数 4~显示标签 5~用于绘制标签和刻度的额外增加的大小
	 */
	int getTickParam(int axis, int paramtype);
	LvChartCursor* getCursor();

};

typedef STTChart* SttChart;


/**
 * @ingroup widget
 * @brief 实时曲线图
 * @image html image/realtrend_main.png
 *
 * 与chart的区别：
 * -# 默认设置下，仅需设置[显示数据]属性即可\n
 * -# X,Y轴标签根据设置自动生\n
 * -# 主刻度线与分隔线共线
 */
class STTRealTrend: public STTChart {
public:
	/**
	 * @HideCA
	 * @brief 实时趋势图控件构造
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTRealTrend(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 实时趋势图控件析构
	 */
	virtual ~STTRealTrend();
	/**
	 * @LEVEL1
	 * @brief 设置数据源
	 * @param vars 		变量清单"vn1[;v2;...;vn]"
	 * @param cfgs		附加json格式配置{"xu":5,"xs":30,...} \n
	 * xu~mXUnit:X轴显示格式(0~SS 1~MM 2~HH 3~MM:SS 4~HH:MM 5~HH:MM:SS) \n
	 * xs:X轴总时间跨度(为0时，取xsv变量；大于等30时，为实际值；其他值无效) \n
	 * xsu:X轴单位(0~秒 1~分 2~小时) \n
	 * xsv:决定X轴总时长的变量。当xs==0时有效mRefVars[0] \n
	 * y1n/y2n:Y1/Y2轴下限变量名，为空时跳过设置mRefVars[1]/mRefVars[3] \n
	 * y1m/y2m:Y1/Y2轴上限变量名，为空时跳过设置mRefVars[2]/mRefVars[4] \n
	 * m:0~绝对时钟趋势曲线 1~相对变量趋势曲线 \n
	 * mv:当m==1时有效mRefVars[5] \n
	 */
	virtual void setDataSource(const string& vars, const string& cfgs="");

	/**
	 * @LEVEL2
	 * @brief 设置标签的小数位数
	 * @param xfraclen X轴标签小数位数
	 * @param y1fraclen Y1轴标签小数位数
	 * @param y2fraclen Y2轴标签小数位数
	 */
	void setFractionLen(int xfraclen = 0,int y1fraclen = 0, int y2fraclen=0);
	/**
	 * @brief 设置x轴标签模式 0~向左移动 1~不动但时间变化
	 */
	void setXLabelMode(int mode);
	/**
	 * @brief 获取x轴标签模式 0~向左移动 1~不动但时间变化
	 */
	int getXLabelMode();
	/**
	 * @LEVEL1
	 * @brief 启动采集
	 */
	virtual bool startCollect();
	/**
	 * @LEVEL1
	 * @brief 暂停采集
	 */
	virtual bool pauseCollect();
	/**
	 * @LEVEL1
	 * @brief 继续采集
	 */
	virtual bool resumeCollect();
	/**
	 * @LEVEL1
	 * @brief 停止采集
	 */
	virtual bool stopCollect();
	/**
	 * @LEVEL1
	 * @brief 设置采样变量的因数和偏移量(实时曲线仅对本控件有效，历史曲线对相应的采样有效)
	 * @note 采样结果 = 实际值 * factor + offset
	 */
	virtual void setFactorAndOffset(const string& varname, double factor, double offset);
public:
    /**
     * @HideCA
     * @brief 内部指针
     */
	void* mTrend;
protected:
    /**
     * @HideCA
     * @brief 内部函数
     */
	void refreshDraw();
    /**
     * @HideCA
     * @brief 内部函数
     */
	virtual void onTimerCb(uint16_t code, LvEvent e);
    /**
     * @HideCA
     * @brief 内部函数
     */
	virtual void onXSpanChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     */
    void onY1MinChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     */
    void onY1MaxChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     */
    void onY2MinChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     */
    void onY2MaxChanged(SttObject * org, IVariable* data);
};

typedef STTRealTrend* SttRealTrend;
/**
 * @ingroup widget
 * @brief 历史曲线图
 * @image html image/realtrend_main.png
 *
 * 与STTRealTrend的区别：
 * -# 需绑定采样
 * -# 需绑定日期变量，日期变量格式为整数：yyyymmdd
 */
class STTHistoryTrend: public STTRealTrend {
public:
	/**
	 * @HideCA
	 * @brief 历史趋势图控件构造（继承实时趋势线）
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTHistoryTrend(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 实时趋势图控件析构
	 */
	virtual ~STTHistoryTrend();
	/**
	 * @Level2
	 * @brief 组合STTChart.setDataSource, STTRealTrend.setDataSource
	 * @param sampleid 趋势线的数据源(采样id)
	 * @param yyyymmdd 趋势线的数据源(采样日期)
	 * @param isrt 趋势线的数据源(是否实时数据，传入false)
	 * @param var 变量清单"vn1[;v2;...;vn]"
	 * @param cfgs		附加json格式配置{"xu":5,"xs":30,...} \n
	 * xu~mXUnit:X轴显示格式(0~SS 1~MM 2~HH 3~MM:SS 4~HH:MM 5~HH:MM:SS) \n
	 * xs:X轴总时间跨度(为0时，取xsv变量；大于等30时，为实际值；其他值无效) \n
	 * xsu:X轴单位(0~秒 1~分 2~小时) \n
	 * xsv:决定X轴总时长的变量。当xs==0时有效mRefVars[0] \n
	 * y1n/y2n:Y1/Y2轴下限变量名，为空时跳过设置mRefVars[1]/mRefVars[3] \n
	 * y1m/y2m:Y1/Y2轴上限变量名，为空时跳过设置mRefVars[2]/mRefVars[4] \n
	 * m:0~绝对时钟趋势曲线 1~相对变量趋势曲线 \n
	 * mv:当m==1时有效mRefVars[5] \n
	 */
	void setDataSource(const string& sampleid, const string& yyyymmdd="", bool isrt = true
			, const string& vars = "", const string& cfgs = "");
	/**
	 * @brief 获取数据源的连接
	 */
	IDatabase * getSampleDB();
	/**
	 * @brief 跳转第一页，对历史曲线的历史数据有效
	 * @return
	 */
	virtual void navFirst();
	/**
	 * @brief 延时delayms毫秒后，跳转到第1页
	 */
	void navFirst(int delayms);
	/**
	 * @LEVEL1
	 * @brief 跳转下一页，对历史曲线的历史数据有效
	 */
	virtual void navNext();
	/**
	 * @LEVEL1
	 * @brief 跳转上一页，对历史曲线的历史数据有效
	 */
	virtual void navBack();
	/**
	 * @LEVEL1
	 * @brief 跳转最后一页，对历史曲线的历史数据有效
	 */
	virtual void navLast();
	/**
	 * @brief 延时delayms毫秒后，跳转到最后1页
	 */
	void navLast(int delayms);
	/**
	 * @LEVEL1
	 * @brief 跳转到指定时间位置
	 * @param is_start 指定的是起始时间还是结束时间
	 * @note 对历史曲线的历史数据有效
	 */
	virtual void navTo(int year,int month, int day, int hour,int minute,int second,bool is_start);
	/**
	 * @LEVEL1
	 * @brief 跳转到指定时间范围
	 * @param is_start 指定的是起始时间还是结束时间
	 * @param msspan 跨度范围，单位ms
	 * @note 对历史曲线的历史数据有效，本方法只能单独使用，即，后面调用navFirst/navBack/navLast/navNext结合使用时，指定的跨度msspan无效
	 */
	virtual void navTo(int year,int month, int day, int hour,int minute,int second,bool is_start, uint64_t msspan);
	/**
	 * @LEVEL1
	 * @brief 前移(移动量为跨度的n分之1,n为图表的垂直分隔段数)
	 */
	virtual void moveNext();
	/**
	 * @LEVEL1
	 * @brief 后移(移动量为跨度的n分之1,n为图表的垂直分隔段数)
	 */
	virtual void moveBack();

	/**
	 * @LEVEL1
	 * @brief 绑定一个历史数据的日期变量yyyymmdd
	 * @param v
	 * @note 仅对历史曲线的历史数据有效
	 */
	virtual void bindHistoryDate(Ivariable v);
	/**
	 * @LEVEL1
	 * @brief 设置历史曲线日期，小于20240101表示当天
	 * @note 仅对历史曲线的历史数据有效
	 */
	virtual void setHistoryDate(int yyyymmdd);
	/**
	 * @LEVEL1
	 * @brief 获取历史曲线绑定的日期
	 */
	int getHistoryDate();
	/**
	 * @LEVEL1
	 * @brief 启动采集
	 * @note 仅对历史曲线的实时模式有效
	 */
	virtual bool startCollect();
	/**
	 * @LEVEL1
	 * @brief 暂停采集
	 * @note 仅对历史曲线的实时模式有效
	 */
	virtual bool pauseCollect();
	/**
	 * @LEVEL1
	 * @brief 继续采集
	 * @note 仅对历史曲线的实时模式有效
	 */
	virtual bool resumeCollect();
	/**
	 * @LEVEL1
	 * @brief 停止采集
	 * @note 仅对历史曲线的实时模式有效
	 */
	virtual bool stopCollect();
	/**
	 * @LEVEL1
	 * @brief 设置实时模式刷新周期
	 * @param s	秒数
	 * @return
	 * @note 要stopCollect后使用
	 */
	bool setRefreshPeriod(int s=1);
	/**
	 * @LEVEL2
	 * @brief 设置[历史曲线历史数据]的时间固定部分 1~日 2~时 3~分，其他值无效
	 * @note 如设为1，则右端为某年某月某日的   0时0分0秒
	 * 		 如设为2，则右端为某年某月某日的某时  0分0秒
	 * 		 如设为3，则右端为某年某月某日的某时某分的0秒
	 */
	void setFixTime(int fixtype);
	/**
	 * @LEVEL2
	 * @brief 设置降采样类型 0(默认)~取过去2个周期内的是近值 1~最近2个周期内的相邻值进行插值 2~lttb算法
	 */
	void setDownSamplingType(int t = 0);
public:
    /**
     * @HideCA
     * @brief 内部指针
     */
	void* mHistroy;
protected:
    /**
     * @HideCA
     * @brief 内部函数
     */
	virtual void onTimerCb(uint16_t code, LvEvent e);
    /**
     * @HideCA
     * @brief 内部函数
     */
    virtual void onHistoryDateChanged(SttObject * org, IVariable* data);
	/**
	 * @HideCA
	 * @brief 刷新显示
	 */
	void refine(bool autoCalSpan = true);
    /**
     * @HideCA
     * @brief 内部函数
     */
	virtual void onXSpanChanged(SttObject * org, IVariable* data);

public:
	/**
	 * @brief 进入历史数据浏览模式，只对周期式有用
	 * @param dataSize 每次显示的数据量，最大不超过65536
	 * @param nPeriod 周期倍数(如设为3，则每3条数据取1条)
	 */
	void browseModeEnter(uint16_t dataSize,  uint16_t nPeriod);
	/**
	 * @brief 放大（x轴变短）
	 */
	void browseModeZoomIn();
	/**
	 * @brief 缩小（x轴变长，显示滚动条）
	 */
	void browseModeZoomOut();
	/**
	 * @brief 上翻
	 */
	void browseModeToPrev();
	/**
	 * @brief 下翻
	 */
	void browseModeToNext();
	/**
	 * @brief 退出浏览模式
	 */
	void browseModeExit();
	/**
	 * @LEVEL1
	 * @brief 设置采样变量的因数和偏移量(实时曲线仅对本控件有效，历史曲线对相应的采样有效)
	 * @note 采样结果 = 实际值 * factor + offset
	 */
	virtual void setFactorAndOffset(const string& varname, float factor, float offset);
private:
    /**
     * @HideCA
     * @brief 内部函数
     */
	//void onRefreshNotify(SttObject * org, IVariable* data);
	void onRefreshNotify(std::shared_ptr<AnyMessage> d);

};
typedef STTHistoryTrend* SttHistoryTrend;

}
