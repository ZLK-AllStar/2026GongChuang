/*
 * Printer.h
 *
 *  Created on: 2023年10月20日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <fstream>
#include <queue>
#include "base/Interface.h"

namespace stt::misc {

/**
 * @brief 条码打印机类
 */
class IBarcodePrinter {
public:
	/**
	 * 获取打印机
	 * @param cfg	配置信息
	 * @return
	 */
	static IBarcodePrinter* getPrinter();
	/**
	 * 打印机析构函数
	 */
	virtual ~IBarcodePrinter();
    /**
     * @LEVEL1
     * @brief 设置参数
     * @param settings	控件参数
     * @return 是否成功
     */
	virtual bool set(const string& settings="");
	/**
	 * @LEVEL0
	 * @brief 启动
	 * @return	是否成功
	 */
	virtual bool open();
	/**
	 * @LEVEL0
	 * @brief 	关闭
	 * @return	是否成功
	 */
	virtual bool close();
	/**
	 * @LEVEL0
	 * @brief 暂停
	 * @return	是否成功
	 */
	virtual bool pause();
	/**
	 * @LEVEL0
	 * @brief 继续
	 * @return	是否成功
	 */
	virtual bool resume();
	/**
	 * @LEVEL0
	 * @brief 获取状态
	 * @return <0: 设置错误  0:未启动 1:启动 2:暂停
	 */
	virtual int status();
	/**
     * @LEVEL1
     * @brief 发送指令
	 * @return	是否成功
	 */
	virtual bool sendCmd();
	/**
	 * 发送打印指令
	 */
	/**
     * @LEVEL1
     * @brief 打印标签
	 * @return	是否成功
	 */
	virtual bool print();

	/**
	 *	添加指令
	 * @param command 指令数据
	 */
	virtual void addCmd(const std::vector<uint8_t>& command);


	/**
	 *	初始化打印机
	 */
	virtual void initPrinter();

	/**
	 * 设置边距
	 * @param left  左边距
	 * @param right 右边距
	 */
	virtual void setEdgeSize(unsigned left,unsigned right);

	/**
	 * 设置汉字模式
	 * @param n 汉字模式
	 * n == 0 取消汉字模式
	 * n 不为零 设置汉字模式
	 */
	virtual void setChineseMode(uint8_t n);

	/**
	 *	设置字体粗细
	 * @param n 字体加粗打印
	 *  n=0，不加粗
	 *  n=1，字体加粗
	 */
	virtual void setFontWeight(uint8_t n);

	/**
	 *	设置字符大小
	 *  @param w 字符宽度
	 *  @param h 字符高度
	 *  w=1:普通 2~8:w倍宽
	 *  h同上
	 */
	virtual void setFontSize(uint8_t w, uint8_t h);

	/**
	 *	设置下划线模式
	 * @param n 下划线模式
	 *  n=0，不显示
	 *  n=1，添加1点宽下划线
	 *  n=2，添加2点宽下划线
	 */
	virtual void setUnderline(uint8_t n);

	/**
	 *	设置字符旋转打印
	 * @param n 字符旋转打印模式
	 *  n=0，取消字符旋转
	 *  n=1，设置字符旋转90
	 *  n=2，设置字符旋转270
	 */
	virtual void setCharRotatPrint(uint8_t n);

	/**
	 *	设置颠倒打印
	 * @param n 颠倒打印模式
	 *  n=0，取消颠倒打印模式
	 *  n=1，设置颠倒打印模式
	 */
	virtual void setReversePrintMode(uint8_t n);

	/**
	 * 设置行间距
	 * @param n  设置行间距为（n×0.125毫米）。0≤n≤255，默认n=3。
	 */
	virtual void setLineSpacing(uint8_t n);

