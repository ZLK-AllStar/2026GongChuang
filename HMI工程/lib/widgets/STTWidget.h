/*
 * STTWidget.h
 *
 *  Created on: 2023年2月14日
 *      Author: SIMTUTAI
 */
#pragma once

#include "stdint.h"
#include <iostream>
#include <vector>
#include "widgets/STTStyle.h"
#include "core/SttObject.h"
#include "base/Interface.h"
#include "util/Util.h"
#include "op/DataSample.h"

using namespace std;
using namespace stt::util;
using namespace stt::constant;

namespace stt{

/**
 * @HideCA 此标志表示不显示在代码助手中
 * 定义与打开窗口pm->open方法相应的类型，便于写代码。参数1~窗口id(如"Frm1"), 参数2~窗口模式(如PageMode::EMBEDDED), 参数3~嵌入模式下的容器对象
 */
typedef std::function<void(string, uint8_t, STTWidget*)> FuncOpenForm;
/**
 * @HideCA
 * 定义与打开窗口pm->close方法相应的类型，便于写代码。参数1~窗口id(如"Frm1")
 */
typedef std::function<void(string)> FuncCloseForm;

/**
 * @ingroup widget
 * @brief 控件基类
 */
class STTWidget : public SttObject {
public:
	/**
	 * @HideCA
	 */
    STTWidget(IRuntime* rt, STTWidget* par, const string& id);
    /**
     * @HideCA
     */
    virtual ~STTWidget();
    /**
     * @HideCA
     * @brief 设置基本参数
     * @param x		x坐标
     * @param y		y坐标
     * @param w		宽
     * @param h		高
     */
    void setMainArg(int x, int y, int w, int h);
    /**
     * @HideCA
     * @brief 设置基本样式
     * @param r			圆角半径。
     * @param c1		主色(渐变起始色)
     * @param c2		渐变终止色。
     * @param dir		渐变方向。0~不渐变 1~上下 2~左右
     * @param opa		不透明度。0~255
     * @param w_border	边框线粗。
     * @param c_border	边框线色
     * @param w_shadow	阴影宽度。
     * @param c_shadow	阴影颜色。
     * @param ss		部件、状态选择器，见STTStyle::_selector
     */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=0, int w_border=0, int c_border=0x0, int w_shadow=0, int c_shadow=0x0, int ss = 0);
    /**
     * @HideCA
     * @brief 设置文本样式
     * @param c			文本颜色
     * @param size		字体大小。
     * @param fontname	字体名称。为NULL时，设为默认字体
     * @param ss		部件、状态选择器，见STTStyle::_selector
     */
    void setFontStyle(int c=0x0, int size=16, const char* fontname=NULL, int ss = 0, int fontstyle=0);
    /**
     * @LEVEL0
     * @brief 获取样式指针
     */
    STTStyle* getStyle();
    /**
     * @LEVEL1
     * @brief 	设置主背景图片(效果同this->getStyle()->p_bg->setImgSrc(path, part)->setOpa(0, part))
     * @param path		图片路径
     * @param part		见stt::constant::Part
     */
    STTWidget* setBgImg(const char* path, int part = stt::constant::Part::Main);
    /**
     * @LEVEL1
     * @brief 	刷新控件显示(绑定变量及动画，而不是刷新控件本身)
     */
    virtual void refresh();
    /**
     * @LEVEL1
     * @brief 	刷新控件及其子控件显示(绑定变量及动画，而不是刷新控件本身)
     */
    virtual void refreshAll();
    /**
     * @HideCA
     * @brief 指定控件的事件处理函数
     * @param code		见stt::constant::Event, 如ALL/PRESSED/PRESSING/PRESS_LOST/SHORT_CLICKED/LONG_PRESSED/LONG_PRESSED_REPEAT/CLICKED......
     * @param object	控件
     * @param handler	处理函数function (uint16_t code, LvEvent e)
     */
    void onEventHandler(uint16_t code, SttObject* object, EHandler handler);
    /**
     * @HideCA
     * @brief 绑定变量
     * 					文字控件 <-> 文字内容
     * 					文本框控件 <-> 文字内容
     * 					LED控件 <-> 亮灭
     * 					进度条控件 <-> 进度值
     * 					下拉框控件 <-> 选中项索引
     * 					仪表盘	<-> 指针值
     * 					指针    <-> 指针值
     * 					刻度盘   <-> 刻度值
     * @param data	变量 vm->get("变量名称")
     */
    virtual void bindData(IVariable * data);
    /**
	 * @HideCA
	 * @brief 解绑变量
	 */
    virtual void disBindData();
    /**
     * @HideCA
     * @brief 绑定变量
     * @param data 	使用共享指针的变量
     */
    virtual void bindData(shared_ptr<IVariable> data);
    /**
     * @HideCA
     * @brief 绑定的变量变化时，响应函数
     * @param org	控件
     * @param data	变量 vm->get("变量名称")
     */
    virtual void onDataChanged(SttObject * org, IVariable* data);
public:/*lv_obj.h*/
    /**
     * @LEVEL1
     * @brief 	增加标志
     * 			Set one or more flags
     * @param f     见[常量FLAG]，可或操作组合
     * 				OR-ed values from `stt::constant::FLAG` to set.
     */
    STTWidget* addFlag(uint32_t f);
    /**
     * @LEVEL1
     * @brief 	清除标志
     * 			Clear one or more flags
     * @param f     见[常量FLAG]，可或操作组合
     * 				OR-ed values from `stt::constant::FLAG` to set.
     */
    STTWidget* clearFlag(uint32_t f);
    /**
     * @LEVEL0
     * @brief 隐藏/显示控件
     * @param en true~隐藏 false~显示
     */
    virtual STTWidget* setHidden(bool en);
    /**
     * @LEVEL1
     * @brief 禁止/使能控件
     * @param en true~使能 false~禁止
     */
    STTWidget* setEnable(bool en);
    /**
     * @LEVEL1
     * @brief 控件是否可点击
     * @param en true~可 false~不可
     */
    STTWidget* setClickable(bool en);
    /**
     * @LEVEL1
     * @brief 控件是否可滚动
     * @param en true~是 false~否
     */
    STTWidget* setScrollable(bool en);
    /**
     * @LEVEL1
     * @brief 添加控件状态（addState后，当前状态就变了，相当于同时setState）
     * @param state	取值范围stt::constant::WidgetState。用或操作组合
     */
    virtual STTWidget* addState(int state);
    /**
     * @LEVEL1
	 * @brief 取消控件状态
	 * @param state	取值范围stt::constant::WidgetState。用或操作组合
	 */
    virtual STTWidget* clearState(int state);
    /**
     * @LEVEL1
	 * @brief 获取控件状态。
	 * @return 取值范围stt::constant::WidgetState。用或操作组合
	 */
    int getState();
    /**
     * @LEVEL1
	 * @brief 是否添加了指定状态
	 * @param state	取值范围stt::constant::WidgetState。用或操作组合
	 */
    bool hasState(int state);
    /**
     * @LEVEL1
     * @brief 传入用户数据
     * @param user_data 	指向用户数据的指针
     */
    STTWidget* setUserData(void* user_data);
    /**
     * @LEVEL1
     * @brief 是否添加了指定全部标志
     * @param f		标志。见stt:constant::FLAG
     */
    bool hasFlag(uint32_t f);
    /**
     * @LEVEL1
     * @brief 获取控件所在组(一般用于外部键盘、编码器)  |  Get the group of the object
     */
    void* getGroup();
    /**
     * @brief 获取控件用户数据 | Get the user_data field of the object
     */
    void* getUserData();
    /**
     * @brief 判断控件是否还存在
     */
    bool isValid();

public: /*lv_obj_pos*/
    /**
     * @LEVEL0
     * @brief 	设置控件位置
     * @param x		X坐标
     * @param y		Y坐标
     */
    STTWidget* setPos(int x, int y);
    /**
     * @LEVEL0
     * @brief 	设置控件位置
     * @param x		X坐标
     */
    STTWidget* setX(int x);
    /**
     * @LEVEL0
     * @brief 	设置控件位置
     * @param y		Y坐标
     */
    STTWidget* setY(int y);
    /**
     * @LEVEL1
     * @brief 	设置控件尺寸
     * @param w		宽
     * @param h		高
     */
    virtual STTWidget* setSize(int w, int h);
    /**
     * @LEVEL0
     * @brief 	设置控件尺寸
     * @param w		宽
     */
    virtual STTWidget* setWidth(int w);
    /**
     * @LEVEL0
     * @brief 	设置控件尺寸
     * @param h		高
     */
    virtual STTWidget* setHeight(int h);
    /**
     * @LEVEL1
     * @brief 	设置控件内容尺寸(会同时导致总宽度(=border*2+padleft+padright)变化)
     * @param w		宽
     */
    STTWidget* setContentWidth(int w);
    /**
     * @LEVEL1
     * @brief 	设置控件内容尺寸(会同时导致总高度(=border*2+padtop+padbottom)变化)
     * @param h		高
     */
    STTWidget* setContentHeight(int h);
    /**
     * @LEVEL1
     * @brief 设置控件布局方式
     * @param layout 布局方式。目前有：弹性布局~STT_LAYOUT_FLEX  网格布局~STT_LAYOUT_GRID
     */
    STTWidget* setLayout(unsigned layout);
    /**
     * @LEVEL1
     * @brief 标记控件需要被布局更新
     */
    STTWidget* markLayoutAsDirty();
    /**
     * @LEVEL1
     * @brief 更新布局
     */
    STTWidget* updateLayout();
    /**
     * @LEVEL1
     * @brief 设置与父控件对齐
     * @param align 见stt::constant::ALIGN
     */
    STTWidget* setAlign(int align);
    /**
     * @LEVEL1
     * @brief 设置与父控件对齐及偏移量
     * @param align 见stt::constant::ALIGN
     * @param x_ofs	水平偏移量
     * @param y_ofs 垂直偏移量
     */
    STTWidget* align(int align, int x_ofs=0, int y_ofs=0);
    /**
     * @LEVEL1
     * @brief 与指定控件对齐及偏移
     * @param base	指定要基于哪个控件对齐(这个控件位置不动)
     * @param align 见stt::constant::ALIGN
     * @param x_ofs	水平偏移量
     * @param y_ofs 垂直偏移量
     */
    STTWidget* alignTo(STTWidget* base, int align, int x_ofs=0, int y_ofs=0);
    /**
     * @LEVEL1
     * @brief 位于父控件中心
     */
    STTWidget* center();
    /**
     * @LEVEL1
     * @brief 获取控件位置（绝对坐标）
     * @param coords	x1~左 x2~右 y1~上 y2~下
     */
    void getCoords(LvArea* coords);
    /**
     * @LEVEL0
     * @brief 获取控件左边的x相对坐标
     */
    int getX();
    /**
     * @LEVEL0
     * @brief 获取控件上边的y相对坐标
     */
    int getY();
    /**
     * @LEVEL0
     * @brief 获取控件宽度
     */
    int getWidth();
    /**
     * @LEVEL0
     * @brief 获取控件高度
     */
    int getHeight();
    /**
     * @LEVEL1
     * @brief 获取控件的内容宽度(控件宽 - 边框宽border - 左右边距pad)
     */
    int getContentWidth();
    /**
     * @LEVEL1
     * @brief 获取控件的内容高度(控件高 - 边框宽border - 上下边距pad)
     */
    int getContentHeight();
    /**
     * @LEVEL1
     * @brief 移动控件到指定位置。如超出父控件范围，将自动滚动
     * @param x 目标x坐标
     * @param y 目标y坐标
     */
    STTWidget* moveTo(int x, int y);
    /**
     * @LEVEL1
     * @brief 重绘整个控件
     */
    STTWidget* invalidate();
    /**
     * @LEVEL1
     * @brief 判断控件是否可见(部分可见，也是可见:隐藏、在屏幕外、在父控件外，只要是看不见均为false)
     */
    bool isVisible();
    /**
     * @LEVEL1
     * @brief 控件的每个方向都扩展可点击区域
     * @param size	各方向扩展的像素数
     */
    STTWidget* setExtClickArea(int size);
public:
    /**
     * @LEVEL1
     * @brief 删除控件及其子控件
     */
    STTWidget* del();
    /**
     * @LEVEL1
     * @brief 删除所有子控件
     */
    STTWidget* clean();
    /**
     * @LEVEL1
     * @brief 将控件的顺序移动到指定序号.序号从0开始
     * @param index	序号
     */
    STTWidget* moveToIndex(int32_t index);
    /**
     * @LEVEL1
     * @brief 获取控件的屏幕对象
     */
    void* getScreen();
    /**
     * @LEVEL1
     * @brief 获取控件的显示对象(display)
     */
    void* getDisp();
    /**
     * @LEVEL1
     * @brief 获取父控件lvgl对象
     */
    void* getParent();
    /**
     * @LEVEL2
     * @brief 设置父控件
     * @param lvobj 为lvgl控件对像
     */
    void setParent(void* lvobj);
    /**
     * @LEVEL1
     * @brief 获取父控件
     */
    STTWidget* getParentWidget();
    /**
     * @LEVEL2
     * @brief 设置父控件
     * @param sttobj 为stt控件对像
     */
    void setParentWidget(STTWidget* sttobj);
    /**
     * @LEVEL1
     * @brief 获取控件的第几个子控件
     * @param id 索引值,从0开始
     * @return
     */
    STTWidget* getChildWidget(int id);
    /**
     * @LEVEL1
     * @brief 获取子控件个数
     */
    int getChildCnt();
    /**
     * @LEVEL1
     * @brief 返回当前子控件的(child index)序号。从0开始
     */
    int getIndex();
    /**
     * @LEVEL1
     * @brief 向控件发送消息
     * @param eventCode	消息代码, 见stt::constant::Event
     * @param param		消息参数(根据控件类型及事件类型确定，一般点击事件传入NULL即可)
     */
    void sendEvent(uint16_t eventCode, void * param);
    /**
     * @HideCA
     * @brief 获取lvgl根控件。控件的父为屏幕时，即为根控件。
     */
    void* getRootObject();
    /**
     * @LEVEL1
     * @brief 取根控件
     */
    STTWidget* getRootWidget();
    /**
     * @LEVEL1
     * @brief 获取控件类型
     * 见[常量WidgetType]
     */
    int widgetType();
public:
	/**
	 * @LEVEL2
	 * @brief 控件动画
	 * @param var		变量。
	 * @param options	操作参数。json格式, key:value
	 * 					文字内容{t:0,p:[{k:1,v:大小}, {k:2,v:颜色}]}
	 * 					前景色{t:1,p:[{k:1,v:0xff0000},{k:2,v:0x00ff00}]}
	 * 					背景色{t:2,p:[{k:1,v:0xff0000},{k:2,v:0x00ff00}]}
	 * 					边框色{t:3,p:[{k:1,v:0xff0000},{k:2,v:0x00ff00}]}
	 * 					水平移动{t:4,p:{ml:1,vl:2,mu:100,vu:200}}
	 * 					垂直移动{t:5,p:{ml:1,vl:2,mu:100,vu:200}}
	 * 					尺寸变化{t:5,p:{ml:1,vl:2,mu:100,vu:200}}
	 */
	int opAnimObj(IVariable * var, const string& options);
	/**
	 * @LEVEL2
	 * @brief 控件的可见控制
	 * @param options json格式 {"c":0}
	 * 					c:条件，0可见，1不可见
	 */
	int opVisibleObj(IVariable * var, const string& options);
	/**
	 * @LEVEL2
	 * @brief 控件的使能/禁用控制
	 * @param options json格式 {"c":0}
	 * 					c:条件，0-使能，1-禁用
	 */
	int opEnableObj(IVariable * var, const string& options);
	/**
	 * @LEVEL2
	 * @brief 控件闪烁
	 * @param var		变量。
	 * @param options	操作参数。json格式{"m":0, "p":[], "c":0,"s":1}
	 * 					"m":"闪烁方式：0~隐藏/显示方式 1~通过属性"
	 *					"p":"[k:v,k:v...](m==1时，属性控制)"
	 *					"c":"闪烁条件(0~不闪 非0~闪)"
	 *					"s":速度  0:慢 1:适中 2:快
	 */
	int opFlickObj(IVariable * var, const string& options);
	/**
	 * @LEVEL1
	 * @brief 启用/关闭权限确认
	 * @param b	（无用，默认就是TRUE）
	 * @param auths	权限分组，多个组用","分隔
	 */
	void setAuth(bool b, const string& auths);
	/**
	 * @LEVEL1
	 * @brief 授权
	 * @param b	是/否
	 */
	void authorize(bool b);
	/**
	 * @LEVEL1
	 * @brief 是否授权
	 * @return 是/否
	 */
	bool isAuthorized();
	/**
	 * @LEVEL1
	 * @brief 启用/关闭提示音
	 * @param b 启用/关闭
	 */
	void setBeep(bool b);
	/**
	 * @LEVEL1
	 * @brief 启用/关闭确认对话框
	 * @param b	启用/关闭
	 * @param s	确认对话框信息
	 */
	void setConfirm(bool b, const string& s);
	/**
	 * @LEVEL1
	 * @brief 启用/关闭操作日志
	 * @param b 启用/关闭
	 */
	void setOpLog(bool b);
    /**
     * @HideCA
     * @brief 获取名称大小一致的字体指针。
     * @param size		字体大小。
     * @param fontname 	字体名称。为""时或者没找到相应的字体，则取默认字体
     * @param fontstyle 0~正常 1~斜体 2~粗体 3~斜体加粗体
     */
    void* getFont(int size, const string& fontname="", int fontstyle=0);
    /**
     * @LEVEL1
     * @brief 旋转scr(弃用，控件操作方向不正确，请使用rotateDisp)
     * @param angle 取0, 90, 180, 270
     */
    static void rotateScreen(int angle,int pivotx, int pivoty);
    /**
     * @LEVEL1
     * @brief 旋转disp
     * @param angle 取0, 90, 180, 270
     */
    static void rotateDisp(int angle);
    /**
     * @HideCA
     * @brief 获取绝对坐标位置(通过lvgl加上父控件位置方法)
     * @param x [out] 返回x坐标
     * @param y [out] 返回y坐标
     * @param lvobj 指向 lvgl控件的指针(lv_obj_t*)
     */
    void getAbsPosition(int& x, int& y, void* lvobj);
    /**
     * @HideCA
     * @brief 获取绝对坐标x1,y1,x2,y2(不是宽度，不是高度，是坐标).这个方法要在控件显示后才能正常获取。
     * @note 通过lv_obj_get_coords方法获取，同getCoords
     */
    LvArea getAbsPosition();
    /**
     * @HideCA
     * @brief 通过控件的msettedx,y,w,h获取绝对坐标
     * @param x 要转换的相对坐标
     * @param y 要转换的相对坐标
     * @return 转换后的绝对坐标
     */
    LvArea getAbsPosition(int x,int y);
    /**
     * @LEVEL1
     * @brief 获取控件ID(控件名)
     */
    string getId();
    /**
     * @LEVEL1
     * @brief 获取控件截图，生成png图片
     * @param path 文件路径(后缀名可为png或jpg)
     * @param isBGR	是否互换rb
     */
    bool snapshotToPng(const string& path, bool isBGR = false);
    /**
     * @LEVEL1
     * @brief 设置控件对应外部按钮的键值(1~255)
     * @note 注意启用扩展输入
     */
    void setButtonerCode(uint8_t i);
    /**
     * @LEVEL1
     * @brief 获取控件对应外部按钮的键值(1~255)
     * @note 注意启用扩展输入
     */
    uint8_t getButtonerCode();
    /**
     * @LEVEL1
     * @brief 启用拖拽
     */
    virtual void enableDrag(bool en=true);
    /**
     * @LEVEL1
     * @brief 增加事件回调
     * @param func_cb 回调函数
     * @param 触发事件的编号，见stt::constant::Event(回调中用e->code获取)
     * @param userData 传入回调的数据(回调中用e->target->user_data获取)
     * @example
     * 1.定义回调函数
     * void myfunc(LvEvent * e) {	cout<<"myfunc"<<endl; }
     * 2.给文本框增加长按事件回调
     * wMText3->addEventCB(myfunc, stt::constant::Event::LONG_PRESSED, wMText3);
     */
    void addEventCB(void (*func_cb)(LvEvent * e),int eventCode,void* userData);
    /**
     * @LEVEL1
     * @brief 移除事件回调
     * @param func_cb 回调函数
     * @example 移除长按回调 wMText3->removeEventCB(myfunc)
     */
    void removeEventCB(void (*func_cb)(LvEvent * e));
    /**
     * @HideCA
     * @brief 为字体设置用户数据
     * @param lvfont 通过widget->getStyle()->...获得到的字体,如const_cast<void*>(pp->getStyle()->p_text->getFont()
     * @param userdata 已有功能：当*userdata=1时，文字锐化，用于去掉视频叠加文字去掉锯齿。故不要与其他控件共用字体。
     */
    void setFontUserData(void* lvfont, void* userdata);
    /**
     * @HideCA
     * @brief 取得字体中的userdata
     */
    void* getFontUserData(void* lvfont);
public:
    /**@HideCA*/
    void* mWidget;
    /**@HideCA*/
    void* object;

