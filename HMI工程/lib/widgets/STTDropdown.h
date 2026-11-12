/*
 * 下拉框
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "type/Types.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 下拉框
 */
class STTDropdown: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 下拉框控件构造函数
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTDropdown(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 下拉框控件析构函数
	 */
	virtual ~STTDropdown();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x x坐标
     * @param y y坐标
     * @param w 宽
     * @param h 高
     * @param options 选项（json格式）
     * @param selected 选中项
     * @param dir	下拉方向 1~左 2~右  4~上 8~下
     * @param symbol 下拉按钮图片
     * @param h_list 下拉框的最大高度，默认-1
     * @param beep 是否启用蜂鸣器
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=32, const string& options="", int selected=-1, int dir=8, const void* symbol=NULL, int h_list=-1, bool beep = false);
    /**
     * @LEVEL2
     * @brief 设置字体：1一般文字主体（显示项、下拉框）；2是选中项。
     * @param c1		一般项文字颜色
     * @param size1		一般项字体大小
     * @param fontname1	一般项字体名
     * @param issymbol	下拉箭头是否LV_SYMBOL_XXXX
     * @param c2		选中项文字颜色
     * @param size2		选中项字体大小
     * @param fontname2	选中项字体名
     */
    void setFonts(int c1, int size1, const string& fontname1, bool issymbol, int c2, int size2, const string& fontname2, int fontstyle1=0, int fontstyle2=0);
    /**
     * @LEVEL2
     * @brief 设置基本样式参数
     * @param r 圆角半径
     * @param c1 主色
     * @param c2 次色(渐变时有效)
     * @param dir 渐变方向(0~不渐变 1~上下 2~左右)
     * @param w_border 边框宽
     * @param c_border 边框颜色
     * @param w_shadow 阴影宽
     * @param c_shadow 阴影颜色
     */
    void setMainStyle(int r, int c1, int c2, int dir, int w_border, int c_border, int w_shadow, int c_shadow);
    /**
     * @LEVEL2
     * @brief 设置选中项样式参数
     * @param r 圆角半径
     * @param c1 主色
     * @param c2 次色(渐变时有效)
     * @param dir 渐变方向(0~不渐变 1~上下 2~左右)
     * @param w_border 边框宽
     * @param c_border 边框颜色
     */
    void setListSelectedStyle(int r, int c1, int c2, int dir, int w_border, int c_border);
    /**
     * @LEVEL1
     * @brief 设置弹框位置
     * @param dir	ScrollDir::LEFT/RIGHT/TOP/BOTTOM
     */
    STTDropdown* setDir(int dir = ScrollDir::BOTTOM);
    /**
     * @LEVEL1
     * @brief 设置下拉按钮的图标或符号
     * @param symbol	中文串视为符号、字母或数字串视为文件名称。NULL无图标。ICON::get(ICON::AUDIO)
     * @param isself	是否内置图标符号(是，则将图标字体设为LV_FONT_DEFAULT)
     * @note isself为false时，加入fontawesome字体，可使用其全部图标
     */
    STTDropdown* setSymbol(const void* symbol, bool isself=false);
    /**
     * @LEVEL1
     * @brief 设置是否在弹出框中高亮显示选中项
     * @param en
     * @return
     */
    STTDropdown* setSelectedHightlight(bool en);
    /**
     * @LEVEL1
     * @brief 设置内置表的选项。
     * @param options		选项，json格式
     * @param selectedIndex 选中项id
     */
    void setOptions(const string& options = "", int selectedIndex=-1);
    /**
	 * @LEVEL1
	 * @brief 清除选项(内置表或绑定表均可)
	 */
	void clearOptions();
	/**
	 * @LEVEL1
	 * @brief 追加选项(内置表)，绑定配方变量无效
	 * @note 注意相同id可重复添加
	 */
	void addOption(const string& id, const string& name);
	/**
	 * @LEVEL1
	 * @brief 获取选项总数，调用表的行数(内置表或绑定表均可，即数据源的行数)
	 * @note 与getOptionCnt的区别:这是数据源的行数
	 */
	int getOptionCount();
    /**
     * @LEVEL0
     * @brief 获取选项个数(调用lvgl方法，即显示项的个数)
     * @return
     * @note 与getOptionCount的区别:这是控件中显示项的个数
     */
    int getOptionCnt();
    /**
     * @LEVEL0
     * @brief 获取选中项序号
     * @return
     */
    int getSelectedIndex();
    /**
     * @LEVEL0
     * @brief 获取选中项文本
     * @return
     */
    string getSelectedText();
    /**
     * @LEVEL0
     * @brief 获取选中项的值
     * @return
     */
    string getSelectedValue();
    /**
     * @LEVEL0
     * @brief 选中指定序号的选项
     * @param index		序号
     * @param notify	是否通知
     * @return
     */
    STTDropdown* setSelected(int index, bool notify = true);
    /**
     * @LEVEL1
     * @brief 获取弹框图标
     * @return
     */
    const char* getSymbol();
    /**
     * @LEVEL1
     * @brief 是否高亮显示选中项
     * @return
     */
    bool getSelectedHightlight();
    /**
     * @LEVEL1
     * @brief 获取弹框位置
     * @return
     */
    int getDir();
    /**
     * @LEVEL1
     * @brief 弹出下拉框
     * @return
     */
    STTDropdown* expand();
    /**
     * @LEVEL1
     * @brief 收起下拉框
     * @return
     */
    STTDropdown* collapse();
    /**
     * @LEVEL1
     * @brief 获取数据对象
     * @return
     */
    TTable* getData();

    /**
     * @LEVEL1
     * @brief Dropdown获取下拉列表的样式指针
     */
    STTStyle* getListStyle();
public:
    /**
     * @HideCA
     * @brief 变量绑定响应方法
     * @param org
     * @param data
     */
    void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 确认处理
     */
    void onConfirm(uint16_t code, LvEvent e);
    /**
     * @HideCA
     * @brief 内部指针
     */
    void* mMux;
};
typedef STTDropdown* SttDropdown;

}
