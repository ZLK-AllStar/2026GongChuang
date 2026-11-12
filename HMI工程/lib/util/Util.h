/*
 * Util.h
 *
 *  Created on: 2023年2月13日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include "base/Interface.h"
#include "type/TTable.h"
#include <stack>

using namespace std;
using namespace stt::data;

/**
 * @namespace stt::util
 * @brief 常用工具(日期、字符串、文件、http、DEBUG消息级别、ini配置文件操作、键值对、系统信息等)
 */
namespace stt::util{
	/**
	 * @brief 常用方法
	 */
class Util {
public:
	/**
	 * @LEVEL0
	 * @brief 整型数转BCD
	 * @param decimal
	 * @return
	 */
	static int DecimalToBCD(int decimal);
	/**
	 * @LEVEL0
	 * @brief BCD转32位整型数
	 * @param bcd
	 * @return
	 */
	static unsigned int BCDtoDecimal(unsigned int bcd);
	/**
	 * @LEVEL0
	 * @brief 16位BCD转16位整型数
	 * @param bcd
	 * @return
	 */
	static int16_t BCD16toDecimal(int16_t bcd);
	/**
	 * @LEVEL0
	 * @brief 16位整型数转16位BCD
	 * @param decimal
	 * @return
	 */
	static int16_t DecimalToBCD16(int16_t decimal);
	/**
	 * @LEVEL0
	 * @brief 32位BCD转32位整型数
	 * @param bcd
	 * @return
	 */
	static int32_t BCD32toDecimal(int32_t bcd);
	/**
	 * @LEVEL0
	 * @brief 32位整型数转32位BCD
	 * @param decimal
	 * @return
	 */
	static int32_t DecimalToBCD32(int32_t decimal);
	/**
	 * @LEVEL1
	 * @brief 从json字符串获取key值
	 * @param setting
	 * @param key
	 * @return 找不到则返回""
	 */
	static string getValFromJsonc(const string& json, const string& key);
	/**
	 * @LEVEL1
	 * @brief 批量从jsonc字符串获取属性值
	 * 			以字符串返回值（只能取一层）
	 * @param json		json字符串
	 * @param kvs		KV键值对，用于传入要取值的键，如果为空则遍历json字符串属性并填入
	 * @return	true:成功 false：失败
	 */
	static bool getValsFromJsonc(const string& json, vector<KV_Str_Str>& kvs);
	/**
	 * @LEVEL1
	 * @brief 批量从jsonc字符串数组vector<KV_Str_Str>对象列表
	 * @param json		json字符串
	 * @param kvs		返回得到的键值对列表
	 * @return	true:成功 false：失败
	 */
	static bool getArrayFromJsonc(const string& json, vector<vector<KV_Str_Str>>& kvs);
	/**
	 * @LEVEL1
	 * @brief 将vector<KV_Str_Str>对象列表转为Jsonc字符串
	 * @param kvs		键值对列表
	 * @param json		返回得json字符串
	 * @return	true:成功 false：失败
	 */
	static bool convertArrayToJson(vector<vector<KV_Str_Str>>& kvs, string& json);
	/**
	 * @LEVEL1
	 * @brief 连接到指定的主机和端口号
	 * @param hostname
	 * @param port
	 * @return
	 */
	static int socketConnect(const char *hostname, int port);
	/**
	 * @LEVEL1
	 * @brief 在port指定的端口上建立server端socket
	 * @param port
	 * @param block 阻塞式开关
	 * @return
	 */
	static int socketCreate(int port, bool block = false);
	/**
	 * @LEVEL1
	 * @brief server端socket开始accept的函数
	 * @param listen_st
	 * @return
	 */
	static int socketAccept(int listen_st);
	/**
	 * @LEVEL1
	 * @brief 关闭socket
	 * @param port
	 * @return
	 */
	static int socketClose(int st);
	/**
	 * @LEVEL1
	 * @brief tcp写数据
	 * @param handler
	 * @param buff
	 * @param len
	 * @param flag
	 * @return
	 */
	static int socketWrite(int64_t handler, char * buff, int len, int flag=0);
	/**
	 * @LEVEL1
	 * @brief 读tcp数据
	 * @param handle
	 * @param buff
	 * @param recvLen
	 * @param timeOut
	 * @param printinfo
	 * @return
	 */
	static int socketReadAll(int64_t handle, char * buff,uint8_t recvLen,uint16_t timeOut, bool printinfo=false);
	/**
	 * @LEVEL1
	 * @brief 打开串口
	 * @param pFileName
	 * @param baudRate
	 * @param byteSize
	 * @param parity
	 * @param stopBits
	 * @return
	 */
	static int64_t openComPort(const char * pFileName, int baudRate, int byteSize, int parity, int stopBits);
	/**
	 * @LEVEL1
	 * @brief 串口写
	 * @param handle
	 * @param bytes
	 * @param offset
	 * @param size
	 * @return >0:写字节数  <=0:失败
	 */
	static int uartWrite(int64_t handle, unsigned char * bytes, int offset, int size);
	/**
	 * @LEVEL1
	 * @brief 关闭串口
	 * @param handle
	 */
	static void uartClose(int64_t handle);
	/**
	 * @LEVEL1
	 * @brief 串口是否可用
	 * @param handle
	 * @return
	 */
	static int uartAvailable(int64_t handle);
	/**
	 * @LEVEL1
	 * @brief 读串口数据一次
	 * @param handle
	 * @param bytes 存放读到的数据
	 * @param offset
	 * @param size
	 * @return
	 */
	static int uartRead(int64_t handle, unsigned char * bytes, int offset, int size);
	/**
	 * @LEVEL1
	 * @brief 读取串口数据
	 */
	static int uartReadAll(int64_t handle, char * buff,uint8_t recvLen,uint16_t timeOut, bool printinfo=false);
	/**
	 * @LEVEL1
	 * @brief 获取时间戳到毫秒
	 * @param szTimer
	 * @return
	 */
	static uint64_t getCurrentTimerMS(char* szTimer=NULL);
	/**
	 * @LEVEL2
	 * @brief dlopen封装
	 * @param file
	 * @param mode
	 */
	static void *dllopen(const char *file, int mode);
	/**
	 * @LEVEL2
	 * @brief dlclose封装
	 * @param handle
	 * @return
	 */
	static int dllclose(void *handle);
	/**
	 * @LEVEL2
	 * @brief dlsym封装
	 * @param handle
	 * @param name
	 */
	static void *dllsym(void *handle, const char *name);
	/**
	 * @LEVEL2
	 * @brief dlerror封装
	 * @return
	 */
	static char *dllerror(void);
	/**
	 * @LEVEL2
	 * @brief 动态加载dll/so
	 * @param pdll 为加载返回，如已经加载过，不重复加载
	 * @param path 库文件路径
	 */
	static void* loadDll(void* pdll, const char* path);
	/**
	 * @LEVEL2
	 * @brief 动态加载函数，返回函数指针
	 * @param lib load_all的返回结果
	 * @param funcname 函数名称
	 */
	static void* loadFunc(void* lib, const char* funcname);
	/**
	 * @LEVEL0
	 * @brief RGB转整型
	 * @param r
	 * @param g
	 * @param b
	 * @return
	 */
	static int rgb(int r, int g, int b);
	/**
	 * @LEVEL0
	 * @brief 获取屏幕尺寸screensize
	 * @return
	 */
	static Area getScrSize();
	/**
	 * @LEVEL1
	 * @brief 执行shell命令
	 * @param cmdline 	shell命令
	 * @param mode 		"r" "w"
	 * @return 1: 失败   0：成功
	 */
	static int exe(const string& cmdline, const string& mode);
	/**
	 * @LEVEL1
	 * @brief 同步执行shell命令，并获取执行结果
	 * @param cmdline 	shell命令
	 * @param mode 		"r"可以对返回的流进行读取； "w"可以对返回的流进行写入
	 * @param result    [输出]返回结果
	 * @return false: 失败   true：成功
	 */
	static bool exeSync(const string& cmdline, const string& mode, string& result);
	/**
	 * @LEVEL1
	 * @brief 生成0-MAX之间的随机整数
	 * @param max 范围   0:不限制范围 <0:无效
	 * @return
	 */
	static int randnum(int max=0);
	/**
	 * @LEVEL1
	 * @brief 修改设置密码
	 * @param oldPwd 	旧密码
	 * @param newPwd 	新密码
	 * @return 0：成功	其它: 失败
	 */
	static int changeCfgPassword(const string& oldPwd, const string& newPwd);
	/**
	 * @LEVEL1
	 * @brief 核对输入密码(没有配置密码时，即密码为空时，输入任何密码均为true)
	 * @param pwd	输入的密码
	 */
	static bool checkPassword(const string& pwd);
	/**
	 * @LEVEL1
	 * @brief 启用看门狗
	 * @param timeout 	守护时间(s), 1~100，0则视为关闭
	 * @return
	 */
	static bool enableWatchdog(int timeout);
	/**
	 * @LEVEL2
	 * @brief 当前线程休眠一段时间
	 * @param ms 毫秒数
	 */
	static void sleepfor(int ms);
	/**
	 * @LEVEL2
	 * @brief 设置mma大小C系列不支持
	 * @param mSize单位M 4-70M
	 * @return 0：成功 其它：失败
	 */
	static int setMmaSize(int mSize);
	/**
	 * @LEVEL2
	 * @brief 简单表达式计算
	 * @param str	表达式
	 * @return 计算结果
	 */
	static string simpleCalc(const string& str);
};

/**
 * @brief 键值对操作辅助
 */
class KVUtil{
public:
	/**
	 * @LEVEL1
	 * @brief 插入键值对
	 * @param vct
	 * @param k
	 * @param v
	 */
	static void push_back(vector<KV_Str_Str>& vct, const string& k, const string& v);
	/**
	 * @LEVEL1
	 * @brief 构造KV列表
	 * @param vct		返回
	 * @param keys		键字符串，以逗号分割
	 * @param vals		值字符串，以逗号分割。当vals数量少于keys时，缺少部分默认为""
	 */
	static void build(vector<KV_Str_Str>& vct, const string& keys, const string& vals);
	/**
	 * @LEVEL1
	 * @brief 从表格行构造KV键值对
	 * @param tbl	表格
	 * @param sel	行号
	 * @return		KV数组  k:列名 v:单元格内容
	 */
	static vector<KV_Str_Str> buildFromRow(TTable& tbl, int sel);
	/**
	 * @LEVEL1
	 * @brief 从键值对列中获取值
	 * @param vct
	 * @param key
	 * @return	不存在时返回空字符串""
	 */
	static string getVal(vector<KV_Str_Str>& vct, const string& key);
	/**
	 * @LEVEL1
	 * @brief 从键值对列表中获取K或V的列表
	 * @param vct	键值对列表
	 * @param k		K值，默认true。true：取键列表；false:取值列表
	 * @return
	 */
	static vector<string> getKVector(vector<KV_Str_Str>& vct, bool k = true);
	/**
	 * @LEVEL1
	 * @brief 将k,v用deli连接起来，返回列表，如k='v'，可用于UPDATE数据库
	 * @param vct	键值对列表
	 * @param deli	分隔符
	 * @param padl	给v值左侧增加字符
	 * @param padr  给v值右侧增加字符
	 */
	static vector<string> getKVector(vector<KV_Str_Str>& vct, const string& deli, const string& padl="", const string& padr="");
	/**
	 * @LEVEL1
	 * @brief 从键值对列查找KEY并设置值
	 * @param vct	键值对列表
	 * @param key	键
	 * @param value	设置值
	 * @return	不存在时返回false， 存在返回true
	 */
	static bool setVal(vector<KV_Str_Str>& vct, const string& key, string value);

