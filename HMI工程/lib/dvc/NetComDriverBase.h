/*
 * NetComDriverBase.h
 *
 *  Created on: 2023年3月27日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "dvc/UARTDriverBase.h"
#include <vector>

namespace stt {

class NetComDriverBase : public UARTDriverBase {
public:
	NetComDriverBase(IDeviceManager* dm, string settings);
	virtual ~NetComDriverBase();
	/**
	 * 添加设备
	 * @param dvc
	 */
	virtual void add(Idevice dvc);
protected:
	/**
	 * 重连设备
	 */
	virtual bool reconnect();
	virtual int64_t openPort();
};
}
