/*
 * TVchar.h
 *
 *  Created on: 2023年2月13日
 *      Author: Simtutai
 */
#pragma once

#include <iostream>
#include "base/Interface.h"

using namespace std;
using namespace stt;

namespace stt::data{
	/**
	 * @brief 普通变量 - 字符串
	 */
class TVchar : public IVariable {
public:
	TVchar(const string& name, const string& setting="");
	virtual ~TVchar();

	/**
	 * @LEVEL2
	 * @brief 获取当前值
	 * @param flag	参数标志 [0,1]
	 * @return
	 */
	vector<uint8_t> get(uint8_t flag = 0);
	/**
	 * @LEVEL2
	 * @brief 用字符串赋值
	 * @param newValue 字符串
	 */
	void set(string newValue);
	/**
	 * @LEVEL2
	 * @brief 赋值
	 * @param v
	 */
	void set(vector<uint8_t>& v);
	/**
	 * @LEVEL2
	 * @brief 取当前值的字符串形式
	 * @param frac 小数位(仅浮点型有意义)
	 */
	string toStr(uint8_t frac = 0);
	/**
	 * @HideCA
	 */
	void setWith(SttObject * org, string newValue);
	/**
	 * @LEVEL2
	 * @brief 获取变量类型
	 */
	uint8_t type();
	/**
	 * @LEVEL2
	 * @brief 获取变量占用字节数
	 */
	int size();

public:
	/**@HideCA*/
	vector<uint8_t> mv;
};

typedef TVchar* Tvchar;

}