    /**@HideCA*/
    int16_t mSettedwidth;
    /**@HideCA*/
    int16_t mSettedheight;
    /**@HideCA*/
    int16_t mSettedX;
    /**@HideCA*/
    int16_t mSettedY;
    /**是否启用控件池，默认为否*/
    static bool enWidgetPool;
};
typedef STTWidget* SttWidget;

/**
 * 用法
 * 1.add
 * 2.apply
 * 3.draw
 * 4.clear
 */
class STTMask {
public:
	STTMask();
	virtual ~STTMask();

	/**
	 * @brief 增加角度型蒙板
	 * @param wid 控件
	 * @param x,y 角的顶点位置(相对坐标)
	 * @param angle1 角的起始角度(向右为0度，顺时针为正)
	 * @param angle2 角的终止角度(向右为0度，顺时针为正)
	 */
	static void addAngle(STTWidget* wid, int x, int y, int angle1, int angle2);
	/**
	 * @brief 初始化渐变型蒙版（从上到下）
	 * @param wid 控件
	 * @param x,y,w,h蒙版区域(相对坐标)
	 * @param opa1,y1 上端开始位置的不透明度(相对坐标)
	 * @param opa2,y2 下端开始位置的不透明度(相对坐标)
	 */
	static void addFade(STTWidget* wid,int x, int y, int w, int h, int opa1, int y1, int opa2, int y2);
	/**
	 * @brief 用点角法初始化线蒙版（取线的一侧）
	 * @param wid 控件
	 * @param x,y 线经过的点(相对坐标)
	 * @param angle 线的角度0~360(向右为0度，顺时针为正)
	 * @param side 0~左 1~右 2~上 3~下
	 * @note 垂直(水平)线不存在上下(左右)，设为上下(左右)时蒙版失效
	 */
	static void addLine1(STTWidget* wid,int x, int y,int angle,int side);
	/**
	 * @brief 两点法线蒙版（取线的一侧）
	 * @param wid 控件
	 * @param x1,y1,x2,y2 线经过的两点(相对坐标)
	 * @param side 0~左 1~右 2~上 3~下
	 * @note 垂直(水平)线不存在上下(左右)，设为上下(左右)时蒙版失效
	 */
	static void addLine2(STTWidget* wid,int x1, int y1,int x2, int y2,int side);
	/**
	 * @brief 点阵蒙版
	 * @param wid 控件
	 * @param x,y,w,h 蒙版区域(相对坐标)
	 * @param opamap 各点的透明度
	 */
	static void addMap(STTWidget* wid, int x, int y, int w, int h, uint8_t* opamap);
	/**
	 * @brief 初始化多边型蒙版
	 * @param wid 控件
	 * @param points 多边型顶点(相对坐标)
	 */
	static void addPolygon(STTWidget* wid,vector<LvPoint>& points);
	/**
	 * @brief 初始化圆角区域蒙版
	 * @param wid 控件
	 * @param x,y,w,h 区域(相对坐标)
	 * @param radius 圆角半径
	 * @param inv 是则画内侧，否则画外侧
	 */
	static void addRadius(STTWidget* wid, int x, int y, int w, int h, int radius, bool inv);
	/**
	 * @brief 应用蒙板，一般在LV_EVENT_DRAW_MAIN_BEGIN事件中调用
	 */
	static void apply();
	/**
	 * @brief 清除蒙板，一般在LV_EVENT_DRAW_MAIN_END事件中调用
	 */
	static void clear();
};
}

