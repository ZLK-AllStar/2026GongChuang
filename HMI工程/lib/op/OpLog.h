/*
 * 操作日志
 *
 *  Created on: 2023年8月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include "base/Constants.h"
#include "base/Interface.h"

using namespace std;
using namespace stt;

namespace stt::op{

/**
 * @brief 日志管理器
 */
class OpLogMgr{
public:
	/**
	 * 获取日志管理器实例
	 * @return
	 */
	static OpLogMgr* getInstance(IRuntime * rt);
	/**
	 * 导出日志记录到csv(同步方式)
	 *
	 * @param path	csv文件路径
	 * @return
	 */
	static bool exportAs(const string& path);
	/**
	 * 导出日志记录到csv(异步方式)
	 *
	 * @param path	csv文件路径
	 * @return
	 */
	static void exportAsAsync(const string& path);
	/**
	 * @LEVEL1
	 * @brief 清理操作日志，不会刷新显示控件，需显式调用控件更新方法，如navFirst
	 */
	static void clear();
	/**
	 * @LEVEL1
	 * @brief 刷新(因操作日志数据并非实事保存，而是每60秒保存一次，可调用refresh立即保存
	 * ，也需显式调用控件更新方法，如navFirst)
	 */
	void refresh();
public:
	/**
	 * 新增操作日志
	 * @param page		页面
	 * @param widget	控件名
	 * @param action	动作
	 * @param var		变量名
	 * @param info		消息
	 */
	void add(const string& page,const string& widget, const string& action, const string& var, const string& info);
	/**
	 * 暂停
	 * @return
	 */
	void pause();
	/**
	 * 继续
	 * @return
	 */
	void resume();
	/**
	 * @brief 	日志数据输出到显示表格
	 * @param wTable	显示表格
	 * @return 是否成功
	 */
	bool bind(STTWidget* wTable,bool isDesc=true);

protected:
	void* mm;

private:
	OpLogMgr(IRuntime * rt);
	~OpLogMgr();
};

}
