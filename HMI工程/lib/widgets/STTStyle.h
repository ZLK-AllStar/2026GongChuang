/*
 * STTStyle.h
 *
 *  Created on: 2023年2月14日
 *      Author: SIMTUTAI
 */
#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <functional>

namespace stt{

/**********************
 *  LVGL TYPE MAPPING
 **********************/
#define USE_LARGE_COORD 1
#if USE_LARGE_COORD==1
typedef int32_t LvCoord;
#else
typedef int16_t LvCoord;
#endif

/** @brief Point(lv_point_t) */
struct LvPoint{
	LvCoord x;
	LvCoord y;
	LvPoint() {
		x = y = 0;
	}
	LvPoint(int xx,int yy){
		x = xx;
		y = yy;
	}
};

/** @brief Area(lv_area_t) lv中用于存储绝对坐标*/
struct LvArea{
	LvCoord x1;	//左边x坐标
	LvCoord y1;	//上边y坐标
	LvCoord x2;	//右边x坐标
	LvCoord y2;	//下边y坐标
};

/** @brief color(lv_color_t) */
typedef union {
    struct {
        uint8_t blue;
        uint8_t green;
        uint8_t red;
        uint8_t alpha;
    } ch;
    uint32_t full;
} LvColor;

/** @brief 图片文件头(lv_img_header_t) */
struct LvImgHeader{
    uint32_t cf : 5;          	/*Color format: See `lv_img_color_format_t`*/
    uint32_t always_zero : 3; 	/*It the upper bits of the first byte. Always zero to look like a
                                 non-printable character*/
    uint32_t reserved : 2; 		/*Reserved to be used later*/
    uint32_t w : 11; 			/*Width of the image map*/
    uint32_t h : 11; 			/*Height of the image map*/
};

/** @brief 图片描述(lv_img_dsc_t) */
typedef struct {
	LvImgHeader header; 	/**< A header describing the basics of the image*/
    uint32_t data_size;     /**< Size of the image in bytes*/
    const uint8_t * data;   /**< Pointer to the data of the image*/
} LvImgDsc;

/** @brief 渐变终止(lv_gradient_stop_t) */
struct LvGradientStop{
	LvColor 	color;   /**< The stop color */
    uint8_t    	frac;    /**< The stop position in 1/255 unit */
};

/** @brief 渐变描述(lv_grad_dsc_t) */
struct LvGradDsc{
	LvGradientStop  	stops[2]; /**LV_GRADIENT_MAX_STOPS=2 < A gradient stop array */
    uint8_t             stops_count;                  /**< The number of used stops in the array */
    uint8_t        		dir : 3;                      /**< The gradient direction.
                                                        * Any of LV_GRAD_DIR_HOR, LV_GRAD_DIR_VER, LV_GRAD_DIR_NONE */
    uint8_t     		dither : 3;                   /**< Whether to dither the gradient or not.
                                                        * Any of LV_DITHER_NONE, LV_DITHER_ORDERED, LV_DITHER_ERR_DIFF */
};

/** @brief 矩形绘制描述(lv_draw_rect_dsc_t) */
struct LvDrawRectDsc{

	LvCoord radius;	//圆倒角半径
	uint8_t blend_mode;//混合模式 0~按透明度简易混合 1~加 2~减 3~乘 4~替换

    /*Background*/
    uint8_t bg_opa;//背景 透明度
    LvColor bg_color; //背景色       /**< First element of a gradient is a color, so it maps well here*/
    LvGradDsc bg_grad; //渐变参数

    /*Background img*/
    const void * bg_img_src;//背景图片
    const void * bg_img_symbol_font;//背景符号字体
    LvColor bg_img_recolor;//背景图重着色(会把图片上的颜色变成这个颜色)
    uint8_t bg_img_opa;//背景图透明度
    uint8_t bg_img_recolor_opa;//背景图重着色透明度
    uint8_t bg_img_tiled;//是否平铺

    /*Border*/
    LvColor border_color;//边框颜色
    LvCoord border_width;//边框宽度
    uint8_t border_opa;//边框透明度
    uint8_t border_post : 1; //是否延迟给制       /*There is a border it will be drawn later.*/
    uint8_t border_side : 5; //绘制哪边 0无 | 1下 | 2上 | 4左 | 8右

    /*Outline*/
    LvColor outline_color;//外轮廓颜色
    LvCoord outline_width;//外轮廓宽度
    LvCoord outline_pad;//外轮廓间隙
    uint8_t outline_opa;//外轮廓透明度

