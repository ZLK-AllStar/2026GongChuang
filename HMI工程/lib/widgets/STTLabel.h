/*
 * 标签控件
 *
 *  Created on: 2023年2月18日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"

namespace stt{

/**
 * @ingroup widget
 * @brief 文字
 */
class STTLabel: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 创建文本标签控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTLabel(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTLabel();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x			位置x
     * @param y			位置y
     * @param w			宽度
     * @param h			高度
     * @param txt		文本内容
     * @param align	 	文本对齐方式 0~自动, 1~左, 2~中, 3~右
     * @param longmode	长文本模式 (见constant::LongMode)
     * @param speed		长文本模式为2/3时的滚动速度 像素/秒
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=40, const string& txt="Label", int align=1, int longmode=0, int speed=10);
    /**
     * @LEVEL2
     * @brief 设置报警条的参数
     * @param n	显示报警记录的最新n条信息
     */
    void setWarningArg(int n=1);
    /**
     * @LEVEL2
     * @brief 设置字体
     * @param c1		文字颜色
     * @param size1		字体大小
     * @param fontname1	字体名，为""时，取默认字体
     * @param fontstyle 0~正常 1~斜体 2~粗体 3~斜体加粗体
     */
    void setFonts(int c1=0x0, int size1=16, const string& fontname1="", int fontstyle=0);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色
     * @param c2		背景渐变终止色(dir!=0时有效)
     * @param dir		渐变方向(见constant::GradDir)
     * @param w_border	边框线宽
     * @param c_border	边框线色
     * @param opa		背景不透明度(0~255)
     */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int w_border=0, int c_border=0x0, int opa=0);
    /**overload*/
    STTWidget* setPos(int x, int y);
    /**
     * @LEVEL0
     * @brief 设置文本
     * @param txt	文本
     * @return
     */
    void setText(const string& txt);
    /**
     * @LEVEL1
     * @brief 是否启用recolor
     */
    void setRecolor(bool en);
    /**
     * @LEVEL0
     * @brief 设置内置图标
     * @param symbol	内置图标(constant::ICON)
     * @param size		大小
     * @return
     * @example setSymbol(stt::constant::ICON::AUDIO, 20)
     */
    STTLabel* setSymbol(uint8_t symbol, int size);
    /**
     * @LEVEL1
     * @brief 设置长文本模式
     * @param long_mode 0~自动换行 1~超宽部分用...代替 2~来回滚动  3~循环滚动 4~裁剪超出部分
     * 					见stt::constant::LabelLongMode
     */
    STTLabel* setLongMode(int long_mode);
    /**
     * @LEVEL0
     * @brief 获取控件文本内容
     * @return
     */
    string getText();
    /**
     * @LEVEL2
     * @brief 获取标签值
     * @return
     */
    string getTag();
    /**
     * @LEVEL1
     * @brief 获取长文本模式
     * @return
     */
    int getLongMode();
    /**
     * @HideCA
     * @brief 数据处理
     * @param org
     * @param data
     */
    virtual void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 语言变化处理
     * @param org
     * @param data
     */
    virtual void onLanguageChanged(SttObject * org, IVariable* data);
    /**
     * @LEVEL0
     * @brief 显示变量值
     * @param data	变量
     */
    void setValue(IVariable* data);
