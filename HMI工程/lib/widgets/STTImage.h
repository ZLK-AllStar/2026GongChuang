/*
 * STTImage.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "type/Types.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 图片
 */
class STTImage: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建图片控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
    STTImage(IRuntime* rt, STTWidget* par, const string& id="");
	/**
	 * @HideCA
	 * @brief 图片控件析构
	 */
    virtual ~STTImage();
    /**
     * @LEVEL2
     * @brief 设置控件基本参数
     * 注意：不同大小的控件，不能复用同一个图片文件。即不同大小的控件，必须指定不同的图片文件。
     * @param x x坐标
     * @param y y坐标
     * @param w 宽
     * @param h 高
     * @param img 图片路径
     */
    void setMainArg(int x=0, int y=0, int w=200, int h=200, const string img = "");
    /**
     * @LEVEL1
     * @brief 设置不同状态值下对应图片
     * @param state 状态值
     * @param img 图片路径
     */
    void addStateImg(const string& state, const string& img = "");
    /**
     * @LEVEL0
     * @brief 设置状态，显示相应图片
     */
    void setState(const string& state);
    /**
     * @LEVEL0
     * @brief 设置图片源
     * @param src 图片路径
     */
    STTImage* setSrc(const string& src);
    /**
     * @LEVEL1
     * @brief 设置图片水平偏移量(图片从x开始绘制，大于width-x部分又回到0处开始绘制)
     */
    STTImage* setOffsetX(int x);
    /**
     * @LEVEL1
     * @brief 设置图片垂直偏移量(图片从y开始绘制，大于heihgt-y部分又回到0处开始绘制)
     */
    STTImage* setOffsetY(int y);
    /**
     * @LEVEL0
     * @brief 设置图片旋转角度，范围0~360
     * @param angle 0.0~360.0
     */
    STTImage* setAngle(float angle);
    /**
     * @LEVEL0
     * @brief 在当前位置下，继续旋转一个角度
     * @param delta 0.0~360.0
     */
    STTImage* rotate(float delta);
    /**
     * @LEVEL0
     * @brief 设置旋转中心点位置
     * @param x 宽度/2时为水平中心
     * @param y 高度/2时为垂直中心
     */
    STTImage* setPivot(int x, int y);
    /**
     * @LEVEL0
     * @brief 设置缩放比例，取值范围1/256~256
     */
    STTImage* setZoom(float zoom);
    /**
     * @LEVEL0
     * @brief 获取图片源
     */
    const void* getSrc();
    /**
     * @LEVEL0
     * @brief 获取水平偏移量
     */
    int getOffsetX();
    /**
     * @LEVEL0
     * @brief 获取垂直偏移量
     */
    int getOffsetY();
    /**
     * @LEVEL0
     * @brief 获取旋转角度(0~360)
     */
    float getAngle();
    /**
     * @LEVEL0
     * @brief 获取旋转中心点
     */
    LvPoint getPivot();
    /**
     * @LEVEL0
     * @brief 获取缩放比例
     */
    float getZoom();
    /**
     * @HideCA
     * @brief 刷新
     */
    void refresh();
public:
    /**@HideCA
     * @brief 绑定变量变化响应方法
     */
    void onDataChanged(SttObject * org, IVariable* data);
    /**@HideCA
     * @brief 内部指针*/
    void* mImage;
    /**
     * @HideCA
     * @brief 使指定的图片缓存无效。详细可见lvgl的lv_img_cache_invalidate_src
     * @param src 已经使用的图片文件名，为NULL时，清空所有图片缓存
     */
    static void cacheInvalidateSrc(void* src);

};

typedef STTImage* SttImage;

}