	/**
	 *  设置字符右间距
	 * @param n 设置字符右间距（n×0.125 mm）。0≤n≤255，默认n=0。
	 */
	virtual void setCharRightSpacing(uint8_t n);
	/**
	 * 设置文本对齐方式
	 * @param n 0：左对齐  1：居中  2：右对齐
	 * 将一行数据按照指定的位置对齐。
	 * 【说明】
	 * 1、0≤n≤2，默认n=0；
	 * 2、仅在一行开始时，该命令才有效。
	 */
	virtual void setTextAlignment(uint8_t n);
	/**
	 * 设置打印灰度
	 * @param n 通过不同n值设置不同打印浓度
	 * 【说明】
	 * 1、1≤n≤10,默认n=7；
	 * 2、n=1打印颜色最浅，n=10打印颜色最深。
	 */
	virtual void setPrintDensity(uint8_t n);
	/**
	 *	选择字体
	 * @param n 0：选择字符字体A（12×24）  1：选择字符字体B（8×16）
	 */
	virtual void selectFont(uint8_t n);
	/**
	 * 选择字符集
	 * @param n	0：选择字符集1  1：选择字符集2
	 * 【说明】通过该命令选择字符集1的字符进行打印。
	 * 1、 字符集1有字符224个，包括ASCII字符，及各种图形符号等；
	 * 2、 字符集2有字符224个，包括德文、法文、俄文、日语片假名等；
	 * 3、 代码范围20H-FFH（32-255）。
	 */
	virtual void selectCharacterSet(uint8_t n);
	/**
	 * 设置水平制表位
	【说明】设置水平制表的位置n1,n2等，最多可以设置8个位置。例n1=3,则执行水平造表
	(HT命令)时在第3个单位长度处开始打印。n的取值受当前字符集以及横向放大倍数k的影
	响。当字符集是24点阵字符集时，单位位置距离12×0.125mm,1≤n≤LineDot/12/k；当字
	符集是16点阵字符集时，单位位置距离8×0.125mm，1≤n≤LineDot/8/k；当字符集是字
	符集1和字符集2时，单位位置距离6×0.125mm ,1≤n≤LineDot/6/k；数据NUL加在最后
	表示该命令结束。所有输入的水平造表位置，可用该命令以ESC D NUL（1B 44 00）的格式
	清除。注意：汉字占2个单位位置，ASCII占1个单位位置，所以当n为最大值时只能打印
	一个ASCII字符，不能打印汉字字符
	 */
	virtual void setHorizontalTabPositions(const vector<uint8_t>& positions);

	/**
	 * 09 执行水平造表
	 */
	virtual void printTab();

	/**
	 * 打印并进纸
	 * @param n 0≤n≤255
	 * 【说明】打印出缓存区的数据并进纸：n×0.125毫米。
	 * 1、0≤n≤255；
	 * 2、打印结束后，该命令将打印的起始位置设置为行起点；
	 * 3、该命令的进纸量并不影响ESC 2或ESC 3命令设置的值;
	 * 4、由于打印机的打印头和撕纸口存在一小段距离，打印结束后，打印内容并未完全走出来。
	 * 可在数据末尾加上该指令让打印机多走一小段距离，以便将打印内容完全推出撕纸口。
	 */
	virtual void printAndFeed(uint8_t n);

	/**
	 * 打印文本
	 * @param pstring 	打印文字
	 * @param xPos 		文字X方向间隔距离
	 */
	virtual void PrinterString(const char *pstring,int xPos);

	/**
	 * 打印并换行
	 */
	virtual void PrinterNewLine();

	/**
	 * 打印图片
	 * @param filename  文件完整地址
	 * @param printerWidth  目标宽度
	 * @param printerHeight  目标高度
	 * 支持多种图像格式（如 BMP、PNG、JPG 等）
	 */
	virtual void printImage(const char* filename, unsigned  printerWidth, unsigned  printerHeight);

	/**
	 * 设置条形码高度
	 * @param n 条码数据
	 * n设定条形码垂直方向的点数。0≤n≤255，默认n=162。
	 */
	virtual void setBarcodeHight(unsigned n);

	/**
	 * 设置条形码宽度
	 * @param n 条形码宽度
	 * n 条形码宽度（单位：毫米）
	 * 2 0.250
	 * 3 0.375（默认）
	 * 4 0.5
	 * 当设置条码宽度使条码宽度超出打印范围时，条码将不能被识别。
	 */
	virtual void setBarcodeWidth(unsigned n);

