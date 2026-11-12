/*
 * RCP.h
 *
 *  Created on: 2023年8月14日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include "base/Constants.h"
#include "base/Interface.h"
#include "type/Trcp.h"

using namespace std;
using namespace stt;
using namespace stt::data;

namespace stt::op{

/**
 * @brief 配方管理
 */
class RcpMgr{
public:
	/**
	 * 获取全部配方定义
	 * @return
	 */
	static TTable allRecipes();
	/**
	 * 新增配方
	 * @param name		配方名称
	 * @param defs		定义字段名，用逗号分隔，如"字段名1,字段名2,..."
	 * @param vars		变量设置，变量之间用逗号分隔，字段与变量之间用冒号分隔。如"字段名1:变量名1,字段名2:变量名2,...."
	 * @return	0:成功   1:已存在 	2:参数错误
	 */
	static int add(const string& name, const string& defs, const string& vars);
	/**
	 * 删除配方定义
	 *
	 * @param id
	 * @return
	 */
	static bool remove(const string& name);
	/**
	 * @brief 获取或根据配方配置信息创建配方变量
	 * @param name
	 * @return
	 */
	static TRecipe* get(const string& name);
	/**
	 * @brief 	日志数据输出到显示表格
	 * @param wTable	显示表格
	 * @param name		配方名称
	 * @return 是否成功
	 */
	static bool bind(STTWidget* wTable, const string& name);
	/**
	 * 导出配方
	 * @param name	配方名称
	 * @param path	输出路径
	 * @return
	 */
	static bool exportAs(const string& name, const string& path);
	/**
	 * 导入配方
	 * @param name	配方名称
	 * @param path	输入路径
	 * @return
	 */
	static bool importFrom(const string& name, const string& path);
};
}

