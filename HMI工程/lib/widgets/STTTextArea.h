/*
 * STTTextArea.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 文本框、文本区
 */
class STTTextArea: public STTWidget {
public:
    /**
     * @HideCA
     * @brief 文本输入框构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
     */
	STTTextArea(IRuntime* rt, STTWidget* par, const string& id="");
	/**
	 * @HideCA
	 */
	virtual ~STTTextArea();
     /**
      * @LEVEL0
      * @brief 插入文本
      * @param txt	字符串
      */
     STTTextArea* addText(const string& txt);
     /**
      * @LEVEL0
      * @brief 删除光标左侧一个字符(按下Backspace键效果)
      */
     STTTextArea* delChar();
     /**
      * @LEVEL0
      * @brief 删除光标右侧一个字符(按下Delete键效果)
      */
     STTTextArea* delCharForward();
     /**
      * @LEVEL0
      * @brief 设置文本
      * @param txt		字符串
      */
     STTTextArea* setText(const string& txt);
     /**
      * @LEVEL0
      * @brief 直接设置为文本（即使设置了maxLen和AcceptChars也不检查是否符合规则）
      * @param txt		字符串
      */
     STTTextArea* setTextValue(const string& txt);
     /**
      * @LEVEL1
      * @brief 设置提示文本。即在文本内容为空时，显示的提示内容
      * @param txt	字符串
      */
     STTTextArea* setPlaceholderText(const string& txt);
     /**
      * @LEVEL1
      * @brief 设置光标位置
      * @param pos	光标位置，从0开始
      */
     STTTextArea* setCursorPos(int pos);
     /**
      * @LEVEL1
      * @brief 启用/停止通过点击来改变光标位置
      * @param en	true~启用 false~停止
      */
     STTTextArea* setCursorClickPos(bool en);
     /**
      * @LEVEL0
      * @brief 启用/停止密码模式
      * @param en	true~启用 false~停止
      */
     STTTextArea* setPasswordMode(bool en);
     /**
      * @LEVEL0
      * @brief 启用/停止单行模式
      * @param en	true~启用 false~停止
      */
     STTTextArea* setOneLine(bool en);
     /**
      * @LEVEL0
      * @brief 可输入的字符列表。""表示全部可输入
      * @param chars 字符列表 A-Z a-z 0-9 ~!@#$%^&*()_+-=:";'<>,.?/|\~`
      */
     STTTextArea* setAcceptedChars(const string& chars);
     /**
      * @LEVEL0
      * @brief 设置文本最大长度。
      * @param num 	长度 0~不限制
      */
     STTTextArea* setMaxLength(int num);
     /**
      * @LEVEL1
      * @brief 启用/停止文本选择
      * @param en	true~启用 false~停止
      */
     STTTextArea* setTextSelection(bool en);
     /**
      * @LEVEL0
      * @brief 设置文本对齐
      * @param align	对齐方式：0~自动，1~左，2~中，3~右
      */
     STTTextArea* setAlign(int align);
     /**
      * @LEVEL0
      * @brief 获取文本框文本
      */
     string getText();
     /**
      * @LEVEL1
      * @brief 获取提示文本
      */
     string getPlaceholderText();
     /**
      * @LEVEL1
      * @brief 获取光标位置(字符的索引位置，从0开始)
      */
     int getCursorPos();
     /**
      * @LEVEL1
      * @brief 是否开启通过点击改变光标位置
      */
     bool getCursorClickPos();
     /**
      * @LEVEL0
      * @brief 获取是否开启密码模式
      */
     bool getPasswordMode();
     /**
      * @LEVEL0
      * @brief 获取是否单行模式
      */
     bool getOneLine();
     /**
      * @LEVEL0
      * @brief 获取可输入的字符
      */
     string getAcceptedChars();
     /**
      * @LEVEL0
      * @brief 获取可输入的最大长度
      */
     int getMaxLength();
     /**
      * @brief 是否有文本被选择
      */
     bool textIsSelected();
     /**
      * @LEVEL1
      * @brief 是否开启选择文本
      */
     bool getTextSelection();
     /**
      * @LEVEL1
      * @brief 取消文本选择
      */
     STTTextArea* clearSelection();
     /**
      * @LEVEL0
      * @brief 右移光标
      */
     STTTextArea* cursorRight();
     /**
      * @LEVEL0
      * @brief 左移光标
      */
     STTTextArea* cursorLeft();
     /**
      * @LEVEL0
      * @brief 下移光标
      */
     STTTextArea* cursorDown();
     /**
      * @LEVEL0
      * @brief 上移光标
      */
     STTTextArea* cursorUp();
public:
     /**
      * @LEVEL2
      * @brief 获取整数部分最大可输入长度
      */
     int8_t getIntLen();
     /**
      * @LEVEL2
      * @brief 设置整数部分最大可输入长度
      */
     STTTextArea* setIntLen(uint8_t intLen = 11);
     /**
      * @LEVEL2
      * @brief 获取小数部分最大可输入长度
      */
     int8_t getFracLen();
     /**
      * @LEVEL2
      * @brief 设置小数部分最大可输入长度
      */
     STTTextArea* setFracLen(int8_t fracLen = 0);
     /**
      * @LEVEL2
      * @brief 获取换算率参数
      */
     string getExchRate();
     /**
      * @LEVEL2
      * @brief 设置换算率参数(当绑定的变量值变化时，显示在界面上的是经过换算的值)
      * @param r json串，如R"({"no":"0","mo":"100","nv":"0","mv":"100"})"
      * @note 显示结果 = (mo-no)*(实际值-nv)/(mv-nv) + no;
      * 	  或者对应关系 no是显示最小值，对应的nv是变量最小值; mo是显示最大值，对应mv是变量最大值
      */
     STTTextArea* setExchRate(const string& r = "");
     /**
      * @LEVEL2
      * @brief 设置文本框录入的上下限
      * @param low_limit 下限
      * @param up_limit 上限
      */
     STTTextArea* setLimit(double low_limit, double up_limit);
     /**
      * @LEVEL2
      * @brief 设置文本框录入可变上下限
      * @param lowVar 下限变量名
      * @param upVar  上限变量名
      */
     STTTextArea* setVariableLimit(const string& lowVar = "", const string& upVar = "");
     /**
      * @LEVEL2
      * @brief 获取下限
      */
     double getLowLimit();
     /**
      * @LEVEL2
      * @brief 获取上限
      */
     double getUpLimit();
     /**
      * @LEVEL0
      * @brief	设置键盘默认输入方式
      * @param kb 默认键盘输入方式,见stt::constant::IME :0~小写字母、1~大写字母、2~纯数字、3~符号及数字、4~T9拼音、5~全键盘拼音、6~无、7~自定义
      */
     STTTextArea* setImeMode(uint8_t kb);
     /**
      * @LEVEL0
      * @brief	获取键盘默认输入方式
      * return 	0~小写字母、1~大写字母、2~纯数字、3~符号及数字、4~T9拼音、5~全键盘拼音、6~无、7~自定义
      */
     uint8_t getImeMode();
     /**
      * @LEVEL2
      * @brief 设置基本参数
      * @param x 相对坐标x
      * @param y 相对坐标y
      * @param w 控件宽
      * @param h 控件高
      * @param pwdmode	是否启用密码模式(用*代替原字符)
      * @param online	是否启用单行模式
      * @param align	文本对齐方式 1~左 2~中 3~右 见stt::constant::Align
      * @param intlen	整数部分固定长度，即整数位数，为-1时不固定
      * @param fraclen	小数部分固定长度，即小数位数，为-1时不固定
      * @param fmin		可输入的下限值，为-1时，不设下限
      * @param fmax		可输入的上限值，为-1时，不设上限
      * @param beep		键盘按键音
      */
     STTTextArea* setMainArg(int x, int y, int w, int h, bool pwdmode=false, bool oneline=true, uint8_t align=1, int intlen=-1, int fraclen=-1, float fmin=-1.0E8, float fmax=1.0E8, bool beep=false);
     /**
      * @LEVEL2
      * @brief 设置基本样式
      * @param 	r			圆角半径
      * @param	c1			背景主色
      * @param 	c2			背景次色/渐变终止色
      * @param	dir			渐变方向: 0~不渐变  1~上下  2~左右. 见stt:constant::GradDir
      * @param	w_border	边框线宽
      * @param	c_border	边框线色
      * @param 	padt		文字到上边界的间隙
      * @param	padb		文字到下边界的间隙
      * @param	padl		文字到左边界的间隙
      * @param	padr		文字到右边界的间隙
      * @param	opa			背景透明度
      */
     STTTextArea* setMainStyle(int r=7,int c1=0xffffff,int c2=0xffffff,int dir=0,int w_bord=2,int c_bord=0xdddddd,int padt=4,int padb=4,int padl=4,int padr=4,int opa=255);
     /**
      * @LEVEL2
      * @brief	设置字体
      * @param c_font		文字颜色
      * @param fontsize		字体大小
      * @param fontname		字体名称
      */
     STTTextArea* setFont(int c_font=0x0, int fontsize=16, const string& fontname="", int fontstyle=0);
     /**
      * @HideCA
      * @brief	绑定变量变化时处理函数
      */
    void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @LEVEL2
     * @brief	隐藏光标(全透明)
     * @param b	为true时全透明，看不见。
     */
    STTTextArea* HideCursor(bool b = true);
    /**
     * @LEVEL2
     * @brief 指定选中文本的起始位置
     * @param index 起始位置，第1个是0
     */
    STTTextArea* setTextSelStart(uint32_t index);
    /**
     * @LEVEL2
     * @brief 指定选中文本的结束位置
     * @param index 结束位置，第1个是0
     */
    STTTextArea* setTextSelEnd(uint32_t index);
    /**
     * @LEVEL2
     * @brief 设置选中文本的背景色
     */
    void setTextSelBgColor(uint32_t c);
    /**
     * @LEVEL2
     * @brief 获取选中文本的起始位置
     */
    uint32_t getTextSelStart();
    /**
     * @LEVEL2
     * @brief 获取选中文本的结束位置
     */
    uint32_t getTextSelEnd();
    /**
     * @LEVEL2
     * @brief 获取选中文本的背景色
     */
    uint32_t getTextSelBgColor();
    /**
     * @LEVEL2
     * @brief 设置内置Label的对齐位置,见stt::constant::ALIGN
     */
    STTTextArea* setLabelAlign(uint8_t lvalign);
	/**
	 * @HideCA
	 */
    void onConfirm(uint16_t code, LvEvent e);
public:
    /**@HideCA*/
    void* mTux;
    /**
     * @LEVEL0
     * @brief 是否在获取焦点时选中全部文本
     */
    static bool isSelectAllOnFocus;
    /**
     * @LEVEL0
     * @brief	是否在数字键盘上显示最大最小值(绑定变量、默认为数字键盘时有效)
     */
    static bool isShowMinMaxInNumberKB;
    /**
     * @LEVEL2
     * @brief 设置自定义键盘的按钮
     * @param keymap 键盘按钮表，如{"F1","F2","F3","\n","F4","F5","F6",""} 其中\n表示换行,""表示结束
     * @note 参数keymap在使用期间不能被销毁，建议定义为静态全局数组，如static const char* keymap[] = {"F1","F2",""};
     */
    static void setCustomKey(const char** keymap);
    /**
     * @LEVEL2
     * @brief 设置自定键盘按钮的属性(宽度、隐藏等)，不考虑换行及结束
     * @param ctrlmap 键盘按钮的属性表。以下属性用或操作
     * 宽度：1~7
     * 隐藏：0x0008
     * 按住不重复:0x0010
     * 禁用：0x0020
     * 单击按下：0x0040
     * 初始为按下：0x0080
     * 按下时发送value_change事件：0x0100 (默认为点击时发送value_change事件)
     * 按下时浮现提示气泡：0x0200(类似手机输入法中按住一个键会有提示气泡)
     * 启用重着色0x1000, (在文字前用#ff0088 对文字着色,注意颜色后加空格，如"A #FF00FF B")
     */
    static void setCustomKeyCtrl(uint16_t* ctrlmap);
    /**
     * @LEVEL2
     * @brief 设置自定义键盘的按钮处理函数
     * @param func 按钮处理函数，这个函数返回true时，不再调用键盘默认处理函数；返回false时，会调用默认处理函数。
     * 如一些输入按钮，可返回false，即可输入
     */
    static void setCustomKeyHandler(bool (*func)(const char* txt));
    /**
     * @LEVEL2
     * @brief 设置自定义键盘的字体(如一些小语种、特殊符号在默认字体里没有时)
     */
    static void setCustomKeyFont(void* font);
    /**
     * @LEVEL2
     * @brief 设置自定义键盘的特殊键（如某键为SPACE，实际按下时，输入为空格，则setCustomSpecialKey("SPACE"," ", i)）
     * @param strshow 按键的字符串，如果不要特殊键了则setCustomSpecialKey("","", i)
     * @param strinput 实际输入的字符串
     * @param idx 这个特殊键在列表中的索引，范围0~7(特殊键最多可设8个)
     */
    static void setCustomSpecialKey(const string & strshow, const string& strinput, int idx);
    /**
     * @LEVEL2
     * @brief 获取IME的各部件样式指针，目前仅支持(0和1)
     * @param part 0-7 (0~整个底层容器，1~输入英文或数字或字母的键盘区，2~T9拼音选择的滚动轴
     * 		，3~字母、数字等模式选择区，4~汉字候选区，5~拼音标签，6~下拉按钮，7~汉字区下的可滑动面板)
     */
    static STTStyle* getImeStyle(int part);
    /**
     * @LEVEL2
     * @brief 设置自己的专属自定义键盘，不设置时，使用STTTextAreaInfo::currentInputPageId
     */
    void setprivCustInputPageId(const string& pageid);
    /**
     * @LEVEL2
     * @brief 设置自定义键盘的显示位置0~默认 1~居中 2~上 3~下 4~左 5~右 6~左上 7~右上 8~左下 9~右下
     */
    void setCustomImePosition(int ipos, bool isrelate=false);
private:
    /**
     * @HideCA
     * @brief	下限变量变化时处理函数
     */
   void onLowLimitChanged(SttObject * org, IVariable* data);
   /**
    * @HideCA
    * @brief	上限变量变化时处理函数
    */
  void onUpLimitChanged(SttObject * org, IVariable* data);
};

typedef STTTextArea* SttTextArea;

}

