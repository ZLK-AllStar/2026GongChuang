/*
 * FILEUtil.h
 *
 *  Created on: 2023年8月21日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "type/TTable.h"
#include "widgets/STTPage.h"

using namespace std;
using namespace stt;
using namespace stt::data;

namespace stt::util{
/**
 * @brief 文件类
 */
class FileUtil{
public:
	/**
	 * @LEVEL0
	 * @brief 将sqlite数据库中的表导出为csv文件
	 * 			数据量较大时，需配合线程处理
	 * @param pathdb sqlite数据库文件路径
	 * @param tbname 指定表名
	 * @param pathcsv 导出csv文件路径
	 * @param err	错误信息
	 * @return true~成功；false~失败
	 */
	static bool exportDB2CSV(const string& pathdb,const string& tbname, const string& pathcsv, string& err);
	/**
	 * @LEVEL0
	 * @brief 将sqlite数据库中的表导出为csv文件
	 * 			数据量较大时，需配合线程处理
	 * @param pathdb sqlite数据库文件路径
	 * @param tbname 指定表名
	 * @param sql 对tbname表的查询语句。如果为空，表示tbname所有数据
	 * @param pathcsv 导出csv文件路径
	 * @param err	错误信息
	 * @return true~成功；false~失败
	 */
	static bool exportDB2CSV(const string& pathdb,const string& tbname, const string& sql, const string& pathcsv, string& err);
	/**
	 * @LEVEL0
	 * @brief 将sqlite数据库中的表导出为csv文件
	 * 			数据量较大时，需配合线程处理
	 * @param pathdb sqlite数据库文件路径
	 * @param tbname 指定表名
	 * @param sql 对tbname表的查询语句。如果为空，表示tbname所有数据
	 * @param pathcsv 导出csv文件路径
	 * @param err	错误信息
	 * @param isAsync 是否异步
	 * @return true~成功；false~失败(如果是异步，直接返回true)
	 * @example
	 * 	模拟器:
	 * 	FileUtil::exportDB2CSV("./data/SAMPLE_5.db", "SAMPLE_5_V", "SELECT * FROM SAMPLE_5_V WHERE STIME>'2025-01-01'", "d:/1.csv", err, true,"ok","fail");
	 * 	屏:
	 * 	FileUtil::exportDB2CSV("/customer/usr/samples/SAMPLE_5.db", "SAMPLE_5_V", "SELECT * FROM SAMPLE_5_V WHERE STIME>'2025-01-01'", "/vendor/udisk_sad1/1.csv", err, true,"ok","fail");
	 */
	static bool exportDB2CSV(const string& pathdb,const string& tbname, const string& sql
			, const string& pathcsv, string& err, bool isAsync
			, const string& strOK="导出成功！", const string& strFail="导出失败");
	/**
	 * @HIDECA
	 * @brief	获取导出的目录，ARM下只能是U盘根目录，模拟器下只能是./data，子目录无效
	 * @param	subdir 子目录，1层(如不存在，则会创建。若是多层子目录则不会创建，要确保目录存在。)
	 * @note	如果U盘不存在，返回""，否则返回 "/vendor/udisk_sdxy/subdir/"
	 */
	static string getExportDir(const string& subdir);

