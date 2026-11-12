/*
 * NetcomBase.h
 *
 *  Created on: 2023年3月26日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "base/UARTBase.h"

using namespace std;

namespace stt{

class NetComBase: public UARTBase {
public:
	NetComBase();
	virtual ~NetComBase();
	/**
	 * @LEVEL0
	 * @brief 参数设置
	 * @param settings
	 * @return
	 */
	virtual bool set(const string& settings);
	/**
	 * @HideCA
	 * @brief 从设备读数据
	 */
	virtual void readFromDevice();
	/**
	 * @HideCA
	 * @brief 向设备写数据
	 */
	virtual void writeToDevice();
	/**@HideCA*/
	virtual bool reconnect();
protected:
	/**@HideCA*/
	virtual int64_t openPort();
	/**@HideCA*/
	virtual void closePort();
};

typedef NetComBase* NetcomBase;

/**
 * USB打印机
 */
class USBPrinterBase: public UARTBase {
public:
	USBPrinterBase();
	virtual ~USBPrinterBase();
	/**
	 * @LEVEL0
	 * @brief 参数设置
	 * @param settings
	 * @return
	 */
	virtual bool set(const string& settings);
	/**
	 * @LEVEL0
	 * @brief 读数据
	 * @param pData		存放读出的数据
	 * @return	>0:读出的数据长度  <0:设备状态不对 0:未读到数据
	 */
	virtual int rd(vector<uint8_t>& pData);
	/**
	 * @HideCA
	 * @brief 从设备读数据
	 */
	virtual void readFromDevice();
	/**
	 * @HideCA
	 * @brief 向设备写数据
	 */
	virtual void writeToDevice();
protected:
	/**@HideCA*/
	virtual int64_t openPort();
	/**@HideCA*/
	virtual void closePort();
};

typedef NetComBase* NetcomBase;

}
