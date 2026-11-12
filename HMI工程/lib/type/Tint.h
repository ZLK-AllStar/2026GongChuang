/*
 * Int.h
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
	 * @brief 普通变量 - 整型
	 */
class TInt : public IVariable {
public:
	/**
	 * @HideCA
	 */
	TInt(const string& name);
	/**
	 * @HideCA
	 */
	virtual ~TInt();
	/**
	 * @LEVEL2
	 * @brief 获取当前变量值
	 * @param flag	参数标志，无意义。任取值
	 * @return
	 */
	int get(uint8_t flag = 0);
	/**
	 * @LEVEL2
	 * @brief 赋值
	 */
	void set(int newValue);
	/**
	 * @HideCA 基类已显示
	 * @brief 取当前值的字符串形式
	 * @param frac 小数位(仅浮点型有意义)
	 */
	string toStr(uint8_t frac=0);
	/**
	 * @HideCA 基类已显示
	 * @brief 用字符串赋值
	 * @param newValue 整型值的字符串格式，只会转换有效况部分，见函数std::strtoll
	 */
	void set(string newValue);
	/**
	 * @HideCA 基类已显示
	 */
	void setWith(SttObject * org, string newValue);
	/**
	 * @HideCA 基类已显示
	 * @brief 获取变量类型
	 */
	uint8_t type();
	/**
	 * @HideCA 基类已显示
	 * @brief 获取变量占用字节数
	 */
	int size();

public:
	/**
	 * @HideCA
	 * @brief 变量值
	 */
	std::atomic_int mValue;
};

typedef TInt* Tint;

}