	/**
	 * @LEVEL0
	 * @brief 将表格导出到文件(csv)
	 * @param table
	 * @param path
	 * @return	0:成功  其它：失败
	 */
	static int exportTableToFile(TTable& table, const string& path, bool overwrite=true);
	/**
	 * @LEVEL0
	 * @brief 从csv导入数据到表格
	 * @param table
	 * @param path
	 * @return	0:成功  其它：失败
	 */
	static int importTableFromFile(TTable& table, const string& path);
	/**
	 * @LEVEL0
	 * @brief 文件是否存在
	 * @param filePath	文件路径
	 * @return	是否存在
	 */
	static bool fileExists(const string& filePath);
	/**
	 * @LEVEL0
	 * @brief 目录是否存在
	 * @param path	目录路径
	 * @return	是否存在
	 */
	static bool pathExists(const string& path);
	/**
	 * @LEVEL0
	 * @brief 创建文本格式文件并写入内容
	 * @param filePath	完整文件路径名称
	 * @param content	写入内容
	 * @param mode		0: 覆盖  1：追加
	 * 					如果文件不存在则创建模式不起作用；
	 * 					如果存在，则模式设置起作用
	 * @return	是否成功
	 */
	static bool createTxtFile(const string& filePath, const string& content, int mode=0);
	/**
	 * @LEVEL0
	 * @brief 读取文本格式文件到字符串
	 * @param filePath	完整文件路径名称
	 * @param res		存储结果用字符串
	 * @return	是否成功
	 */
	static bool readTxtFile(const string& filePath, string& res);
	/**
	 * @LEVEL0
	 * @brief 创建目录
	 * @param path	目录路径
	 * @return	是否成功
	 */
	static bool createPath(const string& path);
	/**
	 * @LEVEL0
	 * @brief 判断目录对象类型
	 * @param path	目录/文件完整路径
	 * @return	-1:失败 0:文件 1:目录 2:其它
	 */
	static int isPathOrFile(const string& path);
	/**
	 * @LEVEL0
	 * @brief 获取目录下内容(包含子目录)
	 * @param path	目录完整路径
	 * @param res	返回结果(KV结果,K=文件/目录名称, V存放父目录名)
	 * @return	是否成功
	 */
	static bool getPathContent(const string& path, vector<KV_Str_Str>& res);
	/**
	 * @LEVEL0
	 * @brief 删除文件
	 * @param filePath	文件完整路径
	 * @return	是否成功
	 */
	static bool removeFile(const string& filePath);
	/**
	 * @LEVEL0
	 * @brief 删除目录下文件名符合正则表达式的文件
	 * @param dir	文件夹路径
	 * @param ret	正则表达式
	 * @return	是否成功
	 */
	static void removeFiles(const string& dir, const string& reg);
	/**
	 * @LEVEL0
	 * @brief 删除目录
	 * @param path	目录完整路径
	 * @return	是否成功
	 */
	static bool removePath(const string& path);
	/**
	 * @LEVEL0
	 * @brief 删除目录下所有内容
	 * @param path	目录完整路径
	 * @return	是否成功
	 */
	static bool clearPath(const string& path);
	/**
	 * @LEVEL0
	 * @brief 将文件拷贝到指定目录，新文件名称为"时间戳后8位-原文件名"。注意模拟器下路径分隔符使用"/"
	 * @param srcfile		文件完整路径，注意路径分隔符为"/"
	 * @param dstdir		目录路径
	 * @return	是否成功
	 */
	static bool copyFile(const string& srcfile, const string& dstdir);
	/**
	 * @LEVEL0
	 * @brief 文件复制
	 * @param srcfile	源文件路径
	 * @param dstfile	目标文件路径
	 */
	static bool copyFile2(const string& srcfile, const string& dstfile);
	/**
	 * @LEVEL0
	 * @brief 将目录下所有内容拷贝到指定路径（会递归复制子目录）
	 * @param src		源目录路径
	 * @param target	目录路径
	 * @return	是否成功
	 */
	static bool copyPath(const string& src, const string& target);
	/**
	 * @LEVEL0
	 * @brief 获取目录空间大小信息
	 * @param path		目录路径
	 * @param total		总大小(MB)
	 * @param free		剩余大小(MB)
	 * @return	0：执行成功  其它：失败
	 */
	static int getPathSpace(const string& path, float& total, float& free);
	/**
	 * @LEVEL0
	 * @brief 获取目录下内容,只包含当前目录，不包含子目录及其内容
	 * @param path	目录完整路径
	 * @param res	返回结果(KV结果,K=文件/目录名称, V存放父目录名)
	 * @return	是否成功
	 */
	static bool getPathFile(const string& path, vector<KV_Str_Str>& res);
};

/**
 * @brief PDF文件处理类
 */
