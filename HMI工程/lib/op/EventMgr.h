/*
 * 事件管理
 *
 *  Created on: 2023年8月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include "base/Constants.h"
#include "base/Interface.h"
#include "widgets/STTPage.h"
#include "type/TTableView.h"

using namespace std;
using namespace stt;
using namespace stt::data;

#define _FreshDataSaveEventLog() stt::op::EventMgr::commitThenBeginTrans()
#define _ClearHistoryAlarmData() stt::op::EventMgr::deleteDB()
#if __arm__
#define _ExportHisDataToCSV() stt::op::EventMgr::exportCSV(20000101,22000101,"/vendor/udisk_sda1")
#else
#define _ExportHisDataToCSV() stt::op::EventMgr::exportCSV(20000101,22000101,"./data")
#endif

namespace stt::op{

/**
 * @brief 报警事件管理
 */
class EventMgr{
public:
	/**
	 * @LEVEL1
	 * @brief 获取事件管理器实例
	 * @return
	 */
	static EventMgr* getInstance(IRuntime * rt);
	/**
	 * @LEVEL2
	 * @brief 	绑定控件到相应的事件/报警/报警条数据源
	 * @param wTable	表格控件
	 * @param warning	是否报警表(事件为历史数据，报警为实时数据。实时数据为本次屏幕启动后的数据)
	 * @param catalog	事件/报警 的类别 0~99
	 * @param level		事件/报警 的级别 0~3
	 * @param yyyymmdd 		事件/报警 的日期
	 * @param rowcnt		事件/报警表格的行数
	 * @param autoCreateFile 如果不存在数据，是否需要创建(有写入操作时需要，查询操作时不需要)
	 * @param yyyymmdd		日期
	 * @param rowcnt		行数
	 * @param autoCreateFile	文件不存在时是否创建
	 * @param isDesc		查询时是否倒序
	 * @return 是否成功
	 */
	bool bind(STTWidget* wTable, bool warning, int catalog = 0, int level = 0
			, const string& yyyymmdd=""
			, int rowcnt=1, bool autoCreateFile=true, bool isDesc=true);
	/**
	 * @brief	与EventMgr::bind的区别
	 * 			1.日期范围(按天存储时，ymdstart有效)
	 * 			2.类别范围
	 * 			3.级别用位表示(1低级|2一般|4高级|8紧急)
	 */
	bool bind2(STTWidget* wTable, bool warning, int cata1 = 0, int cata2 = 0, int level = 0
			, const string& ymdstart="", const string& ymdend=""
			, int rowcnt=1, bool autoCreateFile=true, bool isDesc=true);
	/**
	 * @brief 在报警显示表格中，是否显示未报警的数据
	 * @note 在onload中添加代码即可EventMgr::isShowUnwarn = false;
	 */
	static bool isShowUnwarn;
	/**
	 * @brief 事件类别名称映射表
	 */
	static map<string,string> mapEventCatalog;
	/**
	 * @brief 事件级别名称映射表
	 */
	static map<string,string> mapEventLevel;
	/**
	 * @brief 是否双击确认(默认为false)
	 */
	static bool isConfirmWithDoubleClick;
	/**
	 * @brief 自定义确认过的背景颜色
	 * @string 	是eid
	 * @int		是颜色
	 */
	static map<string,int> mapBgColorConfirmed;
	/**
	 * @LEVEL2
	 * @brief 重新绑定（对于bind失败的控件，可重新尝试绑定）
	 */
	void rebind();
	/**
	 * @LEVEL1
	 * @brief 释放操作日志
	 * @return
	 */
	void release();
	/**
	 * @LEVEL1
	 * @brief 启动加载
	 * @return
	 */
	bool load();
	/**
	 * @brief 设置开累毫秒
	 */
	void setACCT(const string& eid,unsigned long long acct);
	/**
	 * @brief 设置开累次数
	 */
	void setACCE(const string& eid, int acce);
	/**
	 * @LEVEL1
	 * @brief 暂停指定事件
	 */
	void pause(const string& eid="");
	/**
	 * @LEVEL1
	 * @brief 判断指定事件是否暂停
	 */
	bool isPaused(const string& eid);
	/**
	 * @LEVEL1
	 * @brief 继续指定事件
	 */
	void resume(const string& eid="");
	/**
	 * @LEVEL1
	 * @brief 设置时间列的显示模式
	 * @param datemode 时间列的显示模式 0~时间(默认) 1~日期 2~日期+时间
	 */
	void setDateMode(int datemode);
	/**
	 * @LEVEL1
	 * @brief	设置持续时间的单位，0~ms(默认) 1~sec 2~min 3~hour
	 */
	void setDurationUnit(int unit);
	/**
	 * @brief 对事件进行确认
	 * @param name	事件EID
	 * @return
	 */
	[[deprecated("")]]
	bool confirm(const string& eid);
	[[deprecated("")]]
	void confirm2(uint16_t code, LvEvent e);
	/**
	 * @brief	点击事件表时的确认（如已有确认时间，不会重复确认）
	 */
	static void confirmEvent(TTableView* tv, const string& lid, const string& eid,const string& ext2);
	/**
	 * @brief	点击报警表时的确认（如已有确认时间，不会重复确认）
	 */
	static void confirmWarn(const string& eid,const string& ext2);
	/**
	 * @brief 	导出指定日期的数据为csv格式
	 * @param year,month,day 只对分天存储有效
	 * @note	如果是分天存储，则导出指定日期范围的数据库；若是不分天存储，则导出全部数据记录
	 * @param csvpath	csv文件完整路径
	 */
	static void exportCSV(int year,int month, int day,const string& csvpath);

