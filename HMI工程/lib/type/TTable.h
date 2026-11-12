/*
 * TTable.h
 *
 *  Created on: 2023年8月11日
 *      Author: SIMTUTAI
 */
#pragma once
#include <atomic>
#include <cassert>
#include <cstdint>
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include "base/Interface.h"

using namespace stt;
using namespace std;

namespace stt::data
{

/**
 * @brief 普通变量 - 表格型
 */
class TTable : public IVariable {
public:
	/**
	 * @HideCA
	 */
	struct Column{
		string id;		//字段
		uint8_t type;	//列数据类型
		string def;		//默认值
		string ext1;	//扩展1
	};
	/**
	 * @HideCA
	 */
	struct Row{
		vector<Variant> values;
	};
public:

	/**
	 * @HideCA
	 * @param name	名称
	 * @param key	键值字段
	 * @param label 标签字段
	 */
	TTable(const string& name = "", const string& key="", const string& label = "");
	/**
	 * @HideCA
	 */
	virtual ~TTable();
    /**
     * @LEVEL0
     * @brief 添加列并赋初值
     * @param col	列的id
     * @param value	增加行时该列的默认值
     */
	virtual void addColumn(const string& col, const string& value="", const uint8_t& type=0);
	/**
	 * @LEVEL0
     * @brief 设置列并赋初值
	 * @param colindex	序号
	 * @param colname	列名
	 * @param defvalue	初值
	 */
	virtual void setColumn(int colindex, const string& colname, const string& defvalue="");
    /**
     * @LEVEL0
     * @brief 检查列名是否存在并返回列的序号
     * @param column_name
     * @return 列的序号 >=0:OK  <0：不包含
     */
	virtual int hasColumn(const string& columnName) const;
    /**
     * @LEVEL0
     * @brief 获取列名
     * @param col		列索引号
     * @return 不存在, 返回空
     */
	virtual string getColumnName(int col);
    /**
     * @LEVEL0
     * @brief 返回列名
     * @return
     */
	virtual vector<string> getColumnNames();
    /**
     * @LEVEL0
     * @brief 获取指定列中值列表
     * @param col	指定列的id
     * @param values	存储该列所有行的值
     * @return	不存在指定列，返回false. 否则返回true
     */
	virtual bool getColumnValues(const string& col, vector<string>& values);
	virtual bool getColumnValues(const string& col, vector<Variant>& values);
    /**
     * @LEVEL0
     * @brief 获取列数
     */
	virtual int getColumnCount();
    /**
     * @LEVEL0
     * @brief 删除指定列
     * @param col 列名
     * @return	不存在指定列，返回false. 否则返回true
     */
	virtual bool delColumn(const string& col);
	/**
	 * @LEVEL0
     * @brief 删除所有列
	 * @return
	 */
	bool delColumnAll();
	/**
	 * @LEVEL0
	 * @brief 在尾部简单新增行
	 * @param str	字符串
	 * @param split	分隔符
	 */
	virtual void addRow(const string& str="", const string& split=",");
    /**
     * @LEVEL0
     * @brief 在尾部简单新增行
     * @param row	值列表
     */
	virtual void addRow(vector<string>& row);
    /**
     * @LEVEL0
     * @brief 在尾部简单新增行
     * @param row	值列表
     */
	virtual void addRow(vector<Variant>& row);
	/**
	 * @LEVEL0
	 * @brief 在尾部新增行
	 *  	新增时比较列ID与新增行参数中键值
	 * @param row	值列表
	 */
	virtual void addRow(vector<KV_Str_Str>& row);
    /**
     * @LEVEL0
     * @brief 在指定位置简单新增行
     * @param pos 插入位置的索引
     * @param row 值列表
     */
	virtual bool insertRow(int pos, vector<string>& row);
    /**
     * @LEVEL0
     * @brief 在指定位置简单新增行
     * @param pos 插入位置的索引
     * @param row 值列表
     */
	virtual bool insertRow(int pos, vector<Variant>& row);
	/**
	 * @LEVEL0
	 * @brief 在指定位置新增行
	 * 		新增时比较列ID与新增行参数中键值
	 * @param pos 插入位置的索引
	 * @param row 值列表
	 */
	virtual bool insertRow(int pos, vector<KV_Str_Str>& row);
	/**
	 * @LEVEL0
	 * @brief 删除指定行
	 * @param rowIndex 行索引
	 * 		如果指定行<0的数，则删除全部
	 * @return
	 */
	virtual bool delRow(int rowIndex);
	/**
	 * @LEVEL0
	 * @brief 删除所有行
	 */
	virtual void delAllRow();
    /**
     * @LEVEL0
     * @brief 返回指定行数据
     * @param rowIndex	行号 0...n
     * @return
     */
	virtual vector<string> getRow(int rowIndex);
	virtual vector<Variant> getRowValues(int rowIndex);
    /**
     * @LEVEL0
     * @brief 获取指定行列值
     * @param rowIndex  行号：0...n
     * @param colName	列名
     * @param res		返回值
     * @return
     */
	virtual string getValue(int rowIndex, const string& colName);
    /**
     * @LEVEL0
     * @brief 获取指定行指定列值
     * @param rowIndex  行号：0...n
     * @param colIndex	列号：0...n
     * @param res		返回值
     * @return
     */
	virtual string getValue(int rowIndex, int colIndex);
    /**
     * @LEVEL0
     * @brief 获取指定行指定列值
     * @param rowIndex  行号：0...n
     * @param colIndex	列号：0...n
     * @param value		返回值
     * @return
     */
	virtual void getValue(int rowIndex, int colIndex, bool& value);
	virtual void getValue(int rowIndex, int colIndex, int8_t& value);
	virtual void getValue(int rowIndex, int colIndex, uint8_t& value);
	virtual void getValue(int rowIndex, int colIndex, int16_t& value);
	virtual void getValue(int rowIndex, int colIndex, uint16_t& value);
	virtual void getValue(int rowIndex, int colIndex, int32_t& value);
	virtual void getValue(int rowIndex, int colIndex, uint32_t& value);
	virtual void getValue(int rowIndex, int colIndex, int64_t& value);
	virtual void getValue(int rowIndex, int colIndex, uint64_t& value);
	virtual void getValue(int rowIndex, int colIndex, float& value);
	virtual void getValue(int rowIndex, int colIndex, double& value);
	virtual void getValue(int rowIndex, int colIndex, string& value);
    /**
     * @LEVEL0
     * @brief 设置指定行指定列的值
     * @param rowIndex  行号：0...n
     * @param colIndex	列号：0...n
     * @param value		设置值
     * @return
     */
	virtual bool setValue(int rowIndex, int colIndex, const string& value);
	virtual bool setValue(int rowIndex, int colIndex, const bool& value);
	virtual bool setValue(int rowIndex, int colIndex, const int8_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const uint8_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const int16_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const uint16_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const int32_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const uint32_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const int64_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const uint64_t& value);
	virtual bool setValue(int rowIndex, int colIndex, const float& value);
	virtual bool setValue(int rowIndex, int colIndex, const double& value);
    /**
     * @LEVEL0
     * @brief 从其它TTable拷贝数据进来
     * @param other
     */
	virtual void copy(TTable& other);
	/**
	 * @LEVEL0
	 * @brief 从其它表将数据合并（复制）进来
	 * @param other 其它表，列数要与本表一致
	 */
	bool merge(TTable& other);
	/**
	 * @LEVEL0
	 * @brief 设置选择项序号
	 * @param sel
	 */
	virtual void setSelectedIndex(int sel, SttObject* obj=nullptr);
	/**
	 * @LEVEL0
	 * @brief 返回选中项序号
	 * @return
	 */
	virtual int getSelectedIndex();
	/**
	 * @LEVEL0
	 * @brief 获取键值列、显示值列序号
	 * @param k	返回键值列序号
	 * @param l	返回显示列序号
	 */
	virtual void getKeyLabelIndex(int& k, int& l);



public:
	/**
	 * @HideCA 基类已显示
	 */
	string toStr(uint8_t frac = 0);
	/**
	 * @HideCA 基类已显示
	 */
	void set(string newValue);
	/**
	 * @HideCA 基类已显示
	 */
	void setWith(SttObject * org, string newValue);
	/**
	 * @HideCA 基类已显示
	 */
	uint8_t type();
	/**
	 * @HideCA 数据的行数
	 */
	int size();
	/**
	 * @brief 获取总记录数
	 * @return
	 */
	virtual int getRecordCnt();
	/**
	 * @brief (关联变量)总记录数
	 * @param v
	 */
	void bindRecordCnt(Ivariable v);
	/**
	 * @brief 通知刷新
	 */
	void notify();
public:
	/**@brief 关键字段（列名）也是排序字段*/
	string 			m_key;
	/**@brief 显示字段（列名）*/
	string 			m_label;
	/**@brief 列集合  K：列名  V：默认值*/
	vector<Column> 	m_columns;
	/**@brief 行集合*/
	vector<Row> 	m_rows;
	/**@brief status*/
	uint8_t mStatus;
protected:
	/**@brief 选中行索引*/
	int selectedIndex;
	/**@brief 总记录数(关联变量)*/
	Ivariable v_record_cnt;
};
}
