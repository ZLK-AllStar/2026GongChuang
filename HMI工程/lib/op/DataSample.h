/*
 * 数据采样
 *
 *  Created on: 2023年8月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include "base/Constants.h"
#include "base/Interface.h"
#include "widgets/STTWidget.h"
#include "type/TTable.h"

using namespace std;
using namespace stt;
using namespace stt::data;

namespace stt::op{

/**
 * @brief 数据采样
 * 单例模式
 */
class SampleMgr{
public:
	/**
	 * @LEVEL0
	 * @brief 获取采样管理器实例
	 * @return
	 */
	static SampleMgr* getInstance(IRuntime * rt);
	/**
	 * @brief 获取全部采样定义[SID, SNAME, STBL]
	 * @return 返回TTable
	 */
	static TTable allSamples();
	/**
	 * @LEVEL1
	 * @brief 暂停采样任务
	 * @param sid	默认空(全部)
	 * @return
	 */
	static bool pause(const string& sid="");
	/**
	 * @LEVEL1
	 * @brief 继续采样任务
	 * @param sid	默认空(全部)
	 * @return
	 */
	static bool resume(const string& sid="");
	/**
	 * @brief 删除指定id及日期范围的采样数据库, 与exportCSV对应
	 * @param sids		要删除的采样id,多个可用逗号分隔，如"1,2,5"，为空时删除全部
	 * @param ymdstart	要删除的起始日期(含)，格式：yyyymmdd
	 * @param ymdend	要删除的截止日期(含)，格式：yyyymmdd
	 * @return 返回删除的文件数
	 * @note 	单文件存储时，删除范围内的记录
	 * 			多文件存储时，删除范围内的文件
	 */
	static int deleteDB(const string& sids, int ymdstart,int ymdend);
	/**
	 * @brief 获取上次导出状态
	 * @usage
	 * for(auto &it : SampleMgr::getExportResult()) {
		cout<<it.first<<" result is: "<<it.second<<endl;
	 * @return 返回列表<文件名,结果>. 结果为1表示成功，其他不成功。
	}
	 */
	static map<string,string> getExportResult();
	/**
	 * @brief 是否导出正在进行中
	 */
	static bool isExporting();
	/**
	 * @HideCA
	 * @brief  删除超过设定期限的db文件。(内部使用)
	 * @usage SampleMgr::deleteExpiredDB("2");
	 */
	static bool deleteExpiredDB(const string& sid);
	/**
	 * @HideCA
	 * @brief 删除超过设定期限的db文件。(内部使用)
	 */
	static void deleteExpiredDB();
	/**
	 * @HideCA
	 * @brief 销毁静态变量，AppRuntime析构时调用
	 */
	static void destroy(IRuntime * rt);
	/**
	 * 锁定采样Table，刷新控件时调用
	 * @param sid，采样id
	 */
	static void lock(const string& sid);

	/**
	 * 解锁采样Table，刷新控件时调用，
	 * @param sid，采样id
	 */
	static void unlock(const string& sid);
	static void locknav(const string& sid);
	static void unlocknav(const string& sid);
	/**
	 * @brief 设定采样变量的因数及偏移量(仅对指定的采样有效)
	 * @note 采样结果 = 实际值 * factor + offset
	 */
	static void setFactorAndOffset(const string& sid, const string& varname, double factor, double offset);
public:
	/**
	 * @LEVEL1
	 * @brief 	获取采样数据
	 * 			如指定的采样不存在，会创建新的TTableView，最后要用release释放
	 * @param name 指定采样的id
	 * @param yyyymmdd 指定日期
	 * @param rowcnt 绑定表格或chart的数据量
	 * @param isrt 是~实时采样, 否~历史采样(是，则取内存数据作为数据源，否，则从数据库取数据作为数据源)
	 */
	TTable* get(const string& sid,const string& yyyymmdd, int rowcnt, bool isrt=true, bool isDesc=true);
	/**
	 * @HideCA
	 * @brief	启动加载采样任务, 程序启动时调用一次
	 * @param 	delay 延时启动秒数
	 */
	bool load(int delay = 5);
	/**
	 * @LEVEL1
	 * @brief 	采样数据输出到显示控件，绑定失败将无法刷新对应控件。
	 * @param widget	显示控件
	 * @param sid 		采样id
	 * @return 是否成功
	 */
	bool bind(STTWidget* widget, const string& sid, const string& yyyymmdd="", int rowcnt=0, bool isrt=true, bool isDesc=true);
	/**
	 * @HideCA
	 * @brief 导出指定id及日期范围的采样数据到csv
	 * @param sids 多个采样id用逗号分隔，如 "1,2,3"。为空时，表示全部
	 * @param ymdstart 要导出的开始日期yyyyMMdd(从0:0:0开始)
	 * @param ymdend 要导出的结束日期yyyyMMdd(到23:59:59结束)
	 * @param dircsv 导出csv的目录，默认为U盘根目录(导出文件名不变，扩展名为csv)
	 * @param isudisk 是则u盘，否则sd卡
	 * @note 如全部导出到U盘目录的"export下"：exportCSV("",0,99999999,"export");注意要确保目录存在
	 * 	     模拟器中导出在【数据】目录下，而不是U盘
	 * 	     初次采样要采1分钟以上才会有数据(每采样1分钟，存盘1次)
	 */
	void exportCSV(const string& sids,int ymdstart,int ymdend, const string& dircsv="", bool udisk=true);
private:
	/** @HideCA	 */
	SampleMgr(IRuntime * rt);
	/** @HideCA	 */
	~SampleMgr();
	/** @HideCA	 */
	IRuntime * m_rt;

};

/**
 * @brief 清除采样、事件历史数据类
 */
interface ExportSampleAndEventHelper : public SttObject, implements IVarOperate {
public:
	ExportSampleAndEventHelper(IRuntime * rt, int period);
	virtual ~ExportSampleAndEventHelper();
	void sendEvent(uint8_t e, void * ev);
	void sendTick(uint64_t tick);
	uint8_t type();
	uint8_t event();
public:
	uint32_t period;			// 时间周期 (ms)
	IRuntime* m_rt;				// pm
	uint64_t stamp;				// 时间戳 (ms)
};

}