    /*Shadow*/
    LvColor shadow_color;//阴影颜色
    LvCoord shadow_width;//阴影宽度
    LvCoord shadow_ofs_x;//水平偏移
    LvCoord shadow_ofs_y;//垂直偏移
    LvCoord shadow_spread;//扩散度
    uint8_t shadow_opa;//透明度
    LvDrawRectDsc(void * p);/*p 保留*/
	LvDrawRectDsc();
};

/** @brief 圆弧绘制描述(lv_draw_arc_dsc_t) */
struct LvDrawArcDsc{
    LvColor color;//颜色
    LvCoord width;//线粗
    uint16_t start_angle;//起始角:向右为0度，顺时针增大 (与数学相反)
    uint16_t end_angle;//终止角:向右为0度，顺时针增大 (与数学相反)
    const void * img_src;//背景图片
    uint8_t opa;//透明度
    uint8_t blend_mode  : 2;//混合模式(0~按不透明度简单混合 1~颜色相加 2~减掉前景颜色 3~颜色相乘 4~将背景色替换掉)
    uint8_t rounded : 1;//是否加圆帽
    LvDrawArcDsc(void* p);/*p 保留*/
    LvDrawArcDsc();
};

/** @brief 标签绘制描述(lv_draw_label_dsc_t) */
struct LvDrawLabelDsc{
    void* font;	//lv_font_t*//字体
    uint32_t sel_start;//选中文本的起始索引
    uint32_t sel_end;//选中文本的结束索引
    LvColor color;//颜色
    LvColor sel_color;//选中文本颜色
    LvColor sel_bg_color;//选中文本背影色
    LvCoord line_space;//行间距
    LvCoord letter_space;//字间距
    LvCoord ofs_x;//水平偏移
    LvCoord ofs_y;//垂直偏移
    uint8_t opa;//透明度
    uint8_t bidi_dir;//基本方向 0~LTR 1~RTL 2~AUTO 32~NEUTRAL 33~WEAK
    uint8_t align;//对方方式 0~自动 1~左 2~中 3~右
    uint8_t flag;//渲染标志 0~无 1~RECOLOR | 2~自动扩展单行宽度 | 4~宽度适应文字最长的一行
    uint8_t decor : 3;// 0~无 1~下划线 2~中划线
    uint8_t blend_mode: 3;//混合模式
    LvDrawLabelDsc(void* lvfnt);
    LvDrawLabelDsc();
};

/** @brief 图片绘制描述(lv_draw_img_dsc_t) */
struct LvDrawImgDsc{
    int16_t angle;//旋转角度0~360
    uint16_t zoom;//用于计算缩放比例，取整数。实际缩放比例=zoom/256
    LvPoint pivot;//旋转轴心
    LvColor recolor;//重着色
    uint8_t recolor_opa;//重着色的透明度
    uint8_t opa;//透明度
    uint8_t blend_mode : 4;//混合模式 0~按透明度简易混合 1~加 2~减 3~乘 4~替换
    int32_t frame_id;//用于解码器，绘制画布无意义
    uint8_t antialias       : 1;//是否抗锯齿
};

/** @brief 直线绘制描述(lv_draw_line_dsc_t) */
struct LvDrawLineDsc{
    LvColor color;//颜色
    LvCoord width;//线粗
    LvCoord dash_width;//虚线的实线部分长度
    LvCoord dash_gap;//虚线的空白部分长度
    uint8_t opa;//透明度
    uint8_t blend_mode  : 2;//混合模式
    uint8_t round_start : 1;//起点是否加圆帽
    uint8_t round_end   : 1;//终点是否加圆帽
    uint8_t raw_end     : 1;    /*Do not bother with perpendicular line ending if it's not visible for any reason*/
    LvDrawLineDsc(void * p);
    LvDrawLineDsc();
};

/** @brief chart系列(lv_chart_series_t) */
struct LvChartSeries{
	LvCoord * x_points;
	LvCoord * y_points;
    LvColor color;
    uint16_t start_point;
    uint8_t hidden : 1;
    uint8_t x_ext_buf_assigned : 1;
    uint8_t y_ext_buf_assigned : 1;
    uint8_t x_axis_sec : 1;
    uint8_t y_axis_sec : 1;
    uint8_t fixFirstLine : 1;
};

/** @brief chart光标(lv_chart_cursor_t) */
struct LvChartCursor{
    LvPoint pos;
    LvCoord point_id;
    LvColor color;
    LvChartSeries * ser;
    uint8_t dir;
    uint8_t pos_set: 1; /*1: pos is set; 0: point_id is set*/
};

/** @brief 仪表刻度(lv_meter_scale_t) */
struct LvMeterScale{
    LvColor tick_color;
    uint16_t tick_cnt;
    uint16_t tick_length;
    uint16_t tick_width;

    LvColor tick_major_color;
    uint16_t tick_major_nth;
    uint16_t tick_major_length;
    uint16_t tick_major_width;

    int16_t label_gap;
    int16_t label_color;

    int32_t min;
    int32_t max;
    int16_t r_mod;
    uint16_t angle_range;
    int16_t rotation;
};

/** @brief 仪表指示器(lv_meter_indicator_t) */
struct LvMeterIndicator{
	LvMeterScale * scale;
	uint8_t indicator_type;
	uint8_t opa;
    int32_t start_value;
    int32_t end_value;
    union {
        struct {
            const void * src;
            LvPoint pivot;
        } needle_img;
        struct {
            uint16_t width;
            int16_t r_mod;
            LvColor color;
        } needle_line;
        struct {
            uint16_t width;
            const void * src;
            LvColor color;
            int16_t r_mod;
        } arc;
        struct {
            int16_t width_mod;
            LvColor color_start;
            LvColor color_end;
            uint8_t local_grad  : 1;
        } scale_lines;
    } type_data;
};

/** @brief 样式值(lv_style_value_t) */
typedef union {
    int32_t num;         /**< Number integer number (opacity, enums, booleans or "normal" numbers)*/
    const void * ptr;    /**< Constant pointers  (font, cone text, etc)*/
    LvColor color;    /**< Colors*/
}LvStyleValue;
/** @brief 样式属性(lv_style_prop_t)*/
typedef struct {
	uint16_t prop;		//枚举lv_style_prop_t这里用uint16_t，需检查
    LvStyleValue value;
} LvStyleConstProp;
/** @brief 样式(lv_style_t) */
typedef struct {
    union {
    	LvStyleValue value1;
        uint8_t * values_and_props;
        const LvStyleConstProp * const_props;
    } v_p;

    uint16_t prop1;
    uint8_t has_group;
    uint8_t prop_cnt;
} LvStyle;


/**********************
 *      CLASSES
 **********************/
/**
 * @ingroup style
 * @brief 样式属性基类
 */
class STTStyleProp
{
public:
	/**
	 *
	 * @param obj	lvgl控件对象
	 */
	STTStyleProp(void* obj);
	virtual ~STTStyleProp();
    uint32_t selector;
    void attach(void *obj);
protected:
    void* object;
};

/**
 * @ingroup style
 * @brief 尺寸样式类
 */
class STTStyleSize : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleSize(void* obj);

