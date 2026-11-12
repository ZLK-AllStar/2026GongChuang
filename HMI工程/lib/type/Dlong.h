/*
 * DLong.h
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
	 * @brief 设备变量 - 长整型(64位)
	 */
class DLong : public IDvcVariable {
public:
	/**@HideCA*/
	DLong(uint8_t type, const string& name, const string& setting);
	/**@HideCA*/
	virtual ~DLong();

	/**
	 * @LEVEL2
	 * @brief 获取当前值
	 * @param flag	参数标志 [0,1]
	 * 0: 取变量临时值(_w_) 1：取变量实际值(_v_)
	 * @return
	 */
	long long get(uint8_t flag = 0);
	/**
	 * @LEVEL2
	 * @brief 赋值
	 */
	void set(long long newValue);
	/**
	 * @LEVEL2
	 * @brief 取当前值的字符串形式
	 * @param frac 小数位(仅浮点型有意义)
	 */
	string toStr(uint8_t flag = 0);
	/**
	 * @LEVEL2
	 * @brief 用字符串赋值
	 * @param newValue 长整型值的字符串格式，只会转换有效况部分，见函数std::strtoll
	 */
	void set(string newValue);
	/**
	 * @HideCA
	 */
	void setWith(SttObject * org, string newValue);
	/**
	 * @LEVEL2
	 * @brief 获取变量类型
	 * 接口方法的实现。
	 */
	uint8_t type();
	/**
	 * @LEVEL2
	 * @brief 获取变量占用字节数
	 * 接口方法的实现。
	 */
	int size();
	/**
	 * @LEVEL2
	 * @brief 取临时值(见_w_)地址
	 */
	virtual unsigned char* getPointerW();
	/**
	 * @LEVEL2
	 * @brief 将设备值分发到控件上。
	 * 从设备读到数据时调用
	 * @param v 为指向一个长整型值的指针
	 * @param force 是否强制刷新本控件(引发当前变量变化的控件)
	 */
	virtual void write(void * v, bool force);
	/**
	 * @LEVEL2
	 * @brief 将设备值分发到控件上。
	 * 从设备读到数据时调用
	 * @param v 长整型值
	 * @param force 是否强制刷新本控件(引发当前变量变化的控件)
	 */
	virtual void write(vector<uint8_t>& v);
public:
	/**
	 * @HideCA
	 * @brief 变量临时值
	 * */
	atomic_llong mw;
};

typedef DLong* Dlong;

}

