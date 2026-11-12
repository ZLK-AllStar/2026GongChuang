/*
 * TFloat.h
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
	 * @brief 普通变量 - 浮点型
	 */
class TFloat : public IVariable {
public:
	/**
	 * @HideCA
	 */
	TFloat(const string& name);
	/**
	 * @HideCA
	 */
	virtual ~TFloat();
	/**
	 * @LEVEL2
	 * @brief 获取当前值
	 * @param flag
	 * @return
	 */
	float get(uint8_t flag = 0);
	/**
	 * @LEVEL2
	 * @brief 赋值
	 */
	void set(float newValue);
	/**
	 * @LEVEL2
	 * @brief 取当前值的字符串形式
	 * @param frac 小数位(仅浮点型有意义)
	 */
	string toStr(uint8_t frac = 0);
	/**
	 * @LEVEL2
	 * @brief 用字符串赋值
	 * @param newValue 浮点值的字符串格式，只会转换有效部分，见函数std::strtof
	 */
	void set(string newValue);
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
	atomic<float> mValue;

};

typedef TFloat* Tfloat;

}