	/**
	 * 打印条形码
	 * @param barcodeData 条码数据
	 * @param m  选择条码系统
		【格式】ASCII码：GS  k  m n d1...dn
			 10进制： 29  107  m n d1...dn
			 16进制： 1D  6B  m n d1...dn
		【说明】
		m 条码系统 n D（条码数据）
		67 EAN13 12≤n≤13 48≤d≤57
		68 EAN8 7≤n≤8 48≤d≤57
		69 CODE39 1≤n≤255 48≤d≤57, 65≤d≤90,
		32,36,37,45,46,47
		74 CODE128 1≤n≤255 0≤d≤127
		1、要注意各个条形码所规定的字符数。EAN8、EAN13、CODE39和CODE128可自动产生校验
		字符，用户不用输入校验字符；
		2、条码数据字符应该包含在条码类型规定的字符集中，如果条码数据字符超出了字符集，
		该命令无效，各类型条码规定的字符集请参考各条码的标准；
		3、CODE39码不包含扩展码（EXTERN CODE39）；
		4、CODE128条码数据串的头部必须是编码集选择字符（CODE A、CODE B、CODE C），在一个
		条码内部也可切换编码集。用字符“{”和一个字符组合用以定义特殊功能，通过连续传送
		两次“{”定义ASCII字符“{”。如下表所示：
		ASCII  HEX  功能
		{A  7B, 41  选择编码集A
		{B  7B, 42  选择编码集B
		{C  7B, 43  选择编码集C
		{S  7B, 53  SHIFT
		{1  7B, 31  FNC1
		{2  7B, 32  FNC2
		{3  7B, 33  FNC3
		{4  7B, 34  FNC4
		5、编码集选择字符（CODE A、CODE B、CODE C）见附录；
		6、打印出条码默认居中；
		7、如果条码宽度超出了打印区域，则打印机只是进纸。
	 */
	virtual void printBarcode(vector<uint8_t>& barcodeData, uint8_t m, uint8_t n);
	/**
	 * 打印二维码
	 * @param QRCodeData QR码数据
	 * @param version 1～9，QR码版本号
	 * @param errorLevel 0～3，ECC纠错级别(依次对应 L:7%, M:15%,Q:25%,H:30%)
		【格式】ASCII码：GS  ( 1 ml mh v e [d]
			 10进制： 29  40 49 ml mh v e [d]
			 16进制： 1D  28 31 ml mh v e [d]
		【说明】
		1、mh×256+ml数据字节数（含v,e）
		2、v=1～9，QR码版本号（0表示自动选择版本）
		3、e=0～3，ECC纠错级别(依次对应 L:7%, M:15%,Q:25%,H:30%)
		4、[d] 数据(任意内容)；
		5、QR 码符号模块的大小为一维条码窄条尺寸的3倍。
	 */
	virtual void printQRCode(vector<uint8_t>& QRCodeData, uint8_t version, uint8_t errorLevel);
	/**
	 * 控制切刀
	 * @param n=0，全切纸； n=1，半切纸。无切刀机型，此指令无效。
	 */
	virtual void controlCutter(uint8_t n);
	/**
	 * 查询打印机状态
	 */
	virtual void queryStatus();
};

/**
 * @brief 条码打印机类
 */
class ILabelPrinter {
public:
	/**
	 * 获取打印机
	 * @param cfg	配置信息
	 * @return
	 */
	static ILabelPrinter* getPrinter();
	/**
	 * 打印机析构函数
	 */
	virtual ~ILabelPrinter();
    /**
     * @LEVEL1
     * @brief 设置参数
     * @param settings	控件参数
     * @return 是否成功
     */
	virtual bool set(const string& settings="");
	/**
	 * @LEVEL0
	 * @brief 启动
	 * @return	是否成功
	 */
	virtual bool open();
	/**
	 * @LEVEL0
	 * @brief 	关闭
	 * @return	是否成功
	 */
	virtual bool close();
	/**
	 * @LEVEL0
	 * @brief 暂停
	 * @return	是否成功
	 */
	virtual bool pause();
	/**
	 * @LEVEL0
	 * @brief 继续
	 * @return	是否成功
	 */
	virtual bool resume();
	/**
	 * @LEVEL0
	 * @brief 获取状态
	 * @return <0: 设置错误  0:未启动 1:启动 2:暂停
	 */
	virtual int status();
	/**
     * @LEVEL1
     * @brief 发送指令
	 * @return	是否成功
	 */
	virtual bool sendCmd();
	/**
	 * 发送打印指令
	 */
	/**
     * @LEVEL1
     * @brief 打印标签
	 * @return	是否成功
	 */
	virtual bool print();

