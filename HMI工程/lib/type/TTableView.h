/*
 * TTableView
 *
 *  Created on: 2023年8月11日
 *      Author: SIMTUTAI
 */
#pragma once

#include <cassert>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "base/Interface.h"
#include "type/TTable.h"
#include "db/IDataBase.h"

using namespace std;
using namespace stt;
using namespace stt::db;

namespace stt::data {

	/**
	 * @brief 普通变量 - 表视图
	 */
class TTableView: public TTable {
public:
	/**
	 * @HideCA
	 * @brief 构造函数
	 * @param name		名称
	 * @param tblName	对应表名(select fields from tblName)
	 * @param key		关键字段(order by key)
	 * @param label		用于list类型显示时的标签列名(用于显示的字段id)
	 * @param db		数据源，nullptr时取HMI默认数据库
	 * @param fields	字段名，以","分割，默认为空时取全部字段(select fields from tblName)
	 * @param desc		是否倒序(order by key [desc])
	 * @param condition	如sql查询条件为where k=1则contidtion="k=1"
	 */
	TTableView(const string& name, const string& tblName, const string& key
			, const string& label, IDatabase *db=nullptr, const string& fields = ""
					, bool desc = false, const string& condition="");
	/**
	 * @HideCA 析构
	 */
	virtual ~TTableView();
	/**
	 * @HideCA
	 */
	void disBindAll();

	/**
	 * @HideCA
	 * @brief 获取当前分页序号
	 * @return
	 */
	int getPageIndex();
	/**
	 * @HideCA
	 * @brief 获取总分页数
	 * @return
	 */
	int getPageCnt();
	/**
	 * @HideCA
	 * @brief 设置分页大小
	 * @param size
	 */
	void setPageSize(int size);
	/**
	 * @HideCA
	 * @brief 获取每页记录数
	 * @return
	 */
	int getPageSize();
	/**
	 * @HideCA
	 * @brief 获取总记录数
	 * @return
	 */
	int getRecordCnt();
	/**
	 * @HideCA
	 * @brief 跳转第一页
	 * @return
	 */
	void navFirst();
	/**
	 * @HideCA
	 * @brief 跳转下一页
	 */
	void navNext();
	/**
	 * @HideCA
	 * @brief 跳转上一页
	 */
	void navBack();
	/**
	 * @HideCA
	 * @brief 跳转最后一页
	 */
	void navLast();
	/**
	 * @HideCA
	 * @brief (关联变量)分页记录数
	 * @param v
	 */
	void bindPageSize(Ivariable v);
	/**
	 * @HideCA
	 * @brief (关联变量)当前页序号
	 * @param v
	 */
	void bindPageIndex(Ivariable v);
	/**
	 * @HideCA
	 * @brief (关联变量)总页数
	 * @param v
	 */
	void bindPageCnt(Ivariable v);
	/**
	 * @HideCA
	 * @brief 获取数据：从数据库中查询数据，并根据notify是否刷新控件
	 * @param notifiy	是否通知控件刷新
	 */
	void drawData(bool notifiy);
	/**
	 * @HideCA 基类已显示
	 * @brief 取变量类型
	 */
	uint8_t type();
	/**
	 * @LEVEL1
	 * @brief 重新定义数据源，用于条件查询时改变查询语句
	 * @param tbl 表名。可以用"(SELECT * FROM XX) YY"
	 * @param cond where条件部分(不要含WHERE这个词)，注意默认值是字符串"NULL"，不是指针NULL。
	 * 为"NULL"时，保持原条件不变。为空时，清除原WHERE
	 */
	void refine(const string& tbl, const string& cond="NULL");
	/**
	 * @HideCA
	 * @brief 是否启用倒序
	 */
	void enDesc(bool b);
	/**
	 * @HideCA
	 * @brief 重设数据库连接。由于U盘热插拔后，盘符变化，在U盘上的db路径就变化了
	 */
	void resetDB(IDatabase * db);
	/**
	 * @HideCA
	 * @brief 获取绑定数据的sql语句
	 * @param bpage 是否增加offset x limit x语句
	 */
	string combineSql(bool bpage);
public:
	/**
	 * @HideCA
	 * @brief 数据源
	 */
	IDatabase *m_db;
	/**
	 * @HideCA
	 * @brief 表名称
	 */
	string m_tbl;
protected:
	/**
	 * @HideCA
	 * @brief 表字段 空或XXX,XXXX,XXX
	 */
	string m_fields;
	/**
	 * @HideCA
	 * @brief 查询条件(不含WHERE)
	 */
	string m_cond;
	/**
	 * @HideCA
	 * @brief 是否倒序
	 */
	bool m_isDesc;
	/**
	 * @HideCA
	 * @brief 当前页序号
	 */
	int m_pg_ix;
	/**
	 * @HideCA
	 * @brief 页记录大小
	 */
	int m_pg_size;
	/**
	 * @HideCA
	 * @brief 总页数
	 */
	int m_pg_cnt;
	/**
	 * @HideCA
	 * @brief 总记录数
	 */
	int m_record_cnt;
	/**
	 * @HideCA
	 * @brief (关联变量)分页记录数
	 */
	Ivariable v_page_size;
	/**
	 * @HideCA
	 * @brief (关联变量)当前页序号
	 */
	Ivariable v_page_ix;
	/**
	 * @HideCA
	 * @brief (关联变量)总页数
	 */
	Ivariable v_page_cnt;

	/**
	 * @HideCA
	 * @brief 内部计算
	 */
	void internalCalc();
	/**
	 * @HideCA
	 * @brief 页面大小变化响应
	 * @param org
	 * @param data
	 */
	void onPageSizeChanged(SttObject * org, IVariable* data);
	/**
	 * @HideCA
	 * @brief 当前页变化响应
	 * @param org
	 * @param data
	 */
	void onPageIndexChanged(SttObject * org, IVariable* data);

public:
	/**
	 * @HideCA
	 * @brief 静态空表
	 */
	static TTableView* blankTable;
};
}