	/**
	 * @Level1
	 * @brief 从数据库S_CFG表中取配置
	 * @param key 关键字
	 * @param defv 默认值。不存在key时，则返回默认值
	 */
	static string getVal(const string& key, const string& defv);
	/**
	 * @Level1
	 * @brief 将配置写入数据库S_CFG表中
	 * @param key 关键字
	 * @param val 值
	 */
	static void setVal(const string& key, const string& val);
};

/**
 * @brief 系统信息
 */
class SysInfo{
public:
	/**
	 * @LEVEL0
	 * @brief 获取HMI型号
	 * @return
	 */
	static string getHMIModel();
	/**
	 * @LEVEL0
	 * @brief 获取设备序列号
	 * @return
	 */
	static string getSerialNumber();
	/**
	 * @LEVEL0
	 * @brief 获取系统版本号
	 * @return
	 */
	static string getSysVersion();
	/**
	 * @LEVEL0
	 * @brief 获取系统版本发布时间
	 * @return
	 */
	static string getSysDay();
	/**
	 * @LEVEL0
	 * @brief 获取OS
	 * @return
	 */
	static string getOS();
	/**
	 * @LEVEL0
	 * @brief 获取固件版本号
	 * @return
	 */
	static string getFwVersion();
	/**
	 * @LEVEL0
	 * @brief 获取固件发布时间
	 * @return
	 */
	static string getFwDay();
	/**
	 * @LEVEL0
	 * @brief 获取U盘列表路径，如:"/vendor/udisk_sda1","/vendor/udisk_sdb1",...
	 * @return
	 */
	static vector<string> getUdisks();
	/**
	 * @LEVEL0
	 * @brief 获取热插拔后U盘列表路径，如:"/vendor/udisk_sda1","/vendor/udisk_sdb1",...
	 * @note 使用创建、删除目录测试是否成功，勿高频次使用本方法，避免U盘寿命减少
	 */
	static vector<string> getUdisksHot();
	static vector<string> getSDCards();
	static void setHotPlugin(bool b);
	static bool getHotPlugin();
	/**
	 * @LEVEL0
	 * @brief 获取打印机列表
	 * @return
	 */
	static vector<string> getPrinters();
	/**
	 * @LEVEL0
	 * @brief 获取输入设备列表（如键盘、鼠标、扫码枪等）
	 * @return
	 */
	static vector<string> getInputDevs();
	/**
	 * @LEVEL0
	 * @brief 获取串口列表
	 * @return
	 */
	static vector<string> getSerialComs();
	/**
	 * @LEVEL0
	 * @brief 获取网口列表
	 * @return
	 */
	static vector<string> getEths();
	/**
	 * @LEVEL0
	 * @brief 获取WLAN列表
	 * @return
	 */
	static vector<string> getWLan();
	/**
	 * @LEVEL0
	 * @brief 获取4G/5G列表
	 * @return
	 */
	static vector<string> getFGs();
	/**
	 * @LEVEL0
	 * @brief 获取指定名称的进程ID
	 * @return	进程ID
	 */
	static int getProcessByName(const string& name);
	/**
	 * @LEVEL0
	 * @brief 杀死指定名称的进程
	 */
	static void killProcessByName(const string& name);
};

/**
 * @brief ini文件助手
 */
class IniFile{
public:
	IniFile(const string& path);
	~IniFile();
	/**
	 * 读键值
	 * @param key		读取的键
	 * @param value		返回的值
	 * @param dv		不存在时默认返回值
	 * @return	true:键存在  false: 键不存在
	 */
	bool readString(const string& key, string& value, const string& dv);
	/**
	 * 写键值
	 * @param key		键
	 * @param value		值
	 * @return	true:写成功  false: 写失败
	 */
	bool writeString(const string& key, const string& value);
private:
	void * mDic;
	string mFile;
};

class iniFileUtil{
public:
	/**
	 * 从配置文件中根据key读取value
	 * @param key：键名
	 * @param value：键值
	 * @param file：配置文件名
	 * @return 0：成功  其他：失败
	 */
	static int readIniValue(const string& key, string& value, const string& file);
	/**
	 * 往配置文件修改或写入一对键值对
	 * @param key：键名
	 * @param value：键值
	 * @param file：配置文件名
	 * @return 0：成功  其他：失败
	 */
	static int writeIniValue(const string& key, const string& value, const string& file);
	/**
	 * 加载Ini文件
	 * @param 	path	文件路径
	 * @return	文件对象
	 */
	static IniFile* load(const string& path);
	/**
	 * 释放Ini文件并保存
	 * @param f	文件对象
	 */
	static void free(IniFile* f);
};

/**
 * @brief 调试日志输出
 */
class DEBUG{
public:
	static string lastErrorString;
	static void setLastErrorString(string s);
	static string getLastErrorString();
	static void LOG(int l, const char * file, int line, const char* pszFmt, ...);
	static void set(int l, bool v=false, bool r=false);

