/*
 * 列表框
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "type/Types.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 列表框
 */
class STTList: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建列表框控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTList(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTList();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x		X坐标
     * @param y		Y坐标
     * @param w		宽
     * @param h		高
     */
    void setMainArg(int x=0, int y=0, int w=50, int h=200, bool beep=false);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色
     * @param c2		背景渐变终止色
     * @param dir		渐变方向 0~不渐变  1~上下  2~左右
     * @param w_border	边框线宽
     */
    void setMainStyle(int r=8, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=2, int c_border=0xc0c0c0);
    /**
     * @LEVEL2
     * @brief 设置字体
     * @param c1		默认文字颜色
     * @param c2		选中项文字颜色
     * @param size		字体大小
     * @param fontname	字体名称、路径
     */
    void setFonts(int c1=0x0, int c2=0xffffff, int size=16, const string& fontname="", int fontstyle=0);
    /**
     * @LEVEL1
     * @brief 设置内置表的选项。
     * 注意：使用时放在setFonts等样式之后，以确保增加的按钮样式、字体正确；使用外部绑定变量时，本方法无意义。
     * @param options	选项，json格式
     * @example wMList1->setOptions(R"([{"id":"1","name":"1"},{"id":"2","name":"2"}])");
     */
    void setOptions(const string& options = "");
    /**
     * @LEVEL1
     * @brief 清除选项(内置表或绑定表均可)
     */
    void clearOptions();
    /**
     * @LEVEL1
     * @brief 追加选项(内置表)
     * @note	addButton与addOption不能混用。
     * 			addButton仅仅是在控件上增加一个选项，不会影响数据源
     * 			addOption是修改数据源，并显示在控件上
     */
    void addOption(const string& id, const string& name);
    /**
     * @LEVEL1
     * @brief 删除选项(内置表)
     */
    bool removeOption(const string& id);
    /**
     * @LEVEL1
     * @brief 获取选项总数(内置表或绑定表均可)
     * @note 这是数据源的行数
     */
    int getOptionCount();
    /**
     * @LEVEL1
     * @brief 获取选项总数
     * @note 这是显示内容的行数
     */
    int getOptionCnt();
    /**
     * @LEVEL1
     * @brief 获取全部选项
     */
    vector<KV_Str_Str> getOptions();
    /**
     * @LEVEL1
     * @brief 获取全部选项的id
     */
    vector<string> getIds();
    /**
     * @LEVEL1
     * @brief 获取全部选项的name
     */
    vector<string> getNames();
    /**
     * @LEVEL2
     * @brief 设置选中项的样式
     * @param r			圆角半径
     * @param c1		选中项的主色
     * @param c2		选中项的渐变终止色
     * @param dir		渐变方向 0~不渐变  1~上下  2~左右
     * @param w_border  边框线宽
     */
    void setButtonStyleSelected(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=0);
    /**
     * @LEVEL0
     * @brief 设置选中项
     * @param i			选中项序号
     * @param notify	是否通知
     * @note 对于addButton的选项无效
     */
    void setSelected(int i, bool notify = true);
    /**
     * @LEVEL0
     * @brief 获取选中项序号
     * @return	-1:无效 >=0：序号
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
     * @brief 获取数据对象
     */
    TTable* getData();
    /**
     * @HideCA
     * @brief 数据处理
     * @param org
     * @param data
     */
    void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @LEVEL1
     * @brief 增加一项(按钮)，可带图标及文字
     * @param icon	按钮上的图标(图片文件路径)
     * @param txt	按钮上的文字
     * @return	返回这个按钮指针(lv_obj_t*)
     * @example addButton("./res/pic/add.png", "新建");
     * @note	addButton与addOption不能混用。
     * 			addButton仅仅是在控件上增加一个选项，不会影响数据源
     * 			addOption是修改数据源，并显示在控件上
    */
    void* addButton(const string icon, const string& txt);
    /**
     * @LEVEL0
     * @brief 禁止/使能控件
     * @param en true~使能 false~禁止
     */
    STTWidget* setEnable(bool en);
public:
    /**@HideCA
     * @brief 确认操作内部处理方法
     */
    void onConfirm(uint16_t code, LvEvent e);
    /**@HideCA
     * @brief 内部指针*/
    void* mList;
};

typedef STTList* SttList;

}
