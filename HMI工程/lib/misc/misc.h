/*
 * misc.h
 *
 *  Created on: 2023年8月28日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"

using namespace std;
using namespace stt;

/**
 * @namespace stt::misc
 * @brief 硬件操作(如：背光、蜂鸣器、环境变量、网口、IO、条码打印机、声音播放等)
 */
namespace stt::misc {
/**
 * @brief wav音频播放辅助
 */
class WavePlayer{
public :
	/**
	 * 开启音频输出
	 * @param filename：音频文件名
	 * @param runtime：播放时间，单位秒(>0时为实际播放秒数；=0~完整播放1次; <0~循环播放)
	 * @param volume：播放音量：(0-100)
	 * @return 0:成功, 其它失败
	 */
	static int playWave(string filename,int runtime,int volume);
	/**
	 * 获取当前播放状态
	 * @return //0-停止；1-播放；2-暂停；
	 */
	static int getSta();
	/**
	 * 暂停播放
	 */
	static void Pause();
	/**
	 * 恢复播放
	 */
	static void Resume();
	/**
	 * 获取音频总长度，单位s
	 *
	 * @return
	 */
	static int getDuration();
	/**
	 * 获取当前播放时间,单位s
	 * @return
	 */
	static int getCurTime();
	/**
	 * 设置音频播放音量
	 * @param volume：播放音量：(-60~30)
	 */
	static void setVolume(signed int volume);
	/**
	 * 设置音频播放音（算法不同）
	 * @param volume: 音量 0-100
	 */
	static void setVolume2(uint8_t volume);
};
/**
 * @brief 背光操作辅助
 */
class BackLight {
public:
	/**
	 * 背光初始化
	 * @return,0:初始化成功，-1：初始化失败
	 */
	static int setup();
	/**
	 * 关掉背光
	 * @return 0：成功；-1：失败
	 */
	static int close();
	/**
	 * 设置背光亮度值
	 * @param a (1-100)
	 */
	static void set(int a);
	/**
	 * 获取当前背光亮度
	 * @return ：1-100
	 */
	static int get(void);
	/**
	 * 调整背光亮度值（不保存）
	 * @param a (1-100)
	 */
	static void change(int a);
	/**
	 * 设置熄屏参数
	 *
	 * @param t			延时时间（s），0-关闭熄屏功能
	 * @param light		熄屏亮度值(1-100)
	 */
	static void setBackLightHoldingTime(int t = 0, int light=0);
	/**
	 * 获取熄屏参数
	 * @param t			延时时间（s）
	 * @param light		熄屏亮度值
	 */
	static void getBackLightHoldingTime(int& t, int& light);
	/**
	 * 设置屏保延时
	 *
	 * @param t			延时时间（s），0-关闭
	 */
	static void setScreenSaverHoldingTime(int t = 0);
	/**
	 * 获取屏保延时
	 * @param t			延时时间（s）
	 */
	static void getScreenSaverHoldingTime(int& t);

};

/**
 * @brief 蜂鸣器操作辅助
 */
class Buzzer{
public:
	/**
	 * 打开蜂鸣器
	 * @return 0：成功；	-1：失败
	 */
	static int setup();
	/**
	 * 关闭蜂鸣器
	 * @return 0：成功；	-1：失败
	 */
	static int close();
	/**
	 * 循环蜂鸣
	 *
	 * @period 单次蜂鸣时长，单位ms
	 * @time 总蜂鸣时长，单位ms
	 * @return 0：成功；	-1：失败
	 */
	static int beeps(int period, int time);
	/**
	 * 蜂鸣一次
	 * @param a:蜂鸣器响的时间（单位：ms）>=100ms 且 <=5000
	 */
	static void beep(int a);
	/**
	 * 键盘蜂鸣控制
	 * @param b   true/false
	 */
	static void enableIMEBeep(bool b);
};

/**
 * @brief GPIO操作辅助
 */
class GPIO{
public:
	/**
	 * 使能gpio
	 * @gpio_num,gpio的标号，如gpio13的标号为13
	 * @return,0:成功，-1：失败
	 */
	static int setup(uint8_t gpio_num);

	/**
	 * 禁能gpio
	 * @gpio_num,gpio的标号，如gpio13的标号为13
	 * @return,0:成功，-1：失败
	 */
	static int close(uint8_t gpio_num);

	/**
	 * 设置gpion的方向
	 * @gpio_num,gpio的标号，如gpio13的标号为13
	 * @pdirection,gpio的方向，输出为"out",输入为"in"
	 * @return,0:成功，-1：失败
	 */
	static int setDirection(uint8_t gpio_num, const string& pdirection);