	class MEM {
	public:
		/**
		 * @LEVEL2
		 * @brief 启动内存监视线程
		 * @param ms 每n秒打印一次内存
		 */
		static void startMemoryMonitor(int ms=100);
		/**
		 * @LEVEL2
		 * @brief 停止内存监视线程
		 */
		static void stopMemoryMonitor();
		/**
		 * @LEVEL2
		 * @brief 打印一次剩余可用内存
		 */
		static void printAvailableMemory();
		/**
		 * @LEVEL2
		 * @brief 可用内存，startMemoryMonitor有效
		 */
		static string availableMemory;
	};
	class CPU {
		static void startCpuMonitor(int ms=100);
		static void stopMemoryMonitor();
		static double cpuUsage;
	};
};

/**
 * @brief 输入设备类
 */
class INDEV {
public:
	static void deleteIndev(int type);//删除输入设备：1~指针(包括触摸、鼠标、硬按键) 2~键盘 3~硬按键 4~编码器
	static void enableIndev(int type,bool en);//启用或禁用某类型的输入设备:1~指针(包括触摸、鼠标、硬按键) 2~键盘 3~硬按键 4~编码器
	static void enableExtDriver();//启用全部驱动(键盘，外部按键，编码器)
	static void disableExtDriver();//禁用全部驱动(键盘，外部按键，编码器)
	static void encoderNext();//通过编码器切换到下一个焦点控件
	static void encoderPrev();//通过编码器切换到上一个焦点控件
	static void encoderEnter();//通过编码器进入或执行当前焦点控件
};

class KEYPADER {
public:
	static void setKeycodeType(int i);//设置键码类型：0~原sdl键码 1~sdl原始扫描码 2~stt外部按键顺序(见ide控件属性)
	static void enableInputMode(bool en);//对于按下一个键时，即响应了Buttoner/Encoder事件，同时又在输入框上响应了输入事件,此时可能需要此方法，关闭输入模式
};

/**
 * @brief 输入设备-外部按键
 * @note 如要使用外部按键，必须启用编码器，因为外部按键对应的控件，通过编码器的组计算得到
 */
class BUTTONER {
public:
	static void resetPoints();//根据默认group，计算button_points
	static void sendId(int id);//发送外部键码索引-与编码器组中的控件顺序一致
	static bool enabled;//是否启用外部按键标记
	static void enableButtoner(bool b);//是否启用外部按键。如为true，会自动启用编码器。
	static void sendCode(uint8_t code);//发送键码：通过键码找到控件->找到组中的id->发送id
};

/**
 * @brief 输入设备编码器
 */
class ENCODER {
public:
	static void sendKey(uint32_t key);//设置编码器当前的按键(0~无按键 1~确认键 2~上一个 3~下一个)
	static void sendChar(uint32_t c);//向焦点控件发送一个控制字符
	static void addObject(STTWidget* w, STTPage* frm = NULL);//将控件加入编码器控制的组
	static void addObjects(vector<void*> vec);//将lv对象加入编码器控制的组
	static void addObjects(vector<STTWidget*> vec);//将lv对象加入编码器控制的组
	static void removeObject(STTWidget* w);//将控件移出编码器控制的组
	static vector<void*> clearObjects();//将所有控件移出编码器控制的组，返回清除前的列表
	static void focusObject(STTWidget* w);//选中指定控件
	static void focusNext();//选中下一个，同sendKey(3)
	static void focusPrev();//选中上一个，同sendKey(2)
	static void enter();//确认，同sendKey(1)
	static void focusFreeze(bool b);//是否选定冻结(冻结时，不能选中其他控件)
	static void showCursor(bool b);//是否显示光标(默认关闭)
	static void wrap(bool b);//是否循环选择，默认为true（是：当前选中最后一个，再点下一个，则回到第一个）
	static void editing(bool b);//手动设置是否编辑模式 0~浏览模式 1~编辑模式
	static STTWidget* getFocused();//获取选中的STTWidget
	static void addIme();//把输入法加入编码器控制的组
	static void removeIme();//把输入法移出编码器控制的组
	static void focusToImeOkButton();//在打开输入法的情况下，直接移动到OK键上
	static bool enabled;//是否启用编码器标记
	static void enableEncoder(bool b);//是否启用编码器
	static int lastKey;
	static void setCursorType(int type);//0:箭头指针 1:手指指针
	static void showOutline(bool b);//是否给焦点控件设置轮廓，默认没有轮廓
	static void setOutline(int color, int width);//默认红色，宽2
	static int color;//轮廓颜色
	static int width;//轮廓宽度
	static bool isShowOutline;//是否显示轮廓
	static void showWidgetOutline(STTWidget* w);//将轮廓设置应用到指定控件上(内部调用)
	static void openImeOnce(bool b);
	static bool isImeOnce();
	static void reset();
};

/**
 * @brief 数字键盘
 */
class NumPad {
public:
	enum  : uint8_t {
        KEY_0       = '0',      /**< 数字键 0 (ASCII 0x30) */
        KEY_1       = '1',      /**< 数字键 1 (ASCII 0x31) */
        KEY_2       = '2',      /**< 数字键 2 (ASCII 0x32) */
        KEY_3       = '3',      /**< 数字键 3 (ASCII 0x33) */
        KEY_4       = '4',      /**< 数字键 4 (ASCII 0x34) */
        KEY_5       = '5',      /**< 数字键 5 (ASCII 0x35) */
        KEY_6       = '6',      /**< 数字键 6 (ASCII 0x36) */
        KEY_7       = '7',      /**< 数字键 7 (ASCII 0x37) */
        KEY_8       = '8',      /**< 数字键 8 (ASCII 0x38) */
        KEY_9       = '9',      /**< 数字键 9 (ASCII 0x39) */