	/**
	 *	添加指令到字符串缓存
	 * @param command 指令字符串
	 */
	virtual void addCmd(const std::string &command);


	/**************      标签打印机      *****************/

	/**************      设置指令      *****************/
	/**
	 *	设置纸张大小
	 * @param pageWidth      纸张宽度单位mm
	 * @param pageHeight 	纸张高度单位mm
	 */
	virtual void setSize(int pageWidth, int pageHeight);
	/**
	 *	设置标签间距
	 * @param gapDistance 纸张间距（单位mm）
	 */
	virtual void setGap(int gapDistance);
	/**
	 * 设定黑标高度及定义标签印完后标签额外送出的长度
	 * @param m  黑标高度 0≤m≤1（inch），0≤m≤25.4（mm）
	 * @param n  额外送出纸张长度 n≤标签纸纸张长度（inch 或 mm）
	 */
	virtual void setBline(int m, int n);
	/**
	 *  该指令用于控制在剥离模式时（peel-off mode）每张卷标停止的位置，在打印
	 *  下一张时打印机会将原先多推出或少推出的部分以回拉方式补偿回来。该指令仅
	 *  适用于剥离模式。
	 * @param n 停止长度单位mm
	 */
	virtual void setOffset(int n);
	/**
	 * 该指令用于定义打印时出纸和打印字体的方向
	 * @param n 0：出纸方向  1：进纸方向
	 */
	virtual void setDirection(int n);
	/**
	 * 设置打印速度
	 * @param n 1.5 设定打印速度为 1.5＂/sec 2.0 设定打印速度为 2.0＂/sec
	 *          3.0 设定打印速度为 3.0＂/sec 4.0 设定打印速度为 4.0＂/sec
	 */
	virtual void setSpeed(float n);
	/**
	 *	设置打印时的浓度
	 * @param n 0～15 0：使用最淡的打印浓度 15：使用最深的打印浓度
	 */
	virtual void setDensity(int n);
	/**
	 * 指定区域反向打印
	 * @param x_start	x起始水平方向起始点坐标
	 * @param y_start   y起始垂直方向起始点坐标
	 * @param x_width   反向打印的宽度
	 * @param y_height  反向打印的高度
	 */
	virtual void setReverse(int x_start, int y_start, int x_width, int y_height);
	/**
	 * 切断参数
	 * @param OFForBATCHorPieces: OFF 关闭切刀功能 BATCH 在 PRINT 命令结束后切纸
	 *							  pieces 0-65535，用于设置每几个标签进行切纸
	 */
	virtual void setCutter(const std::string &OFForBATCHorPieces);
	/**
	 * 设置到间隙处
	 * @param tear ONorOFF
	 */
	virtual void setTear(const std::string &tearONorOFF);
	/**
	 * 打印数量
	 * @param m  指定打印的份数（set）1≤m≤65535
	 * @param n  每张标签需重复打印的张数 1≤n≤65535
	 */
	virtual void setPrintnum(int m, int n);
	/**
	 * 定义卷标的参考坐标原点
	 * @param x 水平方向的坐标位置,单位 dot
	 * @param y 垂直方向的坐标位置,单位 dot
	 */
	virtual void setReference(int x, int y);
	/**
	 * 往前推指定距离
	 * @param n 1≤n≤9999，单位dot
	 */
	virtual void feed(int n);
	/**
	 * 往后卷指定距离
	 * @param n  1≤n≤9999，单位dot
	 * @param m  1≤n≤9999，单位dot
	 */
	virtual void backFeed(int n, int m);

