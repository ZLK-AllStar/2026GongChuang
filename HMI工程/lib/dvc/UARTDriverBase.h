/*
 * UARTDriverBase.h
 *
 *  Created on: 2023年3月26日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "base/UARTBase.h"
#include <vector>
#include <queue>

namespace stt {

class UARTDriverBase : public SttObject, implements IDvcDriver {
public:
	std::mutex lock_;
	UARTDriverBase(IDeviceManager* dm, string settings);
	virtual ~UARTDriverBase();
public:
	/**
	 * 获取设备驱动设置信息
	 * @return
	 */
	virtual string getSettings();
	virtual string getSetting(string key);
	/**
	 * 添加设备
	 * @param dvc
	 */
	virtual void add(Idevice dvc);
	/**
	 * 获取驱动名称
	 * @return
	 */
	string getDllName();
	/**
	 * 获取通讯接口（RS232/RS485-2W/eth0...）
	 * @return
	 */
	string getIf();
	/**
	 * 获取通讯接口名称(COM1/COM2/IP...)
	 * @return
	 */
	string getIfName();
	/**
	 *
	 * @param pData
	 * @param len
	 * @return
	 */
	virtual bool postData(std::shared_ptr<IDvcVariable> d);
	/**
	 * 启动设备驱动
	 * @return
	 */
	virtual bool start();
	/**
	 * 停止设备驱动
	 * @return
	 */
	virtual bool stop();
	/**
	 * 设备运行处理，子类需在此实现协议处理功能
	 * @return
	 */
	virtual bool threadRun();
	/**
	 * 重连设备
	 */
	bool reconnect();
	/**
	 * 设置驱动
	 * @param dll
	 */
	void setDll(void * dll);
	/**
	 * 获取驱动
	 */
	void* getDll();
	/**
	 * @brief 实时写设备 immediate
	 */
	virtual int writeDevice(Ivariable IDvcVar);
	/**
	 * @brief 实时读设备 immediate
	 */
	virtual int readDevice(Ivariable IDvcVar);
protected:
	virtual int64_t openPort();
	string _settings_;
	mutex mtx;
	int64_t uartID;
	SttThread* _w_t_;
	vector<Idevice> _ll_;
	IDeviceManager* _dm_;
	void * dll;
	/**
	 * 写变量队列
	 */
	queue<std::shared_ptr<IDvcVariable>> _q_wr_vars_;
	/**
	 * 采集周期
	 */
	uint16_t period;
};

} /* namespace stt */