	/**
	 * 设置gpion的电平
	 * @gpio_num,gpio的标号，如gpio13的标号为13
	 * @value,gpio的电平，0为低电平,非0为高电平
	 * @return,0:成功，-1：失败
	 */
	static int setValue(uint8_t gpio_num, int value);

	/**
	 * 获取gpion的电平
	 * @gpio_num,gpio的标号，如gpio13的标号为13
	 * @return,gpio的电平，0为低电平,1为高电平,-1:读取错误
	 */
	static int getValue(uint8_t gpio_num);
};

/**
 * @brief I2C操作辅助
 */
class I2C{
public:
	/**
	 * @brief 打开 I2C 设备
	 *
	 * 该函数用于打开指定地址的 I2C 设备，并返回对应的文件句柄。
	 *
	 * @param addr I2C 设备的地址。
	 * @return int 成功时返回 I2C 设备的文件句柄，失败时返回 -1。
	 */
	static int open(unsigned char addr);

	/**
	 * @brief 关闭 I2C 设备
	 *
	 * 该函数用于关闭指定文件句柄对应的 I2C 设备。
	 *
	 * @param iicfd I2C 设备的文件句柄。
	 * @return int 成功时返回 0，失败时返回 -1。
	 */
	static int close(int iicfd);

	/**
	 * @brief 向 I2C 设备写入数据
	 *
	 * 该函数用于向指定文件句柄对应的 I2C 设备写入数据。
	 *
	 * @param iicfd I2C 设备的文件句柄。
	 * @param buf 要写入的数据，存储在一个 `std::vector` 中。
	 * @param size 要写入的数据的字节数。
	 * @return int 成功时返回实际写入的字节数，失败时返回 -1。
	 */
	static int write(int iicfd, const std::vector<uint8_t>& buf, int size);

	/**
	 * @brief 从 I2C 设备读取数据
	 *
	 * 该函数用于从指定文件句柄对应的 I2C 设备读取数据。
	 *
	 * @param iicfd I2C 设备的文件句柄。
	 * @param buf 用于存储读取数据的 `std::vector`。
	 * @param size 要读取的数据的字节数。
	 * @return int 成功时返回实际读取的字节数，失败时返回 -1。
	 */
	static int read(int iicfd, std::vector<uint8_t>& buf, int size);
};

/**
 * @brief PWM操作辅助
 */
class PWM{
public:
	/**
	 * 使能pwm
	 * @param pwm_num	pwm号，0-3
	 * @return	<0,开启失败；1，开启成功
	 */
	static int setup(unsigned char pwm_num);

	/**
	 * 禁能pwm
	 * @param pwm_num：pwm号，0-3
	 * @return <0,关闭失败；1，关闭成功
	 */
	static int close(unsigned char pwm_num);

	/**
	 * pwm的频率设置
	 * @param pwm_num：pwm号，0-3
	 * @param period:频率（单位HZ）
	 * @return <0,失败；1，成功
	 */
	static int setPeriod(unsigned char pwm_num,unsigned int period);

	/**
	 * pwm频率获取
	 * @param pwm_num：pwm号，0-3
	 * @return 频率（单位hz）
	 */
	static int getPeriod(unsigned char pwm_num);

	/**
	 * pwm的占空比设置
	 * @param pwm_num：pwm号，0-3
	 * @param duty:占空比，0-100
	 * @return<0,失败；1，成功
	 */
	static int setDuty(unsigned char pwm_num,unsigned int duty);

	/**
	 * pwm 占空比获取
	 * @param pwm_num：pwm号，0-3
	 * @return 占空比，0-100
	 */
	static int getDuty(unsigned char pwm_num);

	/**
	 * pwm的开启设置
	 * @param pwm_num：pwm号，0-3
	 * @param enable:0，关闭；1，开启
	 * @return<0,失败；1，成功
	 */
	static int setEnable(unsigned char pwm_num,unsigned char enable);

	/**
	 * pwm 开启状态获取
	 * @param pwm_num：pwm号，0-3
	 * @return 0未开启；1开启；<0获取失败
	 */
	static int getEnable(unsigned char pwm_num);

	/**
	 * pwm的输出波形极性设置
	 * @param pwm_num：pwm号，0-3
	 * @param polarity:0，正常；1，取反
	 * @return<0,失败；1，成功
	 */
	static int setPolarity(unsigned char pwm_num,unsigned char polarity);

