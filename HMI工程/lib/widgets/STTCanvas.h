/*
 * STTCanvas.h
 *
 *  Created on: 2023年2月18日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 画布
 * @image html image/canvas_main.png
 */
class STTCanvas: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 画布控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTCanvas(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 画布控件析构
	 */
	virtual ~STTCanvas();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x X坐标(默认0)
     * @param y Y坐标(默认0)
     * @param w 宽度(默认200)
     * @param h 高度(默认200)
     * @param c 背景色(默认白色)
     * @param opa 不透明度，范围0~255(默认255)
     * @param img 要绘制的图片，默认""（不是背景图，按原始大小绘制，不会缩放）
     */
    void setMainArg(int x, int y, int w, int h, int c = 0xFFFFFF, int opa = 255, const string img = "");
    /**
     * @LEVEL2
     * @brief 为画布指定内存
     * @param buf 事先分配好的内存指针，大小为(lv_img_color_format_get_px_size(cf) * w) / 8 * h)
     * 可以使用lv_mem_alloc()或静态数组分配lv_color_t buf[100*50]，若buf为NULL，则分配内存给cbuf
     * @param w 画布宽，默认200
     * @param h 画布高，默认200
     * @param cf 见lv_img_cf_t.默认4(LV_IMG_CF_TRUE_COLOR)
     */
    STTCanvas* setBuffer(void* buf, int w = 200, int h = 200, int cf = 5);
    /**
     * @LEVEL0
     * @brief 指定像素颜色
     * @param x X坐标
     * @param y Y标标
     * @param c 颜色，默认黑色
     */
    STTCanvas* setPixelColor(int x, int y, int c = 0x0);
    /**
     * @LEVEL0
     * @brief 指定像素不透明度
     * @param x X坐标
     * @param y Y标标
     * @param 不透明度，范围 (0..255)，默认255
     */
    STTCanvas* setPixelOpa(int x, int y, int opa = 255);
    /**
     * @LEVEL1
     * @brief 设置调色板颜色
     * @param id 调色板颜色id
     *   - 单色图 `LV_IMG_CF_INDEXED1`: 0..1
     *   - 4色图 `LV_IMG_CF_INDEXED2`: 0..3
     *   - 16色图 `LV_IMG_CF_INDEXED4`: 0..15
     *   - 256色图 `LV_IMG_CF_INDEXED8`: 0..255
     * @param c 颜色，默认黑色
     */
    STTCanvas* setPalette(int id=0, int c = 0x0);
    /**
     * @LEVEL0
     * @brief 获取像素颜色
     */
    int getPixelColor(int x=0, int y=0);
    /**
     * @LEVEL2
     * @brief 获取画布图像信息
     */
    LvImgDsc* getImage();
    /**
     * @LEVEL2
     * @brief 获取画布内存
     */
    LvColor* getBuffer();
    /**
     * @HideCA
     * @brief 变换图像并保存在画布上
     * @param img 要变换的图像，如其他画布的图片lv_canvas_get_img()
     * @param angle 旋转角度，范围 (0..360)
     * @param zoom 放大倍数，如需缩小，请使用小数,范围1/256 ~ 256
     * @param offset_x 显示在目标画布(this)上的x偏移量
     * @param offset_y 显示在目标画布(this)上的y偏移量
     * @param pivot_x 源图旋转中心X坐标, 源图宽/2为中心
     * @param pivot_y 源图旋转中心Y坐标, 源图高/2为中心
     * @param antialias 是否抗锯齿
     */
    STTCanvas* transform(LvImgDsc* img, int angle, int zoom=1, int offset_x=0, int offset_y=0, int pivot_x=0, int pivot_y=0, bool antialias=true);
    /**
     * @LEVEL2
     * @brief 变换图像并保存在画布上
     * @param img 要变换的图像，如其他画布的图片lv_canvas_get_img()
     * @param angle 旋转角度，范围 (0..360)
     * @param zoom 放大倍数，如需缩小，请使用小数,范围1/256 ~ 256
     * @param offset_x 显示在目标画布(this)上的x偏移量
     * @param offset_y 显示在目标画布(this)上的y偏移量
     * @param pivot_x 源图旋转中心X坐标, 源图宽/2为中心
     * @param pivot_y 源图旋转中心Y坐标, 源图高/2为中心
     * @param antialias 是否抗锯齿
     */
    STTCanvas* transform(LvImgDsc* img, float angle, float zoom=1.0f, int offset_x=0, int offset_y=0, int pivot_x=0, int pivot_y=0, bool antialias=true);
    /**
     * @LEVEL1
     * @brief 水平模糊
     * @param area 模糊区域(x1行到x2行、y1列到y2列)，为NULL是，整个画布模糊
     * @param r 模糊半径
     */
    STTCanvas* blurHor(const LvArea* area, int r);
    /**
     * @LEVEL1
     * @brief 垂直模糊
     * @param area 模糊区域(x1行到x2行、y1列到y2列)，为NULL是，整个画布模糊
     * @param r 模糊半径
     */
    STTCanvas* blurVer(const LvArea* area, int r);
    /**
     * @LEVEL0
     * @brief 填充画布颜色
     * @param color 颜色
     * @param opa 不透明度，范围0..255，默认255
     */
    STTCanvas* fillBg(uint32_t color=0xffffff, int opa=255);
    /**
     * @HideCA
     */
    STTCanvas* fillBg(LvColor* color, int opa=255);
    /**
     * @LEVEL1
     * @brief 画矩形
     * @param x 矩形x坐标
     * @param y 矩形y坐标
     * @param w 矩形宽
     * @param h 矩形高
     * @param draw_dsc 矩形绘制参数，背景、边框、阴影等，默认NULL
     */
    STTCanvas* drawRect(int x=0, int y=0, int w=100, int h=100, const LvDrawRectDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 画文字
     * @param x 文本x坐标
     * @param y 文本y坐标
     * @param max_w    文本的最大宽度，超出将换行
     * @param draw_dsc 文本绘制参数，颜色、透明度、文字间距等等，默认NULL
     * (注意：draw_dsc构造时要传入字体,如getRuntime()->getIFntMgr()->getFont("文泉驿微米黑",size))
     * @param txt      文本内容
     */
    STTCanvas* drawText(int x=0, int y=0, int max_w=200, LvDrawLabelDsc* draw_dsc=NULL, const char* txt="");
    /**
     * @LEVEL1
     * @brief 画图片(注意图片大小不能超过画布大小)
     * @param x        x坐标(图片的左边到画布的左边的距离)
     * @param y        y坐标(图片的上边到画布的上边的距离)
     * @param src      源图片
     * @param draw_dsc 图片的旋转、平移、缩放等。默认NULL(默认反锯齿)
     */
    STTCanvas* drawImage(int x, int y, const void* src, const LvDrawImgDsc* draw_dsc = NULL);
    /**
     * @LEVEL2
     * @brief 用坐标点数组画线段
     * @param points     坐标点
     * @param point_cnt  点数
     * @param draw_dsc   线的颜色、线型等，默认为NULL，默认线宽2，黑色
     */
    STTCanvas* drawLine(const LvPoint points[], int point_cnt, const LvDrawLineDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 用数值数组画线段
     * @param xy 坐标对数组，如两个点(0,0),(100,100)为：{0,0,100,100}
     * @param point_cnt  点数
     * @param draw_dsc   线的颜色、线型等，默认为NULL，默认线宽2，黑色
     */
    STTCanvas* drawLine(int xy[], int point_cnt, const LvDrawLineDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 画线段
     * @param draw_dsc   线的颜色、线型等，默认为NULL，默认线宽2，黑色
     */
    STTCanvas* drawLine(int x1,int y1,int x2,int y2, const LvDrawLineDsc* draw_dsc = NULL);
    /**
     * @LEVEL2
     * @brief 用坐标点数组画多边形。
     * 多边形不绘制边框，如需要画多边形边框，请配合DrawLine使用
     * @param points    坐标点
     * @param point_cnt 点数
     * @param draw_dsc  颜色、透明度、圆角等，默认为NULL
     */
    STTCanvas* drawPolygon(const LvPoint points[], int point_cnt, const LvDrawRectDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 用数值数组画多边形。
     * 多边形不绘制边框，如需要画多边形边框，请配合DrawLine使用
	 * @param xy 坐标对数组，如两个点(0,0),(100,100)为：{0,0,100,100}
	 * @param point_cnt 点数
	 * @param draw_dsc  颜色、透明度、圆角等，默认为NULL
	 */
    STTCanvas* drawPolygon(int xy[], int point_cnt, const LvDrawRectDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 画圆弧
     * 从start_angle到end_angle，顺时针画
     * @param x      x坐标 = 圆心x - r
     * @param y      y坐标 = 圆心y - r
     * @param r      弧线半径（含线宽）
     * @param start_angle 起始角。右是0度，顺时针方向增大：下是90度，左是180度，上是270。
     * @param end_angle   结束角。右是0度，顺时针方向增大：下是90度，左是180度，上是270。
     * @param draw_dsc    为NULL时，默认线宽2，黑色
     */
    STTCanvas* drawArc(int x=0, int y=0, int r=50, int start_angle=135, int end_angle=45, const LvDrawArcDsc* draw_dsc = NULL);
    /**
     * @LEVEL1
     * @brief 设置主背景图片(效果同this->drawImage，注意图片大小不能超过画布大小)
     */
    virtual STTWidget* setBgImg(const char* path, int part = stt::constant::Part::Main);
public:
    /**
     * @HideCA
     * @brief 内部指针
     **/
    void* mCnvs;
};
typedef STTCanvas* SttCanvas;

}
