/*
 * STTQRCode.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTCanvas.h"

namespace stt{
/**
 * @ingroup widget
 * @brief 二维码
 */
class STTQRCode: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 二维码控件构造函数
	 * @param rt 	运行时接口指针
	 * @param layer	所在层
	 * @param id 	控件ID
	 */
	STTQRCode(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 */
	virtual ~STTQRCode();
    /**
     * @Level2
     * @brief 设置基本参数
     * @param x,y,w,h		位置、大小
     * @param c_bg			背景色
     * @param c_fg			前景色
     * @param data			二维码内容
     */
    void setMainArg(int x=0, int y=0, int w=100, int h=100, int c_bg=0xffffff, int c_fg=0x0, const char* data="www.simtutai.com");
    /**
     * @Level0
     * @brief 更新二维码
     * @param data 二维码内容/data to display
     * @param data_len 字符串长度/length of data in bytes
     * @return 成功返回1，失败返回0  /  1: if no error; 0: on error
     */
    uint8_t update(const void* data="www.simtutai.cn", int data_len=15);
    /**
     * @Level0
     * @brief 更新二维码
     * @param data 二维码内容/data to display
     * @return 成功返回1，失败返回0  /  1: if no error; 0: on error
     */
    uint8_t update(const string& data="www.simtutai.cn");
    /**
     * @LEVEL1
     * @brief 	设置控件尺寸
     * @param w		宽
     * @param h		高
     */
    STTWidget* setSize(int w, int h);
    /**
     * @LEVEL0
     * @brief 	设置控件尺寸
     * @param w		宽
     */
    STTWidget* setWidth(int w);
    /**
     * @LEVEL0
     * @brief 	设置控件尺寸
     * @param h		高
     */
    STTWidget* setHeight(int h);
public:
	/**
	 * @HideCA
	 */
    void onDataChanged(SttObject * org, IVariable* data);
public:
    /**@HideCA
     * @brief 内部指针*/
    void* mQr;
};

typedef STTQRCode* SttQRCode;

}
