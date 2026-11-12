/*
 * DVchar.h
 *
 *  Created on: 2023年2月15日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include "base/Interface.h"

using namespace std;
using namespace stt;

namespace stt::data{
	/**
	 * @brief 设备变量 - 字符串
	 */
class DVchar : public IDvcVariable {
public:
	/**@HideCA*/
	DVchar(const string& name, const string& setting="");
	/**@HideCA*/
	virtual ~DVchar();
	/**
	 * @LEVEL2
	 * @brief 获取当前值
	 * @param flag	参数标志 [0,1]
	 * 0: 取变量临时值(mw) 1：取变量实际值(mv)
	 * @return  原始字节数组
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
	 * @brief 原始赋值
	 * @param v	char数组
	 * @return
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
	/**
	 * @LEVEL2
	 * @brief 取临时值(见mw)地址
	 */
	virtual unsigned char* getPointerW();
	/**
	 * @LEVEL2
	 * @brief 写入设备值
	 * @param v 为指向一个字符串指针
	 * @param force 是否强制
	 */
	void write(void * v, bool force);
	/**
	 * @LEVEL2
	 * @brief 写入设备值
	 * @param v
	 */
	void write(vector<uint8_t>& v);

public:
	/**
	 * @HideCA
	 * @brief 变量值
	 * */
	vector<uint8_t> mw;
	/**@HideCA*/
	int16_t mLen;
};

typedef DVchar* Dvchar;

}

#pragma once
