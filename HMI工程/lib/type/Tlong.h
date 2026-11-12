/*
 * TLong.h
 *
 *  Created on: 2023年2月13日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include "base/Interface.h"

using namespace std;
using namespace stt;

namespace stt::data{
	/**
	 * @brief 普通变量 - 长整型
	 */
class TLong : public IVariable {
public:
	/**
	 * @HideCA
	 */
	TLong(const string& name);
	/**
	 * @HideCA
	 */
	virtual ~TLong();
	/**
	 * @LEVEL2
	 * @brief 获取当前变量值
	 * @param flag	参数标志，无意义。任取值
	 * @return
	 */
	int64_t get(uint8_t flag = 0);
	/**
	 * @HideCA
	 * @brief 赋值
	 */
	void set(int64_t newValue);
	/**@HideCA 基类已显示*/
	string toStr(uint8_t frac = 0);
	/**@HideCA 基类已显示*/
	void set(string newValue);
	/**@HideCA 基类已显示*/
	void setWith(SttObject * org, string newValue);
	/**@HideCA 基类已显示*/
	uint8_t type();
	/**@HideCA 基类已显示*/
	int size();

public:
	/**
	 * @HideCA
	 * @brief 变量值
	 */
	std::atomic_llong mValue;
};

typedef TLong* Tlong;

}