	/**
	 * @brief  导出数据为csv格式，存储到U盘
	 * @param dircsv	目录路径，最后不用"/"结尾。如放在U盘根目录，则为""
	 * @param msg..		自定义导出提示信息
	 * @note	如dircsv="event"，则导出到U:/event/下
	 */
	void exportCSV(const string& dircsv
			, const string& msg1="正在导出csv..."
			, const string& msgFail="导出csv文件失败!"
			, const string& msgOK="导出csv文件成功!");
	/**
	 * @LEVEL0
	 * @brief 导出事件记录eventlog.db导出到"U盘/eventlog.csv"。注意适用于不是按天存的
	 * @note 如需修改提示信息，请使用：EventMgr::getInstance(getRuntime())->exportCSV("","正在导出信息","导出失败信息","导出成功信息");
	 */
	static void exportCSV();
	/**
	 * @brief 	删除指定日期的数据
	 * @note 	1.如果是分天存储，则删除指定日期的数据库；否则删除指定日期的记录
	 * 			2.如果是分天存储，不能删除今天的（因为今天正在进行事件采集）
	 */
	static bool deleteDB(int year,int month, int day);
	/**
	 * @brief 	删除指定日期范围的数据,如需删除全部数据，直接调用deleteDB()
	 * @note 	如果是分天存储，则删除指定日期的数据库；否则删除指定日期的记录
	 * @return 	返回值无意义
	 */
	static int deleteDB(int ymdstart=20000101,int ymdend=22000101);
	/**
	 * @HideCA
	 * @brief  删除超过设定期限的db文件。
	 */
	bool deleteExpiredDB();
	/**
	 * @HideCA
	 * @brief	提交当前事件数据库事务并重启事务
	 */
	static void commitThenBeginTrans();
	/**
	 * @HideCA
	 * @brief	把m_datus里的所有TTableView.drawData一
	 * @param	warn	是否报警表的TableView
	 */
	void drawData(bool warn);
public:
	map<string, void*> mapFont;//直接取到指针，而不用再通过FONT串去计算
	map<string, uint32_t>mapBgColor;//直接取到背景色，而不再通过COLR串去计算
	map<string, uint32_t>mapFgColor;//直接取到文字色，而不再通过FONT串去计算
	/**
	 * @HideCA
	 * @brief 不同控件对应不同的TableView: yyyymmdd_x_y_z
	 * string: A_yyyymmdd_B_C_D
	 * 	A:是否报警[0,1] B:类别[0..99] C:级别[0..3] D:行数[1...]
	 */
	map<string, shared_ptr<TTableView>> m_datus;
	/**@HideCA 设置当前要确认的EID (弹出确认框前设置一下)*/
	void setConfirmEID(string eid);
	/**@HideCA 获取当前要确认的EID (点击确认时获取被确认的EID)*/
	string getConfirmEID();
	/**@HideCA 获取指定事件的文字颜色*/
	LvColor getForeColor(string eid);
	/**@HideCA 获取指定事件的背景颜色*/
	LvColor getBackColor(string eid);
	/**@HideCA 获取指定事件的字体*/
	void* getFont(string eid);
	/**@HideCA 获取指定事件的状态（true为发生，false为复位）*/
	bool getStat(string eid);
	/**
	 * @brief 获取日志数据库eventlog.db连接指针
	 * @param yyyymmdd 日期（。对原来按分天存储方式有效）
	 * @param autoCreateFile 是否创建db文件，设为false阻止自动创建db文件
	 * （sqlite在连接时，发现文件不存在，则会自动创建。一般在写入时用true，读取时用false）
	 */
	static IDatabase* getdb(string yyyymmdd="", bool autoCreateFile=true);
	static void destroy(IRuntime * rt) ;
protected:
	/**
	 * @HideCA
	 */
	IRuntime * m_rt;
private:
	/**
	 * @HideCA
	 */
	EventMgr(IRuntime * rt);
	/**
	 * @HideCA
	 */
	~EventMgr();
	/**
	 * @HideCA
	 * @brief EVENTLOG中，本次启动前的最大值，用于报警表（报警表为大于LastID的新数据）
	 */
	int lastLID;
public:
	/**
	 * @HideCA
	 * bind时把失败的bind丢进这里，在refresh的时候再尝试bind一把
	 */
	std::queue<std::function<void(void)>> vbinds;

public:
	unsigned long long m_lastCommitTime;
	int8_t dateMode;//日期模式 0~时间 1~日期 2~日期+时间，默认为2
	bool isDropView;//日期模式变化时，要删掉原视图，下次保存数据时，会自动重建视图
	uint32_t msSaveInterval;//数据存盘间隔时间，即提交事务间隔，默认1分钟(无用：由于用了线程每隔定时60秒提交)
	int8_t durationUnit;//持续时间单位 0~ms 1~sec 2~min 3~hour
	IDialogPtr msg;			//提示对话框
	string csvpath;//保存cav的文件夹
	bool exportResult;
	atomic_bool bTouch;
};