	/**
	 * 获取pwm输出波形的极性
	 * @param pwm_num：pwm号，0-3
	 * @return 0：正常；1：取反；<0,获取失败
	 */
	static int getPolarity(unsigned char pwm_num);
};

/**
 * @brief 网络接口操作类
 */
class EthInf{
public:
	/**
	 * 开启网卡
	 * @param eth	网卡名，例如“eth0"
	 * @return 0:成功 其他：失败
	 */
	static int turnOn(const string& eth);
	/**
	 * 关闭网卡
	 * @param eth	网卡名，例如“eth0"
	 * @return 0:成功 其他：失败
	 */
	static int turnOff(const string& eth);
	/**
	 * 获取本机IP地址
	 * @param eth	网卡名称，如“eth0"
	 * @param ip	用来存放IP地址
	 * @return: 0 : 成功 ；  -1 :  失败
	 */
	static int getLocalIP(const string& eth, string& ip);
	/**
	 * 设置本机IP
	 * @param eth	网卡名称，如“eth0"
	 * @param ip 	设置的IP地址，以点分十进制的字符串方式表示，如“192.168.0.5”
	 * @return: 0 : 成功 ；  -1 :  失败
	 */
	static int setLocalIP(const string& eth, const string& ip);
	/**
	 * 设置本机mac地址
	 * @param eth	网卡名称，如“eth0"
	 * @param mac	mac地址，如"00:30:1B:BA:02:DB"
	 * @return: 0 : 成功 ；  -1 :  失败
	 */
	static int setLocalMac(const string& eth, const string& mac);
	/**
	 * 获取本机mac地址
	 * @param eth	网卡名称，如“eth0"
	 * @param mac	用来存放mac地址
	 * @return: 0 : 成功 ；  -1 :  失败
	 */
	static int getLocalMac(const string& eth, string& mac);
	/**
	 * @LEVEL0
	 * @brief  设置本机掩码地址
	 * @param eth		网卡名称，如"eth0"
	 * @param szNetMask	存放掩码地址
	 * @return	0 : 成功 ；  -1 :  失败
	 */
	static int setLocalNetmask(const string& eth, const string& szNetMask);
	/**
	 * @LEVEL0
	 * @brief  获取本机掩码地址
	 * @param eth		网卡名称，如"eth0"
	 * @param szNetMask	存放掩码地址
	 * @return	0 : 成功 ；  -1 :  失败
	 */
	static int getLocalNetmask(const string& eth, string& szNetMask);
	/**
	 * @LEVEL0
	 * @brief  设置本机广播地址
	 * @param eth		网卡名称，如"eth0"
	 * @param broadcast	广播地址，如“192.168.1.255”
	 * @return	0 : 成功 ；  -1 :  失败
	 */
	static int setLocalBroadcast(const string& eth, const string& broadcast);
	/**
	 * @LEVEL0
	 * @brief  获取本机广播地址
	 * @param eth		网卡名称，如"eth0"
	 * @param broadcast	用来存放broadcast地址
	 * @return	0 : 成功 ；  -1 :  失败
	 */
	static int getLocalBroadcast(const string& eth, string& broadcast);
	/**
	 * @LEVEL0
	 * @brief  设置本机网关
	 * @param eth		网卡名称，如"eth0"
	 * @param GateWay	网关地址，如“192.168.1.1”
	 * @return
	 */
	static int setLocalGateway(const string& eth, const string& GateWay);
	/**
	 * @LEVEL0
	 * @brief 获取本机网关地址
	 * @param eth		网卡名称，如"eth0"
	 * @param GateWay	用来存放网关地址
	 * @return
	 */
	static int getLocalGateway(const string& eth, string& GateWay);
	/**
	 * @LEVEL0
	 * @brief 获取开启的网卡
	 * @return	返回开启的网络卡列表
	 */
	static vector<string> getUpEthInfo();
	/**
	 * @LEVEL0
	 * @brief 查询网卡是否开启
	 * @param eth
	 * @return 0：未开启；1：开启；<0：查询失败；
	 */
	static int isUp(const string& eth);
	/**
	 * @LEVEL0
	 * @brief 自动分配ip地址
	 * @param eth  网卡名，如"eth0"或"wlan0"
	 */
	static void callDHCP(const string& eth);
};

/**
 * @brief 环境变量操作辅助
 */
class EnvVar{
public:
	/**
	 * 读变量
	 * @param key	变量名
	 * @param value	返回值
	 * @return false:失败； true:成功
	 */
	static bool readVar(const string& key, string& value);

	/**
	 * 读变量
	 * @param key	变量名
	 * @param value	写入值
	 * @return false:失败； true:成功
	 */
	static bool writeVar(const string& key, const string& value);
};

}