	/**************      绘制指令      *****************/
	/**
	 * 打印线条
	 * @param x 		左上角X坐标，单位dot
	 * @param y 		线条左上角Y坐标，单位dot
	 * @param width 	线宽，单位dot
	 * @param height 	线高，单位dot
	 */
	virtual void drawBar(int x, int y, int width, int height);
	/**
	 * 打印文本
	 * @param x 		文字X方向起始点坐标
	 * @param y 		文字Y方向起始点坐标
	 * @param font 		字体名称 1 8×12 dot英数字体,2 12×20 dot英数字体,3 16×24 dot英数字体,
	 *	 	 	 	 	4 24×32 dot英数字体,5 32×48 dot英数字体,6 14×19 dot英数字体OCR-B,7 21×27 dot英数字体OCR-B,8 14×25 dot英数字体OCR-A,9 9×17 dot英数字体,10 12×24 dot英数字体
	 *	 	 	 	 	TST24.BF2繁体中文24×24Font(大五码)、TSS24.BF2：简体中文24×24Font(GB码) K：韩文24×24Font (KS码)
	 * @param rotation  0 0度，90 90 度，180 180 度，270 270度
	 * @param x_mul 	X方向放大倍率1-10
	 * @param y_mul 	Y方向放大倍率1-10
	 * @param text 		打印文字
	 */
	virtual void drawText(int x, int y, const std::string &font, int rotation, int x_mul, int y_mul, const std::string &text);
	/**
	 * 打印二维码
	 * @param x 			文字X方向起始点坐标
	 * @param y 			文字Y方向起始点坐标
	 * @param ecclevel  	选择QRCODE纠错等级 L 7%,M 15%,Q 25%,H 30%
	 * @param cellwidth 	二维码宽度1-10
	 * @param mode 			手动/自动编码 A,Auto M,Manual
	 * @param rotation 		旋转角度（顺时针方向)0 0度,90 90 度,180 180度,270 270度
	 * @param data		    编码的字符串
	 */
	virtual void drawQRcode(int x, int y, char ecclevel, int cellwidth, char mode, int rotation, const std::string &datastring);
	/**
	 * 打印一维条码
	 * @param x 			二维码水平方向起始点坐标
	 * @param y 			二维码垂直方向起始点坐标
	 * @param codetype 		常用数字 39 常用字幕 128
	 * @param height 		条形码高度，以点（dot）表示
	 * @param readable 		数据可见或不可见
	 * @param rotation 	  	（顺时针方向)0 0度,90 90 度,180 180度,270 270度
	 * @param narrow 		窄bar宽度，以点（dot）表示
	 * @param wide 			宽bar宽度，以点（dot）表示
	 * @param content 		条码文本
	 */
	virtual void drawBarcode(int x, int y, const std::string &codetype, int height, int humanreadable, int rotation, int narrow, int wide, const std::string &content);
	/**
	 * 绘制方框 单位都是dos
	 * @param X_start		  水平方向起始位置
	 * @param Y_start 		  垂直方向起始位置
	 * @param X_end  		  水平方向结束位置
	 * @param Y_end  		  垂直方向结束位置
	 * @param linethickness   方框线条粗细
	 */
	virtual void drawBox(int X_start, int Y_start, int X_end, int Y_end, int linethickness);
	/**
	 * 打印单色 BMP 图片文件
	 * @param x 			图片水平方向起始点坐标
	 * @param y 			图片垂直方向起始点坐标
	 * @param filename 		图片需要已经下载到打印缓存(有些打印机不支持)
	 */
	virtual void putBmp(int x, int y, const std::string &filename);
	/**
	 * 打印单色 PCX 格式图片文件
	 * @param x				图片水平方向起始点坐标
	 * @param y				图片垂直方向起始点坐标
	 * @param filename 		图片需要已经下载到打印缓存(有些打印机不支持)
	 */
	virtual void putPcx(int x, int y, const std::string &filename);
	/**
	 *
	 * @param x 			位图水平起始位置
	 * @param y 			位图垂直起始位置
	 * @param mode 			位图绘制模式 0 OVERWRITE 1 OR 2 XOR
	 * @param filename 		位图位置
	 * @return 				图片读取错误返回flase正确返回ture
	 */
	virtual bool drawBitmap(int x, int y, int mode, const std::string &filename);
};

}