class PDFUtil {
public:
	/**
	 * @LEVEL0
	 * @brief PDFUtil构造
	 * @param xpdfdir	pdftopng可执行文件所在目录，如"D:/pdftool/xpdf"。
	 * 	确保有pngtopdf.exe, pngfonts.exe, pnginfo.exe及配置文件xpdfrc，以及两个字体相关的目录xpdf-t1fonts、xpdf-chinese-simplified
	 * @param filepath	要打开的pdf文件
	 * @param prefix	输出png文件名称的前缀，可含路径。如"d:/png",若ipage=12, 则转换后将输出为d:/png-000012.png
	 * @param panel		面板控件
	 */
	PDFUtil(const string& xpdfdir, const string& filepath, const string& prefix, STTPanel* panel);
	~PDFUtil();
	/**
	 * @LEVEL0
	 * @brief 显示指定页
	 */
	void gotoPage(int ipage);
	/**
	 * @LEVEL0
	 * @brief 显示第1页
	 */
	void first();
	/**
	 * @LEVEL0
	 * @brief 显示最后1页
	 */
	void last();
	/**
	 * @LEVEL0
	 * @brief 显示前1页
	 */
	void back();
	/**
	 * @LEVEL0
	 * @brief 显示下1页
	 */
	void next();
	/**
	 * @LEVEL0
	 * @brief 旋转90度
	 */
	void rotate();
	/**
	 * @LEVEL0
	 * @brief 缩放指定倍数。若放大后的内存需求大于4M，则不放大。
	 */
	void zoom(float scale);
private:
	string m_xpdfdir;//xpdf工具目录
	string m_filepath;//PDF文件路径
	string m_prefix;//输出png的前缀
	int m_page;//当前显示页码[1..n]
	int m_pageCount;//总页数
	int m_rotate;//旋转角度[0,90,180,270]
	float m_zoom;//当前放大例
	STTPanel* m_panel;//显示pdf的控件
//静态方法
public:
	/**
	 * @HideCA
	 * @brief PDF文件的某一页转PNG  注意转换前删除上次转换的png文件，以防空间不足
	 * @param tooldir	pdftopng可执行文件所在目录，如"D:/pdftool/xpdf"
	 * @param ipage		转换pdf文件的第几页[1..n]，如12
	 * @param pdfpath	pdf文件绝对路径，如"D:/pdftool/xpdf/V2.1.pdf"
	 * @param prefix	输出png文件名称的前缀，可含路径。如"d:/png",若ipage=12, 则转换后将输出为d:/png-000012.png
	 * @param rot		旋转角度[0,90,180,270]
	 * @return 返回文件名，如"d:/png-000012.png"。如prefix含路径，则返回文件路径，否则返回文件名(相对目录为程序所在文件夹)
	 */
	static string toPng(const string& tooldir, int ipage, const string& pdfpath, const string& prefix, int rot=0);
	/**
	 * @HideCA
	 * @brief PDF文件的某一页所用到的字体<name,type>
	 * @param tooldir	pdftopng可执行文件所在目录，如"D:/pdftool/xpdf"
	 * @param ipage		转换pdf文件的第几页[1..n]，如12
	 * @param pdfpath	pdf文件绝对路径，如"D:/pdftool/xpdf/V2.1.pdf"
	 * @return 返回字体名称与类型的键值对列表。如：<ArialMT, TrueType><ACHRDX+Marlett, CID TrueType>
	 */
	static vector<KV_Str_Str> getFontNameAndType(const string& tooldir, int ipage, const string& pdfpath);
	/**
	 * @HideCA
	 * @brief 针对pdf某一页，更新xpdf的配置文件xpdfrc，主要是更新一些fontFile项
	 * @param tooldir  xpdf的配置文件所在目录，如"D:/pdftool/xpdf"
	 * @param vfont 字体的名称与类型的键值对，见getFontNameAndType的返回
	 */
	static bool updateXpdfCfg(const string& tooldir, vector<KV_Str_Str> vfont);
	/**
	 * @HideCA
	 * @brief	用pdfinfo工具获取信息 pdfinfo xxx.pdf
	 * @param tooldir  xpdf的配置文件所在目录，如"D:/pdftool/xpdf"
	 * @param pdfpath	pdf文件绝对路径，如"D:/pdftool/xpdf/V2.1.pdf"
	 * @return 包含：Title,Subject,Keywords,Author,Creator,Producter,CreationDate,ModDate,Tagged,Form,Pages
	 * Encrypted, Page size, File size, Optimized, Javascript, PDF version
	 */
	static vector<KV_Str_Str> getInfo(const string& tooldir, const string& pdfpath);
	/**
	 * @HideCA
	 * @brief 获取pdf文件的总页数
	 * @param tooldir  xpdf的配置文件所在目录，如"D:/pdftool/xpdf"
	 * @param pdfpath	pdf文件绝对路径，如"D:/pdftool/xpdf/V2.1.pdf"
	 */
	static int getPageCount(const string& tooldir, const string& pdfpath);
};
}