        // 大写字母键 (ASCII字符)
        KEY_A       = 'A',      /**< 字母键 A (ASCII 0x41) */
        KEY_B       = 'B',      /**< 字母键 B (ASCII 0x42) */
        KEY_C       = 'C',      /**< 字母键 C (ASCII 0x43) */
        KEY_D       = 'D',      /**< 字母键 D (ASCII 0x44) */
        KEY_E       = 'E',      /**< 字母键 E (ASCII 0x45) */
        KEY_F       = 'F',      /**< 字母键 F (ASCII 0x46) */
        KEY_G       = 'G',      /**< 字母键 G (ASCII 0x47) */
        KEY_H       = 'H',      /**< 字母键 H (ASCII 0x48) */
        KEY_I       = 'I',      /**< 字母键 I (ASCII 0x49) */
        KEY_J       = 'J',      /**< 字母键 J (ASCII 0x4A) */
        KEY_K       = 'K',      /**< 字母键 K (ASCII 0x4B) */
        KEY_L       = 'L',      /**< 字母键 L (ASCII 0x4C) */
        KEY_M       = 'M',      /**< 字母键 M (ASCII 0x4D) */
        KEY_N       = 'N',      /**< 字母键 N (ASCII 0x4E) */
        KEY_O       = 'O',      /**< 字母键 O (ASCII 0x4F) */
        KEY_P       = 'P',      /**< 字母键 P (ASCII 0x50) */
        KEY_Q       = 'Q',      /**< 字母键 Q (ASCII 0x51) */
        KEY_R       = 'R',      /**< 字母键 R (ASCII 0x52) */
        KEY_S       = 'S',      /**< 字母键 S (ASCII 0x53) */
        KEY_T       = 'T',      /**< 字母键 T (ASCII 0x54) */
        KEY_U       = 'U',      /**< 字母键 U (ASCII 0x55) */
        KEY_V       = 'V',      /**< 字母键 V (ASCII 0x56) */
        KEY_W       = 'W',      /**< 字母键 W (ASCII 0x57) */
        KEY_X       = 'X',      /**< 字母键 X (ASCII 0x58) */
        KEY_Y       = 'Y',      /**< 字母键 Y (ASCII 0x59) */
        KEY_Z       = 'Z',      /**< 字母键 Z (ASCII 0x5A) */

