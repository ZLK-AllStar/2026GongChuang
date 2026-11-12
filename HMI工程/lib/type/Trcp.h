/*
 * Trcp.h
 *
 *  Created on: 2023年2月13日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "TTable.h"
#include "TTableView.h"

using namespace stt;
using namespace stt::data;

namespace stt::data{
	/**
	 * @brief 普通变量 - 配方型
	 */
class TRecipe : public TTable{
public:
	/**
	 * 配方变量
	 * @param vm		变量管理器
	 * @param id		变量名称，所有变量不可重复
	 * @param rtbl		表名
	 * @param key		关键字段
	 * @param label		标签字段
	 * @param db		数据源
	 */
	TRecipe(IVarManager * vm, const string& id, const string& rtbl, const string& key="", const string& label="", IDatabase *db=nullptr);
	~TRecipe();
	/**
	 * @HideCA 基类已显示
	 */
	uint8_t type();
	/**
	 * @HideCA
	 * @brief 设置列的关联变量
	 * @param colId		列ID
	 * @param var		变量名称
	 * @return
	 */
	bool setColumnVariant(const string& colId, const string& var);
	/**
	 * @HideCA
	 * @brief 设置列的关联变量
	 * @param vars
	 * @return
	 */
	bool setColumnVariants(vector<KV_Str_Str>& vars);
	/**
	 * @HideCA
	 * @brief 获取列变量的类型
	 * @param colId		列
	 * @return			返回变量类型
	 */
	int getColumnVariantType(const string& colId);
	/**
	 * @HideCA
	 * @brief 配方确认
	 * @param sel
	 */
	bool confirm();
	/**
	 * @HideCA
	 * @brief 拷贝指定行
	 * @param pos	行号
	 * @return
	 */
	bool copyRow(int pos=-1);
	/**
	 * @HideCA
	 * @brief 交换行
	 * @param f		第一行
	 * @param s		第二行
	 * @return
	 */
	bool swapRow(int f, int s);
	/**
	 * @HideCA
	 * @brief 写入数据库
	 * @return
	 */
	bool save();
	/**
	 * @HideCA
	 * @brief 重新加载数据
	 * @param notifiy
	 */
	void drawData(bool notifiy);
public:
	/**
	 * @HideCA
	 */
	string m_tbl;				//表名称
	/**
	 * @HideCA
	 */
	IDatabase *m_db;			//数据源
protected:
	/**
	 * @HideCA
	 */
	vector<KV_Str_Str> m_vars;
};

}

