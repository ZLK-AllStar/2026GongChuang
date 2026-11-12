/*
 * UARTBase.h
 *
 *  Created on: 2023年3月16日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "type/Tint.h"

using namespace std;
using namespace stt::data;

namespace stt{

class UARTBase: public SttObject, implements IMISCDevice, IVarOperate {
public:
	UARTBase();
	virtual ~UARTBase();
	/**
	 * @LEVEL0
	 * @brief 参数设置
	 * @param settings
	 * @return
	 */
	virtual bool set(const string& settings);
	/**
	 * @LEVEL0
	 * @brief 启动
	 * @return	是否成功
	 */
	virtual bool open();
	/**
	 * @LEVEL0
	 * @brief 	关闭
	 * @return	是否成功
	 */
	virtual bool close();
	/**
	 * @LEVEL0
	 * @brief 暂停
	 * @return	是否成功
	 */
	virtual bool pause();
	/**
	 * @LEVEL0
	 * @brief 继续
	 * @return	是否成功
	 */
	virtual bool resume();
	/**
	 * @LEVEL1
	 * @biref 清除缓冲区
	 * @param t	0:读缓冲区  1:写缓冲区
	 * @return
	 */
	virtual bool clean(int t=0);
	/**
	 * @LEVEL0
	 * @brief 获取状态
	 * @return IMISCDevice::STATUS
	 */
	virtual int status();
	/**
	 * @LEVEL0
	 * @brief 向UART写
	 * @return	>0:写数据长度  <=0:设备状态不对或错误
	 */
	virtual int wr(const vector<uint8_t>& pData);
	/**
	 * @LEVEL0
	 * @brief 从UART读一条数据
	 * @param pData		存放读出的数据
	 * @return	>0:读出的数据长度  <0:设备状态不对 0:未读到数据
	 */
	virtual int rd(vector<uint8_t>& pData);
	/**
	 * @LEVEL0
	 * @brief 	解析读到的数据
	 * @return	返回长度
	 */
	virtual int parseData();
	/**
	 * @LEVEL0
	 * @brief 处理每一帧数据数据
	 * @param pData		数据
	 * @param len		长度
	 */
	virtual void procData(const uint8_t *pData, uint16_t len);
	/**@HideCA*/
	virtual void readFromDevice();
	/**@HideCA*/
	virtual void writeToDevice();
	/**@HideCA*/
	virtual bool reconnect();
public:
	/**@HideCA*/
	Tint mStatus;
	/**@HideCA*/
	void* mMux;
	/**@HideCA*/
	virtual void statusChange(int status);
	/**
	 * @brief 发送事件
	 * @param e	事件代码，见[常量Event]
	 * @param ev	见LvEvent*
	 */
	void sendEvent(uint8_t e, void * ev);
	/**
	 * @brief 接收时间心跳
	 * @param tick 单位ms
	 */
	void sendTick(uint64_t tick);
	/**
	 * @brief 业务类型
	 * @return
	 */
	uint8_t type();
	/**
	 * @brief 事件类型
	 * @return
	 */
	uint8_t event();
protected:
	/**@HideCA*/
	virtual int64_t openPort();
	/**@HideCA*/
	virtual void closePort();
};

typedef UARTBase* UartBase;

}