        // 小写字母键 (ASCII字符)
        KEY_a       = 'a',      /**< 字母键 a (ASCII 0x61) */
        KEY_b       = 'b',      /**< 字母键 b (ASCII 0x62) */
        KEY_c       = 'c',      /**< 字母键 c (ASCII 0x63) */
        KEY_d       = 'd',      /**< 字母键 d (ASCII 0x64) */
        KEY_e       = 'e',      /**< 字母键 e (ASCII 0x65) */
        KEY_f       = 'f',      /**< 字母键 f (ASCII 0x66) */
        KEY_g       = 'g',      /**< 字母键 g (ASCII 0x67) */
        KEY_h       = 'h',      /**< 字母键 h (ASCII 0x68) */
        KEY_i       = 'i',      /**< 字母键 i (ASCII 0x69) */
        KEY_j       = 'j',      /**< 字母键 j (ASCII 0x6A) */
        KEY_k       = 'k',      /**< 字母键 k (ASCII 0x6B) */
        KEY_l       = 'l',      /**< 字母键 l (ASCII 0x6C) */
        KEY_m       = 'm',      /**< 字母键 m (ASCII 0x6D) */
        KEY_n       = 'n',      /**< 字母键 n (ASCII 0x6E) */
        KEY_o       = 'o',      /**< 字母键 o (ASCII 0x6F) */
        KEY_p       = 'p',      /**< 字母键 p (ASCII 0x70) */
        KEY_q       = 'q',      /**< 字母键 q (ASCII 0x71) */
        KEY_r       = 'r',      /**< 字母键 r (ASCII 0x72) */
        KEY_s       = 's',      /**< 字母键 s (ASCII 0x73) */
        KEY_t       = 't',      /**< 字母键 t (ASCII 0x74) */
        KEY_u       = 'u',      /**< 字母键 u (ASCII 0x75) */
        KEY_v       = 'v',      /**< 字母键 v (ASCII 0x76) */
        KEY_w       = 'w',      /**< 字母键 w (ASCII 0x77) */
        KEY_x       = 'x',      /**< 字母键 x (ASCII 0x78) */
        KEY_y       = 'y',      /**< 字母键 y (ASCII 0x79) */
        KEY_z       = 'z',      /**< 字母键 z (ASCII 0x7A) */

        // 标点符号和特殊字符 (ASCII字符)
        KEY_Point  = '.',      /**< 句点 . (ASCII 0x2E) */

        // 功能键 (使用自定义值)
        KEY_BACK    = 0x08,     /**< 退格键 BackSpace (ASCII BS) */
        KEY_TAB     = 0x09,     /**< 制表键 Tab (ASCII HT) */
        KEY_ENTER   = 0x0D,     /**< 回车键 Enter (ASCII CR) */
        KEY_ESC     = 0x1B,     /**< 退出键 Escape (ASCII ESC) */
        KEY_DEL     = 0x7F,     /**< 删除键 Delete (ASCII DEL) */
        KEY_SIGN    = 0xB0,     /**< 正负号切换键 +/- */
        KEY_CANCEL  = 0xB1,     /**< 取消键 CANCEL */
	};
	/**
	 * @LEVEL2
	 * @brief 发送键值
	 * @param code 键值
	 */
	static void sendCode(uint8_t key);
};

#define LOGD(...) DEBUG::LOG(0, __FILE__, __LINE__, __VA_ARGS__)

}
