/*
 * STTRoller.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "type/Types.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 滑动轴
 */
class STTRoller: public STTWidget {
public:
	/**
	 * @HideCA
     * @brief 滑动轴控件构造函数
     * @param rt		运行时环境
     * @param par		父控件
     * @param id		控件说明
	 */
	STTRoller(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTRoller();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h		位置及大小
     * @param mode			是否循环模式 0~否 1~是
     * @param options		选项
     * @param selectedIndex	默认选中项
     * @param beep			是否启用蜂鸣器
     */
    void setMainArg(int x=0, int y=0, int w=50, int h=100, int mode=0, const string& options = "", int selectedIndex = 0, bool beep=false);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r 		背景圆角
     * @param c1 		背景主色
     * @param c2		背景渐变终止色
     * @param dir		背景渐变方向
     * @param opa		背景不透明度
     * @param w_border	边框线粗细
     * @param c_border 	边框线颜色
     * @param w_shadow	阴影宽度
     * @param c_shadow	阴影颜色
     */
    void setMainStyle(int r=8, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=255, int w_border=2, int c_border=0xdddddd, int w_shadow=0, int c_shadow=0x0);
    /**
     * @LEVEL2
     * @brief 设置选中项样式
     * @param r 		背景圆角
     * @param c1 		背景主色
     * @param c2		背景渐变终止色
     * @param dir		背景渐变方向
     * @param opa		背景不透明度
     */
    void setSelectedStyle(int r=0, int c1=0x1a2b3, int c2=0x1a2b3, int dir=0, int opa=255);
    /**
     * @LEVEL2
     * @brief 设置字体
     * @param c1		非选中项的文字颜色
     * @param size1		非选中项的字体大小
     * @param fontname1	非选中项的字体名称
     * @param c2		选中项的文字颜色
     * @param size2		选中项的字体大小
     * @param fontname2	选中项的字体名称
     */
    void setFontStyle(int c1=0x0, int size1=16, const string& fontname1="", int c2=0xffffff, int size2=16, const string& fontname2="", int fontstyle1=0, int fontstyle2=0);
    /**
     * @LEVEL0
     * @brief 设置可见行数
     * @param row_cnt	行数
     */
    STTRoller* setVisibleRowCount(int row_cnt);
    /**
     * @LEVEL0
     * @brief 选中指定序号的选项
     * @param index		序号
     * @param notify	是否通知
     * @return
     */
    STTRoller* setSelected(int index, bool notify=true);
    /**
     * @LEVEL1
     * @brief 设置内置表的选项。
     * @param options		选项，json格式
     * @param selectedIndex 选中项id
     */
    void setOptions(const string& options = "", int selectedIndex=0);
    /**
     * @LEVEL0
     * @brief 获取选中项索引
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
     * @brief 获取选项个数
     * @return
     */
    int getOptionCnt();
    /**
     * @LEVEL1
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

public:
    /**@HideCA*/
    void onConfirm(uint16_t code, LvEvent e);
    /**@HideCA*/
    void* mRoller;
};

typedef STTRoller* SttRoller;

}