/**
 * @brief 定时任务
 */
class TimerMgr{
public:
	/**
	 * 获取定时任务管理器实例
	 * @return
	 */
	static TimerMgr* getInstance(IRuntime * rt);
	/**
	 * @brief添加定时任务
	 * @param name		名称
	 * @param defs		配置
	 * @param handler	处理函数
	 * @return	0~OK 1~配置错误 2~重复
	 *
	 * @note 配置说明
	 * tmod:0-周期 1-触发
	 * triv:解发变量(tmod=1时有效)
	 * tmcy:tmod=0时表示周期[ms],tmod=1时表示触发条件(1-小于 2-大于 3-等于 4-不等于 5-小于或等于 6-大于或等于 7-ON->OFF 8-OFF->ON)
	 * udyn:是否使用动态参数，值范围[0或1]，触发模式时有效
	 * dynv:动态参数,取值为cron表达式或变量名
	 * inbg:是否后台运行，默认为否
	 */
	int add(const string& name, const string& defs, SttObject* obj, EHandler handler);
	/**
	 * @brief删除定时任务
	 * @param name		名称
	 * @return 0~OK 1~不存在
	 */
	int remove(const string& name);
	/**
	 * 获取定时任务
	 * @param name		名称
	 * @return			定时任务对象
	 */
	STTTimer* get(const string& name);
	/**
	 * 删除全部任务
	 */
	void removeAll();

private:
	TimerMgr(IRuntime * rt);
	~TimerMgr();
};

/**
 * @brief OTA操作接口类
 * 该类提供了一系列与OTA（Over-the-Air）操作相关的功能，包括获取实例、配置、下载、更新、取消下载以及获取OTA状态等。
 */
class OTAEvent{
public:
	static OTAEvent* getInstance(stt::IRuntime * rt);
	//打开指令
	void Config_Event();
	//下载更新包
	void Download_Event();
	//更新（重启）
	void Upgrade_Event();
	//取消下载
	void Cancel_Event();
	/**
	 * @LEVEL1
	 * @brief 获取OTA状态
	 * @return 0:运行 -1:OTA开关未开 -2:服务器url为空
	 */
	int getUpgrade();
private:
	/** @HideCA	 */
	OTAEvent(IRuntime * rt);
	/** @HideCA	 */
	~OTAEvent();
    static OTAEvent* OtaEvent_stance;
};

}