	/**
	 * @LEVEL0
	 * @brief 设置宽度值
	 * @param width		宽度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setWidth(int width, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置最小宽度值
	 * @param minw		最小宽度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setMinWidth(int minw, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置最大宽度值
	 * @param maxw		最大宽度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setMaxWidth(int maxw, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置宽度值
	 * @param height	高度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setHeight(int height, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置最小高度值
	 * @param minh		最小高度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setMinHeight(int minh, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置最大高度值
	 * @param maxh		最大高度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleSize* setMaxHeight(int maxh, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设为正方形，即将宽高设为同一值
     * @param value		边长
     * @param selector 	部件、状态选择器，见STTStyle::_selector
    */
	STTStyleSize* setSize(int value, int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取宽度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getWidth(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取最小可设置宽度值
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getMinWidth(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取最大可设置宽度值
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getMaxWidth(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取高度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getHeight(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取最小可设置高度值
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getMinHeight(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取最大可设置高度值
     * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getMaxHeight(int selector = 0);

};
/**
 * @ingroup style
 * @brief 位置样式类
 */
class STTStyleLocation : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
    STTStyleLocation(void* obj);
    /**
     * @LEVEL0
     * @brief 设置x坐标
     * @param x		x坐标，右为正(原点在左上角)
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleLocation* setX(int x, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置y坐标
     * @param y		y坐标，下为正(原点在左上角)
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleLocation* setY(int y, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置对齐方式。与父对象内部对齐。
     * （注意：调用本方法后，控件的位置相对于对齐点，如控件的x坐标为100，设置中心对齐后，其位置为中心x+100）
     * @param align		对齐方式 见stt::constant::ALIGN
     * 							0~不设对齐方式
     * 							1~上	左 2~上中 3~上右
     * 							4~下左 5~下中 6~下右
     * 							7~中左 8~中右 9~中心
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleLocation* setAlign(int align, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取x坐标
     */
    int getX(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取y坐标
     */
    int getY(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取与父对象之间的对齐方式 	见stt::constant::ALIGN
     * 							0~不设对齐方式
     * 							1~上	左 2~上中 3~上右
     * 							4~下左 5~下中 6~下右
     * 							7~中左 8~中右 9~中心
     */
    int getAlign(int selector = 0);
};

/**
 * @ingroup style
 * @brief 动画样式类
 */
class STTStyleAnim : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
    STTStyleAnim(void* obj);
    /**
     * @LEVEL0
     * @brief 设置动画时长
     * @param ms		毫秒
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleAnim* setTime(int ms, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置动画速度
     * @param speed		像素/秒
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleAnim* setSpeed(int speed, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取动画时长
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getTime(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取动画速度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getSpeed(int selector = 0);
public:
    /**
     * @LEVEL2
     * @brief 给控件增加动画效果，并执行
     * @param sttwidget 控件
     * @param vstart 指定动画属性的起始值
     * @param vend 指定动务属性的终止值
     * @param duration 动画持续时间
     * @param valuecb 动画属性值变化过程中执行的回调函数(vstart~vend之间变化时，调用本方法)
     * @param finishcb 动画结束时执行的回调函数
     * @param repeat	重复次数，<0时无限次， =0时不重复， >0时为次数
     * @param playbackdelay 反向播放延时(当动画正放完毕时，会倒放)
     * @param playbacktime  反向播放时间(当动画正放完毕时，会倒放)
     * @param pathcb 动画效果 0~线性 1~easeIn 2~easeOut 3~easeInOut 4~overshoot 5~bounce 6~step
     * @return 返回lv_anim_t *指针
     */
    void* doAnim(void* sttwidget,int32_t vstart, int32_t vend, uint32_t duration
    		,void(*valuecb)(void*,int32_t)
    		,void(*finishcb)(void*)
			,int repeat=-1
			,int playbackdelay=0
			,int playbacktime=0
			,int pathcb=0
			);
    /**
     * @brief 终止控件的的动列列表中的第1个动画
     * @param sttwidget stt控件
     * @param a 指定要删除的动画(由doAnim返回)。为NULL时，则删除第1个动画
     * @return true:至少有1个动画被删除 false:没有动画被删除
     */
    bool stopAnim(void* sttwidget, void* a = NULL);
    /**
     * @LEVEL2
     * @brief 获取动画的控件。针对doAnim产生的动画，在动画结束时的回调函数中使用
     */
    static void* getSTTWidget(void* lvanim);
private:
};

/**
 * @ingroup style
 * @brief 圆弧样式类
 */
class STTStyleArc : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
    STTStyleArc(void* obj);
    /**
     * @LEVEL0
     * @brief 设置圆弧的宽度
     * @param width 	宽度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleArc* setWidth(int width, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置弧线的两端是否为圆头
     * @param isRounded	是否圆头
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleArc* setRounded(bool isRounded, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置圆弧的颜色
     * @param color 	颜色
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleArc* setColor(int color, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置圆弧的不透明度
     * @param opa		不透明度0~255
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleArc* setOpa(int opa, int selector = 0);
    /**
     * @LEVEL1
     * @brief 设置圆弧的底图
     * @param path 		图片文件路径
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleArc* setImgSrc(const void* path, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取圆弧宽度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getWidth(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取是否使用了圆头
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    bool getRounded(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取圆弧颜色
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getColor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取圆弧透明度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOpa(int selector = 0);
    /**
     * @LEVEL1
     * 获取图片路径
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    const void* getImgSrc(int selector = 0);
};

/**
 * @ingroup style
 * @brief 直线样式类
 */
class STTStyleLine : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleLine(void* obj);
	/**
	 * @LEVEL0
	 * @brief 设置线的粗线
	 * @param width		粗细
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setWidth(int width, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置一段虚线的黑色部分长度
	 * @param len_black	虚线的黑色部分长度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setDashWidth(int len_black, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置一段虚线的白色部分长度
	 * @param len_white	虚线的白色部分长度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setDashGap(int len_white, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置线的两端是否为圆头
	 * @param isRounded	true~圆头 false~平头
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setRounded(bool isRounded, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置线的颜色
	 * @param color		颜色
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setColor(int color, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置线的透明度
	 * @param opa		不透明度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleLine* setOpa(int opa, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取线的组细
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getWidth(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取一段虚线的黑色部分长度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
    int getDashWidth(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取一段虚线的白色部分长度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getDashGap(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取线的两端是否为圆头
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    bool getRounded(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取线的颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getColor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取线的透明度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOpa(int selector = 0);
};

/**
 * @ingroup style
 * @brief 阴影样式类
 */
class STTStyleShadow : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 */
    STTStyleShadow(void* obj);
    /**
     * @LEVEL0
     * @brief 设置阴影宽度
     * @param width
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setWidth(int width, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置阴影水平偏移
     * @param offsetX	水平偏移量，右为正，左为负
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setOfsX(int offsetX, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置阴影垂直偏移
     * @param offsetY	垂直偏移量，下为正，上为负
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setOfsY(int offsetY, int selector = 0);
    /**
     * @LEVEL1
     * @brief 设置阴影开始扩散位置
     * @param spread 	相对于控件边界的位置。正值扩大，负值缩小。如设为-2，则阴影将从控件内侧2像素开始向外扩散
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setSpread(int spread, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置阴影颜色
     * @param color		颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setColor(int color, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置不透明度
     * @param opa		不透明度0~255
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleShadow* setOpa(int opa, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取阴影宽度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getWidth(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取阴影水平方向偏移量
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOfsX(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取阴影垂直方向偏移量
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOfsY(int selector = 0);
    /**
     * @LEVEL1
     * @brief 获取阴影扩散的相对于控件边界起始位置。正为外扩，负为内缩
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getSpread(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取阴影颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getColor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取阴影不透明度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOpa(int selector = 0);
};

/**
 * @ingroup style
 * @brief 轮廓线样式类
 */
class STTStyleOutline : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 */
    STTStyleOutline(void* obj);
    /**
     * @LEVEL0
     * @brief 设置轮廓线粗细
     * @param width		粗细
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleOutline* setWidth(int width, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置轮廓线颜色
     * @param color		颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleOutline* setColor(int color, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置轮廓线透明度
     * @param opa		不透明度 0~255
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleOutline* setOpa(int opa, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置轮廓线内侧与控件边界的间隙
     * @param pad		间隙
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleOutline* setPad(int pad, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取轮廓线粗细
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getWidth(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取轮廓线颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getColor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取轮廓线透明度
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getOpa(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取轮廓线内侧与控件边界的间隙
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getPad(int selector = 0);
};

/**
 * @ingroup style
 * @brief 图片样式类
 */
class STTStyleImg : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleImg(void* obj);
	/**
	 * @LEVEL0
	 * @brief 设置不透明度
	 * @param opa		不透明度 0~255
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleImg* setOpa(int opa, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置重新着色的颜色，一般用于png图片
	 * @param color		颜色
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleImg* setRecolor(int color, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置重新着色的颜色透明度
	 * @param opa		不透明度 0~255
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleImg* setRecolorOpa(int opa, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取不透明度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getOpa(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取重新着色的颜色
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getRecolor(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取重新着色的颜色透明度
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getRecolorOpa(int selector = 0);
};

/**
 * @ingroup style
 * @brief 文本样式类
 */
class STTStyleText : public STTStyleProp{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
    STTStyleText(void* obj);
    /**
     * @LEVEL0
     * @brief 设置文字颜色
     * @param color 	颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setColor(int color, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置文字透明度
     * @param opa		不透明度 0~255
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setOpa(int opa, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置字体
     * @param value		通过fm.getFont获取字体
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setFont(const void* value, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置字符间距
     * @param value		字符间距
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setLetterSpace(int value, int selector = 0);
    /**
     * @LEVEL0
     * @brief 设置行间距
     * @param value		行间距
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setLineSpace(int value, int selector = 0);
    /**
     * @LEVEL0
     * @brief 带下划线、删除线
     * @param value		0~无 1~下划线 2~删除线 3~同时使用下划线和删除线
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setDecor(int value, int selector = 0);
    /**
     * @LEVEL0
     * @brief			设置文本的对齐方式
     * @param value 	0~自动 1~左 2~中 3~右
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    STTStyleText* setAlign(int value, int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取文字颜色
     * @param selector 	部件、状态选择器，见STTStyle::_selector
     */
    int getColor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取文字透明度
     */
    int getOpa(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取字体
     */
    const void* getFont(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取字符间距
     */
    int getLetterSpace(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取行间距
     */
    int getLineSpace(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取是否使用下划线、删除线
     * 0~无 1~下划线 2~删除线 3~同时使用下划线和删除线
     */
    int getDecor(int selector = 0);
    /**
     * @LEVEL0
     * @brief 获取文字对齐方式
     * 	0~自动 1~左 2~中 3~右
     */
    int getAlign(int selector = 0);
};

/**
 * @ingroup style
 * @brief 边距间隙样式类
 */
class STTStylePad : public STTStyleProp {
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStylePad(void* obj);
	/**
	 * @LEVEL0
	 * @brief 设置上边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setTop(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置下边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setBottom(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置左边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setLeft(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置右边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setRight(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置上下左右边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setAll(int value, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置行间距(flex布局\grid布局，如键盘控件)
	 * @param value		间距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setRow(int value, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置列间距(flex布局\grid布局，如键盘控件)
	 * @param value		间距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setColumn(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置左右边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setHor(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置上下边距
	 * @param value		边距值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setVer(int value, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 设置柱状图的系列之间的间距、textarea字符间隙
	 * @param value		间隙值，单位像素
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStylePad* setGap(int value, int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取上边距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getTop(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取下边距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getBottom(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取左边距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getLeft(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取右边距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getRight(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取行间距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getRow(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取列间距
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getColumn(int selector = 0);
};

/**
 * @ingroup style
 * @brief 平移样式类
 */
class STTStyleTranslate : public STTStyleProp {
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleTranslate(void* obj);
	/**
	 * @LEVEL0
	 * @brief 设置水平平移值，向右为正
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTranslate* setX(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 设置垂直平移值，向下为正
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTranslate* setY(int value, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取水平平移值，向右为正
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getX(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取垂直平移值，向下为正
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	int getY(int selector = 0);
};

/**
 * @ingroup style
 * @brief 变换(缩放、旋转)样式类
 */
class STTStyleTransform : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleTransform(void* obj);
	/**
	 * @HideCA
	 */
	virtual ~STTStyleTransform(){}
	/**
	 * @LEVEL0
	 * @brief 宽度扩大或减小(相对于控件的width)
	 * @param width 扩大的值，负数为减小，0不变
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTransform* setWidth(int width, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 高度扩大或减小(相对于控件的height)
	 * @param height 扩大的值，负数为减小，0不变
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTransform* setHeight(int height, int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 旋转角度
	 * @param angle 	旋转角度0.0~360.0(有效精度0.1度)
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTransform* setAngle(float angle, int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取宽度的扩大值，负数为缩小
	 */
	int getWidth(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取高度的扩大值，负数为缩小
	 */
	int getHeight(int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取旋转角度,返回0.0~360.0(有效精度0.1度)
	 */
	float getAngle(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置旋转轴心坐标x
	 */
	STTStyleTransform* setPivotX(int v, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 设置旋转轴心坐标y
	 */
	STTStyleTransform* setPivotY(int v, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取旋转轴心坐标x
	 */
	int getPivotX(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取旋转轴心坐标y
	 */
	int getPivotY(int selector = 0);
public:
	/**
	 * @LEVEL0 效果不能达到lvgl所述，暂时丢入private
	 * @brief 获取缩放倍数,返回1/256~256
	 */
	float getZoom(int selector = 0);
	/**
	 * @LEVEL0 效果不能达到lvgl所述，暂时丢入private
	 * @brief 缩放比例(相对于控件的width、height)
	 * @param zoom 		缩放倍数1/256 ~ 256
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleTransform* setZoom(float zoom, int selector = 0);
};

/**
 * @ingroup style
 * @brief 边框样式类
 */
class STTStyleBorder : public STTStyleProp {
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleBorder(void* obj);
	/**
	 * @HideCA
	 */
	virtual ~STTStyleBorder(){}
	/**
	 * @LEVEL0
	 * @brief 设置边框线的颜色
	 * @param color 颜色 0x0~0xFFFFFF
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBorder* setColor(int color, int selector = 0);
	/**
	 * @LEVE0
	 * @brief 设置边框线的不透明度
	 * @param opa	不透明度(0全透明~255全不透明)
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBorder* setOpa(int opa, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 设置边框线粗细
	 * @param width 边框组的粗细
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBorder* setWidth(int width, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 指定绘制哪些边框(上下左右)
	 * @param side 0~不绘制 1~下 2~上 4~左 8~右；组合，如15=1+2+4+8(上下左右)
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBorder* setSide(int side, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 设置边框绘制时机
	 * @param ispost 	true: 子类之后(边框盖住子控件), false: 子类之前(子控件盖住边框)
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBorder* setPost(bool ispost, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取边框颜色
	 */
	int getColor(int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取边框不透明度
	 * @return 返回值[0..255](0~全透明 255~完全不透明)
	 */
	int getOpa(int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取边框粗细
	 */
	int getWidth(int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取绘制了上下左右哪些边框
	 * @return 返回0~不绘制 1~下 2~上 4~左 8~右；组合，如15=1+2+4+8(上下左右)
	 */
	int getSide(int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取边框绘制时机
	 * @return true: true: 子类之后(边框盖住子控件), false: 子类之前(子控件盖住边框)
	 */
    bool getPost(int selector = 0);
};

/**
 * @ingroup style
 * @brief 背景样式类
 */
class STTStyleBg : public STTStyleProp {
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyleBg(void* obj);
	/**
	 * @HideCA
	 */
	virtual ~STTStyleBg(){};

	/**
	 * @LEVEL2
	 * @brief 通过dsc设置背景
	 * @param dsc		矩形绘制描述符
	 * @param selector 	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setFromDsc(LvDrawRectDsc* dsc, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 设置主色
	 * @param color		颜色值(0x0~0xFFFFFF)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setColor(int color, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 设置不透明度
	 * @param opa		不透明度(0~255)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setOpa(int opa, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 设置渐变终止色
	 * @param color		颜色值(0x0~0xFFFFFF)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setGradColor(int color, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 设置渐变方向
	 * @param dir		渐变方向0~不渐变 1~上下 2~左右
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setGradDir(int dir, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 设置主色渐变开始位置
	 * @param percent 	起始位置百分比，范围(0.0~1.0).如控件宽度为100，0~9是主色不变，10开始渐变，则为10/100=0.1
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setMainStop(float percent, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 设置渐变结束位置
	 * @param percent		结束位置百分比，范围(0.0~1.0).如控件宽度为100，90以后全部为渐变终止色，则为90/100=0.9
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setGradStop(float percent, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 设置背景图片
	 * @param path		图片文件路径，为NULL时表示不显示图片
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setImgSrc(const void* path, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 设置图片不透明度
	 * @param opa		不透明度(0~255)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setImgOpa(int opa, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 设置图片重新上色的颜色。一般对png图使用
	 * @param color		新的颜色(0x0~0xFFFFFF)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setImgRecolor(int color, int selector = 0 | 0);

	/**
	 * @LEVEL1
	 * @brief 设置图片重新上色的颜色不透明度。
	 * @param opa		新的不透明度(0~255)
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setImgRecolorOpa(int opa, int selector = 0);
	/**
	 * @LEVEL1
	 * @brief (无效方法)是否平铺。当图片小于控件大小时有效果
	 * @param isTiled		true则平铺，false则只显示1个
	 * @param selector	部件、状态选择器，见STTStyle::_selector
	 */
	STTStyleBg* setImgTiled(bool isTiled, int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取主色
	 */
	int getColor(int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取不透明度
	 */
	int getOpa(int selector = 0);
	/**
	 * @LEVEL0
	 * @brief 获取次色(渐变终止色)
	 */
	int getGradColor(int selector = 0);

	/**
	 * @LEVEL0
	 * @brief 获取渐变方向 0~不渐变 1~上下 2~左右
	 */
	int getGradDir(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取渐变起始位置，返回0.0~1.0
	 */
	float getMainStop(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取渐变终止位置，返回0.0~1.0
	 */
	float getGradStop(int selector = 0);
	/**
	 * @LEVEL1
	 * @brief 获取图片文件路径
	 */
    const void* getImgSrc(int selector = 0);
    /**
     * @LEVEL1
     * @brief 获取图片透明度
     */
    int getImgOpa(int selector = 0);
    /**
     * @LEVEL1
     * @brief 获取图片重着色的颜色
     */
    int getImgRecolor(int selector = 0);
    /**
     * @LEVEL1
     * @brief 获取过滤后的图片重着色的颜色（需指定过滤回调函数）
     */
    int getImgRecolorFiltered(int selector = 0);
    /**
     * @LEVEL1
     * @brief 获取图片重着色的颜色不透明度
     */
    int getImgRecolorOpa(int selector = 0);
    /**
     * @LEVEL1
     * @brief (无效方法)
     */
    bool getImgTiled(int selector = 0);
};

/**
 * @ingroup style
 * @brief 滚动条样式属性及方法
 */
class STTStylePropScrollbar : public STTStyleProp
{
public:
	/**
	 * @HideCA
	 */
	STTStylePropScrollbar(void* obj);
	/**
	 * @LEVEL0
	 * @brief 设置滚动模式/Set how the scrollbars should behave.
	 * @param mode      模式 0~关闭 1~开启 2~滚动时显示 3~自动(超出时显示)
	 */
    void setMode(uint8_t mode=3);
    /**
     * @LEVEL1
     * @brief 设置允许滚动的方向/Set the object in which directions can be scrolled
     * @param dir 方向：0~不滚动 1~左 2~右 4~上 8~下及组合 / the allow scroll directions. An element or OR-ed values of `lv_dir_t`
     * 				3~左右 12~上下 15~上下左右
     */
    void setDir(uint8_t dir=15);
    /**
     * @LEVEL2
     * @brief 当水平滚动到头时，对象的对齐方式。/ Set where to snap the children when scrolling ends horizontally
     * @param align  对齐方式：0~不对齐 1~左和上对齐 2~右和下对齐 3~中心对齐 /the snap align to set from `lv_scroll_snap_t`
     */
    void setSnapX(uint8_t align=2);
    /**@LEVEL2
     * @brief 当竖向滚动到头时，对象的对齐方式。/Set where to snap the children when scrolling ends vertically
     * @param align     对齐方式：0~不对齐 1~左和上对齐 2~右和下对齐 3~中心对齐 /the snap align to set from `lv_scroll_snap_t`
     */
    void setSnapY(uint8_t align=2);
    /**
     * @LEVEL0
     * @brief 获取滚动模式/Get the current scroll mode (when to hide the scrollbars)
     * @return          模式 0~关闭 1~开启 2~滚动时显示 3~自动(超出时显示) / the current scroll mode from `lv_scrollbar_mode_t`
     */
    uint8_t getMode();
    /**
     * @LEVEL0
     * @brief 获取允许滚动的方向/Get the object in which directions can be scrolled
     * @return       方向：0~不滚动 1~左 2~右 4~上 8~下及组合 / the allow scroll directions. An element or OR-ed values of `lv_dir_t`
     * 				3~左右 12~上下 15~上下左右
     */
    uint8_t getDir();
    /**
     * @LEVEL2
     * @brief 获取水平滚动到头时，对象的对齐方式。| Get where to snap the children when scrolling ends horizontally
     * @return          对齐方式：0~不对齐 1~左和上对齐 2~右和下对齐 3~中心对齐 /the current snap align from `lv_scroll_snap_t`
     */
    uint8_t getSnapX();
    /**
     * @LEVEL2
     * @brief 获取竖向滚动到头时，对象的对齐方式。| Get where to snap the children when scrolling ends vertically
     * @return          对齐方式：0~不对齐 1~左和上对齐 2~右和下对齐 3~中心对齐 /the current snap align from `lv_scroll_snap_t`
     */
    uint8_t getSnapY();
    /**
     * @LEVEL0
     * @brief 获取滚动条当前x位置
     */
    int16_t getX();
    /**
     * @LEVEL0
     * @brief 获取滚动条当前y位置
     */
    int16_t getY();
    /**
     * @LEVEL1
     * @brief 返回对象上侧区域的宽度。/Return the width of the area on the top the object.
     * 对象可以向上滚动的像素数。/That is the number of pixels the object can be scrolled .
     * 通常为正，但在内部滚动时可能为负。/Normally positive but can be negative when scrolled inside.
     */
    int16_t getTop();
    /**
     * @LEVEL1
     * @brief 返回对象下侧区域的宽度。/Return the width of the area on the bottom the object.
     * 对象可以向下滚动的像素数。/That is the number of pixels the object can be scrolled .
     * 通常为正，但在内部滚动时可能为负。/Normally positive but can be negative when scrolled inside.
     */
    int16_t getBottom();
    /**
     * @LEVEL1
     * @brief 返回对象左侧区域的宽度。/Return the width of the area on the left the object.
     * 对象可以向左滚动的像素数。/That is the number of pixels the object can be scrolled .
     * 通常为正，但在内部滚动时可能为负。/Normally positive but can be negative when scrolled inside.
     */
    int16_t getLeft();
    /**
     * @LEVEL1
     * @brief 返回对象右侧区域的宽度。/Return the width of the area on the right the object.
     * 对象可以向右滚动的像素数。/That is the number of pixels the object can be scrolled .
     * 通常为正，但在内部滚动时可能为负。/Normally positive but can be negative when scrolled inside.
     */
    int16_t getRight();

    /**
     * @LEVEL1
     * @brief 获取正在滚动的终点/get the X and Y coordinates where the scrolling will end for this object if a scrolling animation is in progress.
     * 如没有滚动动画，获取当前滚动位置/If no scrolling animation, give the current `x` or `y` scroll position.
     * @param end       输出终点位置/pointer to store the result
     */
    void getEnd(LvPoint* end);

    /**
     * @LEVEL0
     * @brief 滚动像素量 / Scroll by a given amount of pixels
     * @note   大于0表示向右/下滚动 / > 0 value means scroll right/bottom (show the more content on the right/bottom)
     */
    void scrollBy(int16_t x, int16_t y, uint8_t anim_en);

    /**
     * @LEVEL0
     * @brief 滚动对象到给定的x,y坐标 / Scroll to a given coordinate on an object.
     * @param x,y 仅允许在内容区域上滚动/`x` and `y` will be limited internally to allow scrolling only on the content area.
     */
    void scrollTo(int16_t x, int16_t y, uint8_t anim_en);

    /**
     * @brief 滚动对象到给定的x坐标 / Scroll to a given X coordinate on an object.
     * @param x 仅允许在内容区域上滚动/`x` will be limited internally to allow scrolling only on the content area.
     */
    void scrollToX(int16_t x, uint8_t anim_en);

    /**
     * @LEVEL1
     * @brief 滚动对象到给定的y坐标 / Scroll to a given Y coordinate on an object
     * @param y 仅允许在内容区域上滚动/`y` will be limited internally to allow scrolling only on the content area.
     */
    void scrollToY(int16_t y, uint8_t anim_en);

    /**
     * @LEVEL0
     * @brief 滚动对象，直到它在其父对象上可见 / Scroll to an object until it becomes visible on its parent
     */
    void scrollToView(uint8_t anim_en);

    /**
     * @LEVEL0
     * @brief 递归滚动到一个对象，直到它在其父对象上可见。/ Scroll to an object until it becomes visible on its parent.
     * 父对象的父对象执行相同操作，依此类推。 / Do the same on the parent's parent, and so on.
     * 即使它有嵌套的滚动，对象也能滚动到视图中 / Therefore the object will be scrolled into view even it has nested scrollable parents
     */
    void scrollToViewRecursive(uint8_t anim_en);

    /**
     * @LEVEL1
     * @brief 判断是否正在滚动/Tell whether an object is being scrolled or not at this moment
     */
    bool isScrolling();

    /**
     * @LEVEL2
     * @brief 检查“obj”和滚动“obj’”的子项以实现滚动快照设置/Check the children of `obj` and scroll `obj` to fulfill the scroll_snap settings
     */
    void updateSnap(uint8_t anim_en);

    /**
     * @LEVEL1
     * @brief 获取滚动视图的当前区域/get the area of the scrollbars
     * @param hor   水平滚动条位置 / pointer to store the area of the horizontal scrollbar
     * @param ver   垂直滚动条位置 / pointer to store the area of the vertical  scrollbar
     */
    void getArea(LvArea* hor, LvArea* ver);

    /**
     * @LEVEL0
     * @brief 刷新滚动视图的当前区域/Invalidate the area of the scrollbars
     */
    void invalidate();

    /**
     * @LEVEL0
     * @brief 检查内容是否进入滚动区并调整到正常位置/Checked if the content is scrolled "in" and adjusts it to a normal position.
     * @param anim_en 是否动画/立即显示  0~动画显示 1~立即显示
     */
    void reAdjust(uint8_t anim_en);
};

/**
 * @ingroup style
 * @brief 样式类
 *
 * -# 对lvgl进行了封装，如需详细了解，可参阅lvgl相关文档
 * -# 调用控件的getStyle()方法可得到STTStyle指针
 * -# 样式应用的部件及状态，用或操作表示多个部件及状态 （部件 | 状态）
 * 					部件：
 * 						0x0~矩形背景
 * 						0x010000~滚动条
 * 						0x020000~指示器(slider,arc,bar,switch等)及复选框的框
 * 						0x030000~拖动钮(slider,arc,switch等))
 * 						0x040000~选中项(list,dropdown,roller)
 * 						0x050000~选项(list,dropdown,roller,table)
 * 						0x060000~刻度线
 * 						0x070000~光标(textarea光标、chart光标)
 * 						0x080000~自定义起始值
 * 						0x0F0000~全部)
 * 					状态：
 * 						0x0000~默认状态
 * 						0x0001~checked状态
 * 						0x0002~聚焦状态
 * 						0x0004~通过键盘或编码器聚焦，但不通过触摸板/鼠标聚焦
 * 						0x0008~编辑状态
 * 						0x0010~鼠标悬停
 * 						0x0020~按下状态
 * 						0x0040~滚动状态
 * 						0x0080~禁用状态
 */
class STTStyle {
private:
	/**
	 * @HideCA
	 * @brief 要设置的部件样式及哪个状态，(部件 | 状态
	 * 					部件：
	 * 						0x0~矩形背景
	 * 						0x010000~滚动条
	 * 						0x020000~指示器(slider,arc,bar,switch等)及复选框的框
	 * 						0x030000~拖动钮(slider,arc,switch等))
	 * 						0x040000~选中项(list,dropdown,roller)
	 * 						0x050000~选项(list,dropdown,roller,table)
	 * 						0x060000~刻度线
	 * 						0x070000~光标(textarea光标、chart光标)
	 * 						0x080000~自定义起始值
	 * 						0x0F0000~全部)
	 * 					状态：
	 * 						0x0000~默认状态
	 * 						0x0001~checked状态
	 * 						0x0002~聚焦状态
	 * 						0x0004~通过键盘或编码器聚焦，但不通过触摸板/鼠标聚焦
	 * 						0x0008~编辑状态
	 * 						0x0010~鼠标悬停
	 * 						0x0020~按下状态
	 * 						0x0040~滚动状态
	 * 						0x0080~禁用状态
	 */
	int _selector = 0;
public:
	/**
	 * @HideCA
	 * @param obj	lvgl控件对象
	 */
	STTStyle(void *obj, int selector=0);
	/**
	 * @HideCA
	 */
	virtual ~STTStyle();
	/**
	 * @LEVEL0
	 * @brief 修改圆角半径
	 * @param value 新值
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setRadius(int value, int selector = 0 | 0);
	/**
	 * @LEVEL
	 * @brief 修改宽度
	 * @param value 新值
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setWidth(int value, int selector = 0 | 0);
	/**
	 * @LEVEL2
	 * @brief 是否显示子控件的超出部分
	 * @param value 新值
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setClipCorner(bool value, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 修改不透明度
	 * @param value 新值(0~255)
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setOpa(int value, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 修改混合模式
	 * @param value 新值 0~按不透明度混合 1~增加相应颜色通道 2~从背景中减去前景 3~将背景和前景相乘 4~将背景替换为前景
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setBlendMode(int value, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 修改布局模式
	 * @param value		弹性布局STT_LAYOUT_FLEX / 网格布局STT_LAYOUT_GRID
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setLayout(int value, int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 修改控件的基本方向
	 * @param value 0~左到右 1~右到左 2~自动
	 * @param selector	要设置的哪部分样式，见成员变量_selector
	 */
	STTStyle* setBaseDir(int value, int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 获取圆角半径
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	int getRadius(int selector = 0 | 0);
	/**
	 * @LEVEL2
	 * @brief 获取是否裁剪超出部分
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	bool getClipCorner(int selector = 0 | 0);
	/**
	 * @LEVEL0
	 * @brief 获取不透明度(0~255)
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	uint8_t getOpa(int selector = 0 | 0);
	/**
	 * @LEVEL2
	 * @brief 获取过滤色不透明度(0~255)
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	uint8_t getColorFilterOpa(int selector = 0 | 0);
	/**
	 * @LEVEL2
	 * @brief 设置过滤不透明度
	 * @param 当设为0时，将保持原色。默认一些控件在禁用时设了128,变灰的感觉。此时修改为0，保持原色
	 * @example wMText3->getStyle()->setColorFilterOpa(0, stt::constant::WidgetState::DISABLED);
	 */
	void setColorFilterOpa(uint8_t opa, int selector = 0);
	/**
	 * @LEVEL2
	 * @brief 获取颜色混合模式
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	uint8_t getBlendMode(int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 获取布局
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	uint16_t getLayout(int selector = 0 | 0);
	/**
	 * @LEVEL1
	 * @brief 获取基本方向
	 * @param selector	要获取的哪部分样式，见成员变量_selector
	 */
	uint8_t getBaseDir(int selector = 0 | 0);

	/**
	 * @LEVEL2
	 * @brief 增加控件样式/Add a style to an object.
	 * @param style     指定样式/pointer to a style to add
	 * @param selector  要获取的哪部分样式，见成员变量_selector / OR-ed value of parts and state to which the style should be added
	 */
    STTStyle* add(LvStyle* style, int selector = 0 | 0);
	/**
	 * @LEVEL2
	 * @brief 删除控件样式/remove style from an object.
	 * @param style     指定样式/pointer to a style to remove. Can be NULL to check only the selector
	 * @param selector  要获取的哪部分样式，见成员变量_selector / OR-ed values of states and a part to remove only styles with matching selectors. LV_STATE_ANY and LV_PART_ANY can be used
	 */
    STTStyle* remove(LvStyle* style = NULL, int selector = 0x0F0000 | 0xFFFF);

    void attach(void * obj, int selector=0);
public:
    /**
     * @LEVEL0
     * @brief 背景样式
     */
	STTStyleBg *p_bg;
	/**
	 * @LEVEL0
	 * @brief 边框样式
	 */
	STTStyleBorder *p_border;
	/**
	 * @brief 变换样式
	 */
	STTStyleTransform *p_transform;
	/**
	 * @brief 图片样式
	 */
	STTStyleImg *p_img;
	/**
	 * @brief 图片样式
	 */
	STTStyleOutline *p_outline;
	/**
	 * @LEVEL0
	 * @brief 间隙样式
	 */
	STTStylePad *p_pad;
	/**
	 * @LEVEL0
	 * @brief 阴影样式
	 */
	STTStyleShadow *p_shadow;
	/**
	 * @LEVEL0
	 * @brief 文字样式
	 */
	STTStyleText *p_text;
	/**
	 * @brief 变换样式
	 */
	STTStyleTranslate *p_translate;
	/**
	 * @brief 线型样式
	 */
	STTStyleLine *p_line;
	/**
	 * @brief 圆弧样式
	 */
	STTStyleArc *p_arc;
	/**
	 * @brief 动画样式
	 */
	STTStyleAnim *p_anim;
	/**
	 * @brief 位置样式
	 */
	STTStyleLocation *p_location;
	/**
	 * @brief 尺寸样式
	 */
	STTStyleSize *p_size;
	/**
	 * @brief 滚动条样式
	 */
	STTStylePropScrollbar *p_scrollbar;

private:
	/**
	 * @HideCA
	 */
	void *object;
};
/**
 * @ingroup style
 * @brief 样式助手(等同于STTWidget::getStyle()方法)
 */
class STTStyleHelper {
public:
	/**
	 * @brief 		获取控件对象的样式指针
	 * @param obj	lvgl控件对象或stt控件对象，取决于flag
	 * @param flag	0~stt控件对象，非0~lvgl控件对象
	 */
	static STTStyle* attach(void *obj, int flag = 0);
	/**
	 * @HideCA
	 */
	STTStyle * p_style;
private:
	/**
	 * @HideCA
	 * @brief lvgl控件对象
	 */
	void *object;
};

typedef STTStyle* SttStyle;

/**
 * @brief 布局
 */
class STTLayout{
public:
	/**
	 * @brief 		获取Flex布局
	 */
	static uint16_t FlexLayout();
	/**
	 * @brief 		获取Grid布局
	 */
	static uint16_t GridLayout();
	/**
	 * @brief 		设置Flex布局格式
	 * @param obj	stt控件对象
	 * @param style
	 */
	static void setFlexFlow(void* obj, uint8_t style = 0);
};

}
