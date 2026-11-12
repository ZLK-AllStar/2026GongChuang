/*
 * 普通按钮
 *
 *  Created on: 2023年2月18日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTLabel.h"
#include "widgets/STTWidget.h"

using namespace std;

namespace stt{

/**
 * @ingroup widget
 * @brief 按钮(功能键、元件之按钮型、元件之开关型、元件之指示灯)
 * @image html image/button_main.png
 * 功能键：不使用背景图片\n
 * 按钮型：仅2个状态及对应图片\n
 * 开关型：可设为单状态或多状态，多状态可超过2个状态及图\n
 * 指示灯：可设为单状态或多状态，多状态可超过2个状态及图\n
 */
class STTButton: public STTWidget {
public:
	/**
	 * @HideCA 按钮构造
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 * @param type	按钮类型：0~ 功能键 1~按钮元件 2~开关元件 3~指示灯元件
	 */
	STTButton(IRuntime* rt, STTWidget* par, const string& id="", uint16_t type=0);
	/**
	 * @HideCA
	 * @brief 弧形指示条/进度条析构
	 */
	virtual ~STTButton();
    /**overload*/
    STTWidget* setPos(int x, int y);
    /**
     * @LEVEL2
     * @brief				设置基本参数
     * @param x 			X坐标(默认0)
     * @param y 			Y坐标(默认0)
     * @param w 			宽度(默认100)
     * @param h 			高度(默认50)
     * @param isbeep 		是否打开蜂鸣器
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=50, bool isbeep=false);
    /**
     * @LEVEL2
     * @brief				设置松开时文字
     * @param title 		文字内容(默认Button)
     * @param c_title 		文字颜色(默认0x0)
     * @param fontname 		文字字体名称(默认"")
     * @param fontsize 		文字字体大小(默认16)
     */
    void setReleasedArg(const string& title="Button", int c_title=0x0, const string& fontname="", int fontsize=16, int fontstyle=0);
    /**
     * @LEVEL2
     * @brief				设置松开时样式
     * @param r 			圆角半径(默认8)
     * @param opa 			背景透明度(默认255)
     * @param c1 			背景色：主色(默认0xffffff)
     * @param c2 			背景色：次色（渐变）(默认0xffffff)
     * @param dir 			渐变方向(默认0不渐变), 1~上下 2~左右
     * @param w_bord 		边框宽(默认2)
     * @param c_bord 		边框色(默认0x1a2b3)
     * @param w_shadow 		阴影宽(默认0)
     * @param c_shadow 		阴影色(默认0x0)
     * @param img 			背景图(默认"")
     */
    void setReleasedStyle(int r = 8, int opa = 255, int c1 = 0xffffff, int c2 = 0xffffff, int dir = 0, int w_bord = 2, int c_bord = 0x1a2b3, int w_shadow=0,
        int c_shadow=0x0, const string& img = "");
    /**
     * @LEVEL2
     * @brief			设置按下时文字
     * @param title 	文字内容(默认Button)
     * @param c_title 	文字颜色(默认0x0)
     * @param fontname 	文字字体名称(默认"")
     * @param fontsize 	文字字体大小(默认16)
     */
    void setPressedArg(const string& title="Button", int c_title=0x0, const string& fontname="", int fontsize=16, int fontstyle=0);
    /**
     * @LEVEL2
     * @brief			设置按下时样式
     * @param r 		圆角半径(默认8)
     * @param opa 		背景透明度(默认255)
     * @param c1 		背景色：主色(默认0xffffff)
     * @param c2 		背景色：次色（渐变）(默认0xffffff)
     * @param dir 		渐变方向(默认0不渐变), 1~上下 2~左右
     * @param w_bord 	边框宽(默认2)
     * @param c_bord 	边框色(默认0x1a2b3)
     * @param w_shadow 	阴影宽(默认0)
     * @param c_shadow 	阴影色(默认0x0)
     * @param img 		背景图(默认"")
     */
    void setPressedStyle(int r = 8, int opa = 255, int c1 = 0xffffff, int c2 = 0xffffff, int dir = 0, int w_bord = 2, int c_bord = 0x1a2b3, int w_shadow=0, int c_shadow=0x0, const string & img = "");
    /**
     * @LEVEL1
     * @brief 			设定状态及其对应的图片路径
     * @param state 	自定义的状态值
     * @param path  	状态对应的图片路径(图片对[功能键]无效)
     * @param title		状态对应的文本，文字太长可能导致图片不显示
     */
    void setImg(const string& state, const string& path, const string & title = "");
    /**
     * @LEVEL1
     * @brief 			获取指定状态的图片路径
     * 		没有相应状态时，会返回空串
     * @param state 	状态值
     */
    string getImg(const string& state);
    /**
     * @LEVEL0
     * @brief 设置状态及对应标题
     * @param state 状态值
     * @param title 状态标题
     */
    void setTitle(const string& state, const string & title);
    /**
     * @LEVEL0
     * @brief			获取状态对应的标题
     * @param state		状态
     * @return
     */
    string getTitle(const string& state);
    /**
     * @LEVEL0
     * @brief 			设为指定状态，显示相应的图片
     * @param state 	指定状态值。如未添加状态，则无效。
     */
    void setState(const string& state);
    /**
     * @LEVEL0
     * @brief 获取当前显示图片对应的状态
     */
    string getState();
    /**
     * @LEVEL0
     * @brief 是否包含状态(注意与hasState方法的区别)
     */
    bool hasStateString(const string& str);
    /**
     * @LEVEL0
     * @brief 获取状态数量
     */
    int getStateCount();
    /**
     * @LEVEL0
     * @brief 切换到下一个状态
     */
    void switchNextState();
    /**
     * @LEVEL1
     * @brief 启用状态标签
     */
    void enStateLabel();
    /**
     * @LEVEL2
     * @brief			获取标签
     */
    SttLabel getStateLabel();
    /**
     * @LEVEL2
     * @brief 变量操作
     * @param e			事件。见stt::constant::EVENT
     * @param var		变量。
     * @param options	操作参数。json格式，key:value：
     * 					"op":"stt::constant::SIT_DBOBJ::",
     * 					"period":"周期ms",
     * 					"initvalue":"初始值",
     * 					"setvalue":"设为常数值",
     * 					"step":"递增步长",
     * 					"lowerlimit":"下限",
     * 					"uplimit":"上限",
     */
	int opDataObj(uint8_t e, IVariable * var, const string& options);
	/**
	 * @LEVEL2
	 * @brief		将事件~打开窗口加入列表
	 * @param evt 	事件，见stt::constant::Event
	 * @param id	窗口id，如"Frm1"
	 * @param mode	窗口的打开模式，见stt::constant::PageMode
	 * @param target	如果是嵌入模式，指定目标控件
	 */
	void bindOpenFormHandler(int evt, string id, uint8_t mode = PageMode::NEWWIN, STTWidget* target=nullptr);
	/**
	 * @LEVEL2
	 * @brief		将事件~关闭窗口加入列表
	 * @param id	窗口id，如"Frm1"
	 */
	void bindCloseFormHandler(int evt, string id);
    /**
     * @LEVEL0
     * @brief		确认操作
     */
    void onConfirm(uint16_t code, LvEvent e);
    /**
     * @LEVEL0
     * @brief		登录认证
     */
    void onAuth(uint16_t code, LvEvent e);
    /**
     * @HideCA
     * @brief 刷新
     */
    void refresh();

public:
    /**
     * @HideCA
     * @brief 内部指针
     **/
    void* mBtn;
    /**
     * @HideCA
     * @brief 设置按钮按下时的宽高变化量，正值为缩小，负值为放大
     * @param xrate 宽度变化量，如设为20，表示变化量为宽度的1/20
     * @param yrate 高度变化量，如设为20，表示变化量为高度的1/20
     * @note 对功能键有效，对元件按钮无效
     */
	void setTransform(int xrate=20,int yrate=20);

private:

    /**
     * @HideCA
     * @brief 内部函数
     **/
    void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 内部函数
     **/
    STTButton* setText(const string& str);
};

typedef STTButton* SttButton;

}