public:	//设置显示格式
    /**
     * @LEVEL2
     * @brief 将当前文本内容格式化成文本显示
     * @param align		文字对齐 0~自动 1~左 2~中 3~右
     * @param longmode	long_mode 0~自动换行 1~超宽部分用...代替 2~来回滚动  3~循环滚动 4~裁剪超出部分
     * 					见stt::constant::LabelLongMode
     * @param speed		长文本模式为2/3时的滚动速度 像素/秒
     */
    void setFormatText(int align=0, int longmode=0, int speed=10);
    /**
     * @LEVEL2
     * @brief 将当前文本内容格式化成数字显示
     * @param fraclen		保留小数位数
     * @param isthousandth	是否显示千分位逗号
     * @param fmttype		数字格式	1~普通数字 4~百分比 5~科学计数
     * @param factor		显示系数
     * @param offset		偏移量
     * @param intnum		整数位长度
     * @param intbl			整数位补零
     * @param prefix		前缀
     * @param suffix		后缀
     */
    void setFormatNumerical(int fraclen=0, bool isthousandth=false, int fmttype=1,double factor=1.0,
    		double offset=0.0, int intnum=0, bool intbl=false, const string& prefix="", const string& suffix="");
    /**
     * @LEVEL2
     * @brief 将当前文本内容格式化成货币显示
     * @param fraclen		保留小数位数
     * @param isthousandth	是否显示千分位逗号
     * @param symbol		货币符号
     */
    void setFormatCurrency(int fraclen=2, bool isthousandth=false, const string& symbol="$");
    /**
     * @LEVEL2
     * @brief 将当前文本内容(必须是10位数字，即1970至今的秒数)格式化成日期显示
     * @param deli1			年月日的分隔符，若deli1="中文"，则显示xxxx年xx月xx日
     * @param deli2			时分秒的分隔符，若deli2="中文"，则显示xx时xx分xx秒
     * @param YY			是否显示年
     * @param MM			是否显示月
     * @param DD			是否显示日
     * @param h				是否显示时
     * @param m				是否显示分
     * @param s				是否显示秒
     * @param w				是否显示星期
     */
    void setFormatDate(const string& deli1="-", const string& deli2=":", bool YY=true, bool MM=true, bool DD=true, bool h=true, bool m=true, bool s=true, bool w=false);
    /**
     * @LEVEL0
     * @brief 文本垂直居中
     */
    void middle();
    /**
     * @HideCA
     * @brief 刷新文本内容
     */
    void refresh();
    /**
     * @LEVEL0
     * @brief 文字高度
     * @return
     */
    int16_t getTextHeight();
    /**
     * @LEVEL1
     * @brief 设置报警条的数据源
     * @param	catalog	类别[0..99]
     * @param	level	级别[0..3]
     */
    void setDataSourceOfEvent(int catalog, int level);
    /**
     * @LEVEL1
     * @brief 设置报警条的数据源
     * @param cata1 类别范围起始(含)[0..99]
     * @param cata2 类别范围终止(含)[0..99]
     * @param level 级别[0..15]: 低级1 | 一般2 | 高级4 | 紧急8，如全部显示则为15
     */
    void setDataSourceOfEvent(int cata1, int cata2, int level);
    /**
     * @HideCA
     * @brief 设置报警条的显示内容(对应8位二进制)
     * @param flags 各位分别表示：
     * 			1<<0 事件ID
     * 			1<<1 事件名称
     * 			1<<2 开累次数
     * 			1<<3 开累时长
     * 			1<<4 时间
     * 			1<<5 用户
     * 			1<<6 说明
     * 			1<<7 持续时长
     */
    void setEventFlags(int flags);
protected:

    /**
     * @HideCA
     * @brief 获取单行显示时的文本宽度
     */
    int getTextOneLineWidth();

    /**
     * @HideCA
     * @brief 内部指针
     */
    void* mMux;
};
typedef STTLabel* SttLabel;

/**
 * 多状态标签
 */
class STTMultiLabel: public STTLabel {
public:
	/**
	 * @HideCA
	 * @brief 文本标签控件
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTMultiLabel(IRuntime* rt, STTWidget* par, int type=0, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTMultiLabel();
	/**
	 * @LEVEL0
	 * @brief 设置标签
	 * @param state	状态值
	 * @param title 标签
	 * @param style 样式
	 */
	void setStateTitle(const string& state, const string& title="", const string& style="");
    /**
     * @HideCA
     * @brief 数据处理
     * @param org
     * @param data
     */
    void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 语言处理
     * @param org
     * @param data
     */
    void onLanguageChanged(SttObject * org, IVariable* data);
protected:
    /**@HideCA*/
    void* mLbls;
};
typedef STTMultiLabel* SttMultiLabel;

}
