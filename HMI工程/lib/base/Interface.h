/*
 * 定义基础类库常用接口、参数
 *
 *  Created on: 2023年2月8日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include "core/SttSignal.h"
#include "core/SttObject.h"
#include "Constants.h"

#ifndef interface
#define interface class
#endif
#define implements public

#define _L(...) localeLaungauge(__VA_ARGS__)
#define _V(...) vm->get(__VA_ARGS__)
#define _W(...) vm->getDvcVar(__VA_ARGS__)
#define VM (*vm)

using namespace std;
using namespace stt;
using namespace stt::constant;

/**
 * @namespace stt
 * @brief 鑫通态主命名空间
 * @image apuside-32.png
 */
namespace stt{

//前置定义
class STTWidget;
class STTPage;
interface IDevice;
struct LvEvent;
class VarMgrBase;
interface IAsynWorker;
class HmiAppBase;

typedef void (SttObject::*EHandler)(uint16_t code, LvEvent e);

//一些前置类，对应lvgl一些结构体
struct LvEvent {
	STTWidget* target;
	STTWidget* currentTarget;
    uint16_t eventCode;
    void * user_data;
    void * param;
    uint8_t deleted:1;
    uint8_t stopProcessing:1;
    uint8_t stopBubbling:1;
};

/**
 * @brief 事件处理入口类
 */
class EHandlerEntry{
public:
	uint16_t code;
	SttObject* object;
	EHandler handler;
};

/**
 * @brief 键值对 整型-字符串
 */
struct KV_Int_Str{
public:
	int k;
	string v;
};
/**
 * @brief 键值对 字符串-字符串
 */
struct KV_Str_Str{
public:
	string k;
	string v;
};
/**
 * @brief 键值对 整型-整型
 */
struct KV_Int_Int{
public:
	int k;
	int v;
};

/**
 * @brief 面积结构体
 */
struct Area{
	int w;
	int h;
};

/**
 * @brief 消息类
 */
class AnyMessage {
public:
    template<typename T>
    AnyMessage(T& value) : data_(make_unique<Holder<T>>(std::move(value))) {}

    template<typename T>
    T get() const {
        if(typeid(T) != data_->type_info()) {
            throw std::runtime_error("Bad any cast");
        }
        return static_cast<Holder<T>*>(data_.get())->value_;
    }

private:
    AnyMessage() = default;
    struct BaseHolder {
        virtual ~BaseHolder() = default;
        virtual const std::type_info& type_info() const = 0;
    };

    template<typename T>
    struct Holder : BaseHolder {
        T value_;

        Holder(T value) : value_(std::move(value)) {}
        const std::type_info& type_info() const override { return typeid(T); }
    };

    template<typename T, typename... Args>
    std::shared_ptr<T> make_unique(Args&&... args) {
        return std::shared_ptr<T>(new T(std::forward<Args>(args)...));
    }

    std::shared_ptr<BaseHolder> data_;
};

/**
 * @brief 可变类型
 */
class Variant {
public:
	enum Type{
		STRING,
		BOOL,
		INT8,
		UINT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		INT64,
		UINT64,
		FLOAT,
		DOUBLE,
		NONE
	};
public:
	Variant();
	Variant(const bool& value);
	Variant(const int8_t& value);
	Variant(const uint8_t& value);
	Variant(const int16_t& value);
	Variant(const uint16_t& value);
	Variant(const int32_t& value);
	Variant(const uint32_t& value);
	Variant(const int64_t& value);
	Variant(const uint64_t& value);
	Variant(const float& value);
	Variant(const double& value);
	Variant(const string& value);
	virtual ~Variant();

	virtual uint8_t getType() const;
	virtual void setType(uint8_t& t);
    virtual int size() const;
    virtual void set(const bool& v);
    virtual void set(const int8_t& v);
    virtual void set(const uint8_t& v);
    virtual void set(const int16_t& v);
    virtual void set(const uint16_t& v);
    virtual void set(const int32_t& v);
    virtual void set(const uint32_t& v);
    virtual void set(const float& v);
    virtual void set(const int64_t& v);
    virtual void set(const uint64_t& v);
    virtual void set(const double& v);
    virtual void set(const string& v);
    virtual void get(bool& v);
    virtual void get(int8_t& v);
    virtual void get(uint8_t& v);
    virtual void get(int16_t& v);
    virtual void get(uint16_t& v);
    virtual void get(int32_t& v);
    virtual void get(uint32_t& v);
    virtual void get(float& v);
    virtual void get(int64_t& v);
    virtual void get(uint64_t& v);
    virtual void get(double& v);
    virtual void get(string& v);
    virtual string toString();
public:
	Variant& operator=(const bool& value);
	Variant& operator=(const int8_t& value);
	Variant& operator=(const uint8_t& value);
	Variant& operator=(const int16_t& value);
	Variant& operator=(const uint16_t& value);
	Variant& operator=(const int32_t& value);
	Variant& operator=(const uint32_t& value);
	Variant& operator=(const int64_t& value);
	Variant& operator=(const uint64_t& value);
	Variant& operator=(const float& value);
	Variant& operator=(const double& value);
	Variant& operator=(const string& value);
	Variant(const Variant& other);
	Variant& operator=(const Variant& other);

	Variant(Variant&& other) noexcept;
	Variant& operator=(Variant&& other) noexcept;
public:
    uint8_t mt;
    void* mv;
};

/**
 * @brief 普通变量类
 */
class IVariable: public SttObject {
public:
	/**@HideCA*/
	IVariable(const string& name);
	/**@HideCA*/
	virtual ~IVariable();
public:
	/**
	 * @LEVEL2
	 * @brief 获取变量名
	 * @return 变量名称
	 */
	string getName();
	/**
	 * @LEVEL2
	 * @brief 将变量值转换为字符串
	 * @param frac 小数位(仅浮点型有意义)
	 * @return 变量的字符串值
	 */
	virtual string toStr(uint8_t frac = 0);
	/**
	 * @LEVEL2
	 * @brief 	用字符串值设置变量
	 * 			自动转换为变量匹配的类型
	 * @param newValue 新值
	 */
	virtual void set(string newValue);
	/**
	 * @LEVEL2
	 * @brief 		字符串转变量并指定来源
	 * @param org	来源对像（用于在设置值时，如来源对像与当前对像一致，则不发出消息）
	 * @param newValue	新值
	 */
	virtual void setWith(SttObject * org, string newValue);
	/**
	 * @HideCA
	 * @brief	只用于变量之间的连接
	 * @param org	来源对像（用于在设置值时，如来源对像与当前对像一致，则不发出消息）
	 * @param newValue 来源变量
	 */
	virtual void setWith2(SttObject * org, IVariable* newValue);
	/**
	 * @LEVEL2
	 * @brief 获取变量类型
	 * @return 见[常量DataType]
	 */
	virtual uint8_t type();
	/**
	 * @LEVEL2
	 * @brief 获取变量长度(单位：字节)
	 */
	virtual int size();
	/**
	 * @brief 	是否通知
	 * @return
	 */
	bool getNotifier();
	/**
	 * @brief	启用/关闭通知
	 * @param f	启用/关闭
	 */
	void setNotifier(bool f);
	/**
	 * @brief	启用/关闭变量是否自动保存
	 * @param 	启用/关闭
	 */
	void setSavable(bool b);
	/**
	 * @brief	是否自动保存
	 */
	bool isSavable();
	/**
	 * @HideCA
	 * 信号槽
	 */
	stt::SttSignal<SttObject*, IVariable*> valueChanged;
    /**
     * @LEVEL2
     * @brief 触发函数
     * @param code
     * @param object
     * @param handler
     */
    void onTriHandler(uint16_t code, SttObject* object, EHandler handler);

public:
    virtual operator bool();
    virtual operator int();
    virtual operator float();
    virtual operator int64_t();
    virtual operator double();
    virtual IVariable& operator=(int v);
    virtual IVariable& operator=(int64_t v);
    virtual IVariable& operator=(float v);
    virtual IVariable& operator=(double v);
    virtual int operator+(int v);
    virtual int64_t operator+(int64_t v);
    virtual float operator+(float v);
    virtual double operator+(double v);
    virtual double operator+(IVariable& v);
    virtual int operator-(int v);
    virtual int64_t operator-(int64_t v);
    virtual float operator-(float v);
    virtual double operator-(double v);
    virtual double operator-(IVariable& v);
    virtual int operator*(int v);
    virtual int64_t operator*(int64_t v);
    virtual float operator*(float v);
    virtual double operator*(double v);
    virtual double operator*(IVariable& v);
    virtual int operator/(int v);
    virtual int64_t operator/(int64_t v);
    virtual float operator/(float v);
    virtual double operator/(double v);
    virtual double operator/(IVariable& v);
    virtual int operator%(int v);
    virtual int64_t operator%(int64_t v);
    virtual float operator%(float v);
    virtual double operator%(double v);
    virtual double operator%(IVariable& v);
    bool operator==(IVariable& other);
    bool operator!=(IVariable& other);
    bool operator<(IVariable& other);
    bool operator<=(IVariable& other);
    bool operator>(IVariable& other);
    bool operator>=(IVariable& other);
    virtual bool operator==(int v);
    virtual bool operator==(int64_t v);
    virtual bool operator==(float v);
    virtual bool operator==(double v);
    virtual bool operator!=(int v);
    virtual bool operator!=(int64_t v);
    virtual bool operator!=(float v);
    virtual bool operator!=(double v);
    virtual bool operator<(int v);
    virtual bool operator<(int64_t v);
    virtual bool operator<(float v);
    virtual bool operator<(double v);
    virtual bool operator<=(int v);
    virtual bool operator<=(int64_t v);
    virtual bool operator<=(float v);
    virtual bool operator<=(double v);
    virtual bool operator>(int v);
    virtual bool operator>(int64_t v);
    virtual bool operator>(float v);
    virtual bool operator>(double v);
    virtual bool operator>=(int v);
    virtual bool operator>=(int64_t v);
    virtual bool operator>=(float v);
    virtual bool operator>=(double v);

    virtual IVariable& offset(int v);
    virtual bool bit(int bits);
    virtual void setBit(int bits, bool v);

protected:
	/**@HideCA 启用/关闭通知*/
	uint8_t _n_ : 1;
	/**@HideCA 启用/关闭自动保存*/
	uint8_t _s_ : 1;
	/**@HideCA 类型*/
	uint8_t m_type : 6;
	/**@HideCA 变量名*/
	string _m_;
public:
	IVariable* connSource;//避免变量连接时的震荡
};
typedef IVariable* Ivariable;

int operator+(int other, IVariable& num);
int64_t operator+(int64_t other, IVariable& num);
float operator+(float other, IVariable& num);
double operator+(double other, IVariable& num);
int operator-(int other, IVariable& num);
int64_t operator-(int64_t other, IVariable& num);
float operator-(float other, IVariable& num);
double operator-(double other, IVariable& num);
int operator*(int other, IVariable& num);
int64_t operator*(int64_t other, IVariable& num);
float operator*(float other, IVariable& num);
double operator*(double other, IVariable& num);
int operator/(int other, IVariable& num);
int64_t operator/(int64_t other, IVariable& num);
float operator/(float other, IVariable& num);
double operator/(double other, IVariable& num);
int operator%(int other, IVariable& num);
int64_t operator%(int64_t other, IVariable& num);
float operator%(float other, IVariable& num);
double operator%(double other, IVariable& num);
bool operator==(int other, IVariable& num);
bool operator==(int64_t other, IVariable& num);
bool operator==(float other, IVariable& num);
bool operator==(double other, IVariable& num);
bool operator!=(int other, IVariable& num);
bool operator!=(int64_t other, IVariable& num);
bool operator!=(float other, IVariable& num);
bool operator!=(double other, IVariable& num);
bool operator<(int other, IVariable& num);
bool operator<(int64_t other, IVariable& num);
bool operator<(float other, IVariable& num);
bool operator<(double other, IVariable& num);
bool operator<=(int other, IVariable& num);
bool operator<=(int64_t other, IVariable& num);
bool operator<=(float other, IVariable& num);
bool operator<=(double other, IVariable& num);
bool operator>(int other, IVariable& num);
bool operator>(int64_t other, IVariable& num);
bool operator>(float other, IVariable& num);
bool operator>(double other, IVariable& num);
bool operator>=(int other, IVariable& num);
bool operator>=(int64_t other, IVariable& num);
bool operator>=(float other, IVariable& num);
bool operator>=(double other, IVariable& num);

/**
 * 设备变量类
 */
class IDvcVariable : public IVariable {
public:
	/**@HideCA*/
	IDvcVariable(const string& name, const string& setting);
	/**@HideCA*/
	virtual ~IDvcVariable();
public:
	/**
	 * @brief 获取变量读写属性
	 * @return
	 */
	virtual uint8_t getAtti();
	/**
	 * @brief 设置变量读写属性
	 * @param state
	 * @return 0:读写 1：只读 2：只写 3:暂停读写
	 */
	virtual uint8_t setAtti(uint8_t state);
	/**
	 * @brief 设置
	 * @return true/false
	 */
	virtual bool setSettings(const string& setting);
	/**
	 * @brief 分配到指定设备
	 * @return
	 */
	virtual uint8_t assignTo(IDevice* v);
	/**
	 * @brief 取所属设备
	 * @return
	 */
	virtual IDevice* getDevice();
	/**
	 * @brief 取软元件类型
	 * @return
	 */
	virtual string getComponent();
	/**
	 * @brief 设置软元件
	 * @param value
	 */
	virtual void setComponent(const string& value);
	/**
	 * @brief 取元件地址
	 * @return
	 */
	virtual string getAddress();
	/**
	 * @brief 设置元件地址
	 * @param value
	 */
	virtual void setAddress(const string& value);
	/**
	 * @brief 取格式
	 * @return
	 */
	virtual string getFrmt();
	/**
	 * @brief 启用/禁用变量驻留
	 * @return
	 */
	virtual void setReside(bool b);
	/**
	 * @brief 是否驻留
	 * @return
	 */
	virtual bool isReside();
	/**
	 * @brief 获取变量读取状态
	 * @return
	 */
	virtual bool getOnline();
	/**
	 * @brief 获取变量已读
	 * @return
	 */
	virtual bool getHaveRead();
	/**
	 * @brief 设置变量已读
	 * @return
	 */
	virtual void setHaveRead(bool sta);
	/**
	 * @brief 写操作
	 * @param v
	 */
	virtual void write(vector<uint8_t>& v);
	/**
	 * @brief 写操作
	 * @param v
	 */
	virtual void write(void * v, bool force);
	/**
	 * @brief 字节写操作1
	 * @param buff
	 * @param len
	 */
	virtual void writeFromByteArray1(unsigned char* buff,int len);
	/**
	 * @brief 字节写操作2
	 * @param buff
	 * @param len
	 */
	virtual void writeFromByteArray2(unsigned char* buff,int len);
	/**
	 * @brief 设备变量值转字节(双字节序)
	 * @param buff 转换后保存在buff中
	 * @return 返回转换后的字节长度
	 */
	virtual int toByteArray2(unsigned char* buff);
	/**
	 * @brief 获取_w_值的指针
	 */
	virtual unsigned char* getPointerW();
protected:
	/** @HideCA*/
	uint8_t _r_ : 1;
	/** @HideCA*/
	uint8_t _onLine_ : 1;
	/** @HideCA*/
	uint8_t _haveRead_ : 1;
	/**读写属性0:读写 1：只读 2：只写 3:暂停读写*/
	uint8_t _att_ : 5;
	/** @HideCA*/
	IDevice* _dvc_;
	/** @HideCA*/
	string _c_;
	/** @HideCA*/
	string _a_;
	/** @HideCA*/
	string _f_;
};
typedef IDvcVariable* IdvcVariable;

/**
 * @brief 设备接口
 */
interface IDevice{
public:
	/**
	 * @HideCA
	 */
	virtual ~IDevice(){};
	/**
	 * @brief 注册设备变量
	 * @param v
	 * @return
	 */
	virtual bool regVar(IDvcVariable *v)=0;
	/**
	 * @brief 获取设备变量列表
	 * @return
	 */
	virtual list<IdvcVariable> getVarList()=0;
	/**
	 * @brief 获取设置字符串
	 * @return
	 */
	virtual string getSettings() = 0;
	virtual bool setSettings(const string& settings) = 0;
	virtual string getSetting(string key) = 0;
	/**
	 * @brief 获取设备ID
	 * @return
	 */
	virtual string getId() = 0;
	/**
	 * @brief 获取驱动名称
	 * @return
	 */
	virtual string getDllName() = 0;
	/**
	 * @brief 获取通讯接口（RS232/RS485-2W/eth0...）
	 * @return
	 */
	virtual string getIf() = 0;
	/**
	 * @brief 获取通讯接口名称(COM1/COM2/IP...)
	 * @return
	 */
	virtual string getIfName() = 0;
	/**
	 * @brief 设置设备状态
	 * @return
	 */
	virtual uint8_t setDevState(uint8_t sta) = 0;
	/**
	 * @brief 取设备运行状态
	 * @return 0~离线 1~在线 2~通讯异常 3~使能关闭
	 */
	virtual uint8_t getDevState() = 0;
	/**
	 * @brief 使能设备
	 * @return
	 */
	virtual uint8_t start() = 0;
	/**
	 * @brief 停止设备
	 * @return
	 */
	virtual uint8_t stop() = 0;
	/**
	 * @brief 获取设备使能状态
	 * @return
	 */
	virtual bool getEnableState() = 0;
	/**
	 * @brief 获取设备是否队列写
	 * @return
	 */
	virtual bool isQueue()=0;
	/**
	 * @brief 获取站号
	 * @return
	 */
	virtual uint16_t getStationNo()=0;
	/**
	 * @brief 设置站号
	 */
	virtual void setStationNo(int n)=0;
	/**
	 * @brief 获取设备是否启用校验和
	 * @return
	 */
	virtual bool getVerifyCode()=0;
	/**
	 * @brief 获取协议格式
	 * @return
	 */
	virtual bool getProtocolFormat()=0;
	/**
	 * 获取通讯超时
	 * @return
	 */
	virtual uint16_t getComDelay()=0;
	/**
	* @brief 获取最大读取长度
	* @return
	*/
	virtual uint16_t getMaxReadLen()=0;
	/**
	 * @brief 最大写入字数
	 */
	virtual uint16_t getMaxWriteLen()=0;
	/**
	 * @brief 地址整段间隔
	 */
	virtual uint16_t getMaxAddrInterval()=0;
	/**
	 * @brief 获取帧延时
	 * @return
	 */
	virtual uint16_t getFramDelay()=0;
	/**
	 * @brief 获取Int16顺序
	 * 0-12
	 * 1-21
	 * @return
	 */
	virtual uint8_t getSeqInt16()=0;
	/**
	 * @brief 获取Int32顺序
	 * 0-1234
	 * 1-2143
	 * 2-3412
	 * 3-4321
	 * @return
	 */
	virtual uint8_t getSeqInt32()=0;
	/**
	 * @brief 获取长整型顺序
	 * 0-21小端模式
	 * 1-12大端模式
	 * @return
	 */
	virtual uint8_t getSeqLong()=0;
	/**
	 * @brief 获取Float顺序
	 * 0-1234
	 * 1-2143
	 * 2-3412
	 * 3-4321
	 * @return
	 */
	virtual uint8_t getSeqFloat()=0;
	/**
	 * @brief 获取Asc顺序
	 * 0-21
	 * 1-12
	 * @return
	 */
	virtual uint8_t getSeqAsc()=0;
	/**
	 * @brief 设备变量信号槽
	 */
	stt::SttSignal<std::shared_ptr<IDvcVariable>> writeValue;
public:
	atomic_bool	refreshVar;//刷新变量标记，跳转页面时置一，驱动重新合并元件
};
typedef IDevice* Idevice;

/**
 * @brief 设备驱动接口
 */
interface IDvcDriver{
public:
	virtual ~IDvcDriver(){};
	/**
	 * @brief 添加设备
	 * @param dvc
	 */
	virtual void add(Idevice dvc) = 0;
	/**
	 * @brief 获取设备驱动设置信息
	 * @return
	 */
	virtual string getSettings() = 0;
	/**
	 * @brief 获取设备驱动设置信息
	 * @return
	 */
	virtual string getSetting(string key) = 0;
	/**
	 * @brief 获取驱动名称
	 * @return
	 */
	virtual string getDllName() = 0;
	/**
	 * @brief 获取通讯接口（RS232/RS485-2W/eth0...）
	 * @return
	 */
	virtual string getIf() = 0;
	/**
	 * @brief 获取通讯接口名称(COM1/COM2/IP...)
	 * @return
	 */
	virtual string getIfName() = 0;
	/**
	 * @brief 写数据
	 * @param pData
	 * @param len
	 * @return
	 */
	virtual bool postData(std::shared_ptr<IDvcVariable> d) = 0;
	/**
	 * @brief 使能设备
	 * @return
	 */
	virtual bool start() = 0;
	/**
	 * @brief 停止设备
	 * @return
	 */
	virtual bool stop() = 0;
	/**
	 * 重连
	 * @return
	 */
	virtual bool reconnect()=0;
	/**
	 * @brief 设置驱动
	 * @param dll
	 */
	virtual void setDll(void * dll)=0;
	/**
	 * @brief 获取驱动
	 */
	virtual void* getDll()=0;
};
typedef IDvcDriver* IdvcDriver;

/**
 * @brief 设备管理器接口
 */
interface IDeviceManager {
public:
	/**
	 * @HideCA
	 */
	virtual ~IDeviceManager(){}
	/**
	 * @HideCA
	 * @brief 添加设备
	 * @param name 设备名称(用户自定义，唯一)
	 * @param settings 设备配置, json格式。
	 * @return
	 */
	virtual Idevice addDevice(const string& name, const string& settings) throw() = 0;
	/**
	 * @brief 获取设备
	 * @param name	设备名称
	 * @return	返回设备指针
	 */
	virtual Idevice getDevice(const string& name) = 0;
	/**
	 * @brief 获取设备驱动
	 * @param name	通讯接口名称(COM1/COM2/IP..)
	 * @return	返回设备驱动指针
	 */
	virtual IdvcDriver getDvcdriver(const string& name) = 0;
	/**
	 * @brief 设备采集开关
	 * @param name	 设备名称
	 * @param enable 设备使能
	 * @return	返回设备
	 */
	virtual Idevice enableDevice(const string& name, bool enable) = 0;
	/**
	 * @brief 启动设备
	 */
	virtual void run() = 0;
	/**
	 * @brief 刷新设备
	 */
	virtual void setDevsRefreshFlag()=0;

};
typedef IDeviceManager* IDvcMgr;

/**
 * 页面接口
 */
interface IPage {
public:
	virtual ~IPage();
	/**
	 * @brief 页面构造前初始化
	 */
	virtual void onInit();
	/**
	 * @brief 创建控件
	 *
	 * @param container	窗体对象自身
	 */
	virtual void createParts();
	/**
	 * 页面创建后
	 */
	virtual void onLoad();
	/**
	 * @brief 关闭当前页面
	 */
	virtual void close();
	/**
	 * @brief 页面关闭前检查
	 * @return	false：阻止关闭  true:不阻止关闭
	 */
	virtual bool onClosing();
	/**
	 * @brief 页面销毁后处理
	 */
	virtual void onDispose();
	/**
	 * @brief 刷新显示
	 */
	virtual void display();
};
typedef IPage* IPagePtr;

/**
 * 变量管理器
 */
interface IVarManager {
public:
	virtual ~IVarManager(){}
	/**
	 * @brief 添加变量
	 * @param name	名称(用户自定义名称，唯一)
	 * @param type	设备变量类型(如：stt::DataType::D_INT)
	 * @param setting 设备变量配置(如：{"c":"06","a":"0","f":"1"}   c是元件(16进制), a是地址(10进制),f是格式)
	 * @return 返回指定type的设备变量
	 */
	virtual Ivariable add(const string& name, uint8_t type, const string& setting="") throw()=0;
	/**
	 * @brief 注册变量
	 * @param var	变量
	 * @return 变量指针
	 */
	virtual bool reg(Ivariable var) throw()=0;
	/**
	 * @brief 获取变量
	 * @param name
	 * @return 变量指针
	 */
	virtual Ivariable get(const string& name)=0;
	/**
	 * @brief 删除变量
	 * @param name
	 * @return 变量指针
	 */
	virtual bool remove(const string& name) = 0;
	/**
	 * @brief 清除所有变量
	 * @return
	 */
	virtual bool clearAll() = 0;
	/**
	 * @brief 保存所有(savable)变量
	 * @return
	 */
	virtual bool saveAll() = 0;
	/**
	 * @brief		获取变量float值
	 * @param name	变量名称
	 * @param v		赋值对象
	 * @param imm	实时读取（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool getFloat(const string& name, float& v, bool immediate = false) = 0;
	/**
	 * @brief		获取变量int值
	 * @param name	变量名称
	 * @param v		赋值对象
	 * @param imm	实时读取（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool getInt(const string& name, int& v, bool immediate = false) = 0;
	/**
	 * @brief		获取变量long值
	 * @param name	变量名称
	 * @param v		赋值对象
	 * @param imm 	实时读取（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool getLong(const string& name, int64_t& v, bool immediate = false) = 0;
	/**
	 * @brief		获取变量char值
	 * @param name	变量名称
	 * @param v		赋值对象
	 * @param imm 	实时读取（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool getChar(const string& name, string& v, bool immediate = false) = 0;
	/**
	 * @brief		获取配方数据值
	 * @param name	变量名称
	 * @param row	行索引
	 * @param col	列索引
	 * @param v		赋值对象
	 * @return		是否成功
	 */
	virtual bool getTrcp(const string& name, int rowIndex, int colIndex, string& v) = 0;
	/**
	 * @brief		设置变量float值
	 * @param name	变量名称
	 * @param v		值
	 * @param imm 	实时写入（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool setFloat(const string& name, float v, bool immediate = false) = 0;
	/**
	 * @brief		设置变量int值
	 * @param name	变量名称
	 * @param v		值
	 * @param imm 	实时写入（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool setInt(const string& name, int v, bool immediate = false) = 0;
	/**
	 * @brief		设置变量long值
	 * @param name	变量名称
	 * @param v		值
	 * @param imm 	实时写入（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool setLong(const string& name, int64_t v, bool immediate = false) = 0;
	/**
	 * @brief		设置变量char值
	 * @param name	变量名称
	 * @param v		值
	 * @param imm 	实时写入（设备变量有效）
	 * @return		是否成功
	 */
	virtual bool setChar(const string& name, const string& v, bool immediate = false) = 0;
	/**
	 * @brief		设置变量配方值
	 * @param name	变量名称
	 * @param row	行索引
	 * @param col	列索引
	 * @param v		值
	 * @return		是否成功
	 */
	virtual bool setTrcp(const string& name, int rowIndex, int colIndex, const string& v) = 0;
	/**
	 * @brief 变量值传递
	 * @param namefrom	源变量名称
	 * @param nameto 目标变量名称
	 * @param isbid 是否双向(false:仅源变化时引起目标变化, true:目标变化时也会引起源变化)
	 */
	virtual void connectVar(const string& namefrom, const string& nameto, bool isbid) = 0;

	/**
	 * @brief 消息订阅
	 * @param n			消息名
	 * @param obj		实例对象
	 * @param fun		类函数
	 */
	template<typename _Class, typename _Callable>
	void anyMsgSubscribe(string n, _Class *obj, const _Callable &fun){
		anyMsgList[n].connect(obj, fun);
	}

	/**
	 * @brief 消息发送
	 * @param n			消息名
	 * @param va		消息值
	 */
	template<typename T>
	void anyMsgPost(const string& n, T& va){
		anyMsgList[n].emit(std::make_shared<AnyMessage>(va));
	}

	/**
	 * @brief 消息订阅取消
	 * @param n			消息名
	 * @param obj		实例对象
	 * @param fun		类函数
	 */
	template<typename _Class, typename _Callable>
	void anyMsgUnsubscribe(string n, _Class *object_, const _Callable &fun_){
		anyMsgList[n].disconnect(object_, fun_);
	}
public:
	/**
	 * @brief 获取变量
	 * @param name
	 * @return 变量引用
	 */
	virtual IVariable& get2(const string& name)=0;
	virtual IVariable& operator[](const std::string& key)=0;
private:
	map<string, stt::SttSignal<std::shared_ptr<AnyMessage>>> anyMsgList;
};
typedef IVarManager* IVarMgr;

/**
 * @brief 多语言管理器
 */
interface ILanguageManager {
public:
	virtual ~ILanguageManager(){}

	/**
	 * @brief 添加标签
	 * @param tag	标签
	 * @return
	 */
	virtual bool addTag(const string& tag) = 0;

	/**
	 * @brief 添加语言
	 * @param locale			cn/en...
	 * @param defaultfont		默认字体库名称（fm已注册的）
	 */
	virtual bool addLocale(const string& locale, const string& defaultfont)=0;

	/**
	 * @brief 设置标签翻译
	 * @param tag		标签
	 * @param locale	语言
	 * @param trans		翻译信息
	 * @param font		字体库名称
	 * @return
	 */
	virtual bool setTagLocale(const string& tag, const string& locale, const string& trans, const string& font="") = 0;

	/**
	 * @brief标签是否存在
	 * @param tag
	 * @return
	 */
	virtual bool hasTag(const string& tag) = 0;

	/**
	 * @brief获取标签语言值
	 * @param tag
	 * @return
	 */
	virtual string getTag(const string& tag) = 0;

	/**
	 * @brief获取标签指定语言值
	 * @param tag		标签
	 * @param locale	语言
	 * @return
	 */
	virtual string getLocaleTag(const string& tag, const string& locale) = 0;

};
typedef ILanguageManager* ILanguageMgr;

/**
 * @brief对话框接口定义
 */
interface IDialog {
public:
	virtual ~IDialog();
	virtual IDialog* onEventHandler(uint16_t code, SttObject* object, EHandler handler);
	virtual void show();
	virtual IDialog* setBoxSize(int size);
	virtual IDialog* setFontSize(int fntSize);
	virtual IDialog* onOK(function<void()> f);
	virtual IDialog* onCancel(function<void()> f);
	virtual IDialog* onOpen(function<void()> f);
	virtual IDialog* onDispose(function<void()> f);
	virtual STTWidget* getRootWidget();
};
typedef IDialog* IDialogPtr;

/**
 * @brief 页面管理器接口定义
 */
interface IPageManager {
public:
	/**
	 * @HideCA
	 */
	virtual ~IPageManager(){}
	/**
	 * @brief 打开页面
	 * @param id		页面名称id, 默认空时打开首页. 如Frm01，id由组态自动生成，不可人为修改
	 * @param mode		模式，默认0-打开新页面
	 * @param target	父页面, 默认nullptr
	 * @return 失败时返回空指针
	 */
	virtual IPagePtr open(const string& id = "", int mode = PageMode::NEWWIN, STTWidget* target=nullptr) throw() = 0;
	/**
	 * 通过索引号打开页面
	 * @param idx 页面索引，由用户指定。默认为-1
	 */
	virtual IPagePtr open2(int idx, int mode = PageMode::NEWWIN, STTWidget* target=nullptr) throw() = 0;
	/**
	 * @brief 关闭指定页
	 * @param id
	 * @return
	 */
	virtual bool close(const string& id)=0;
	/**
	 * @brief 构建页面
	 * @param id
	 * @param mode
	 * @param target
	 * @return
	 */
	virtual IPagePtr build(const string& id = "", int mode = PageMode::NEWWIN, STTWidget* target=nullptr)=0;
	/**
	 * @brief 获取当前页（最上层页）
	 * @return nullptr或页面指针
	 */
	virtual STTPage* getCurrentPage()=0;
	/**
	 * @brief 从已打开页面中获取指定页
	 * @return nullptr或页面指针
	 */
	virtual STTPage* getPage(const string& id)=0;
	/**
	 * @brief 获取消息提示框
	 * @param msg		消息提示内容
	 * @param type		消息框类型
	 * @return
	 */
	virtual IDialogPtr getMessageBox(const string& msg="", int type=MSGBOX_TYPE::TIP) = 0;
	/**
	 * @brief 获取消息确认框
	 * @param msg		消息内容
	 * @return
	 */
	virtual IDialogPtr getConfirmBox(const string& msg="") = 0;
	/**
	 * @brief 获取输入框
	 * @param msg		消息内容
	 * @param content	文本框内容
	 * @return
	 */
	virtual IDialogPtr getInputBox(const string& msg = "" , const string& content = "") = 0;
	/**
	 * @brief 获取进度框(倒计时结束或者关联变量值达到100)
	 * @param msg			消息内容
	 * @param mode			进度框模式(0：倒计时，1：变量值变化)
	 * @param parameters	设置参数 (例如：倒计时模式时："5"[100的因数]  变量值变化模式时："变量名")
	 * @return
	 */
	virtual IDialogPtr getProgressBox(const string& msg = "", int mode=PROGRESS_TYPE::COUNT_DOWN, const string& parameters="") = 0;
	/**
	 * 获取文件选择对话框
	 * @param title		标题
	 * @return
	 */
	virtual IDialogPtr getFileBox(const string& title = "") = 0;
	/**
	 * @brief 获取用户登录
	 * @param title		标题
	 * @return
	 */
	virtual IDialogPtr getLoginBox(const string& title = "登录") = 0;
	/**
	 * @brief 获取用户注销提示框
	 * @param title		标题
	 * @return
	 */
	virtual IDialogPtr getLogoutBox(const string& title="注销", const string& tip="") = 0;
	/**
	 * @brief 指定屏保页
	 *
	 * @param id		页面名
	 * @param type		屏保类型 0：系统默认 1：用户定制(注：此页面需要在build中能返回)
	 * @param timeout	>=10(s), <10时关闭屏保
	 */
	virtual void setScreenSaver(const string& id = "", int type = 0, int timeout=0) = 0;
	/**
	 * @brief 设置熄屏参数
	 *
	 * @param t			延时时间（s），0-关闭熄屏功能
	 * @param light		熄屏亮度值(1-100)
	 */
	virtual void setBackLightHoldingTime(int t = 0, int light = 0) = 0;
	/**
	 * @brief 通知外部输入（如：触屏点击、键盘等）
	 *
	 * @param reset		复位
	 */
	virtual void notifyEvdevInput(bool reset=false) = 0;
	/**
	 * @brief 获取锁屏熄屏状态
	 * @return	0:非锁屏或熄屏	1:锁屏或熄屏状态
	 */
	virtual int getScreenSaverStatus() = 0;
	/**
	 * @brief 获取屏保类型
	 * @return	屏保类型 0：普通 1：需认证
	 */
	virtual int getScreenSaverType() = 0;
	/**
	 * 映射索引值-页面id，用于通过索引值打开页面
	 * @param idx 大于等于0的索引值
	 * @param frmid 页面id，如Frm01
	 */
	virtual void mapping(int idx, const string& frmid) = 0;
	/**
	 * 绑定切换页面的变量，变量变化时自动调用open2(idx)进行切换
	 */
	virtual void bindSwitchPageVar(Ivariable var) = 0;
	/**
	 * 添加驻留页面
	 * @param pageId 页面名
	 */
	virtual void addResidentPage(const string& pageId) = 0;
	/**
	 * 移除驻留页面
	 * @param pageId 页面名
	 */
	virtual void removeResidentPage(const string& pageId) = 0;
	/**
	 * 清理缓存
	 */
	virtual void clearCache() = 0;
};
typedef IPageManager* IPageMgr;

/**
 * @brief 字体管理器接口
 */
interface IFontManager {
public:
	/**
	 * @HideCA
	 */
	virtual ~IFontManager();
	/**@HideCA
	 * @brief 注册字体文件
	 *
	 */
	virtual bool registerFontFile(const string& name, const string& path);
	/**@LEVEL0
	 * @brief 获取字体
	 * @param name 字体名
	 * @param size 字体大小
	 * @param style 0~正常 1~斜体 2~粗体 3~斜体加粗体
	 */
	virtual void * getFont(const string& name, const uint16_t& size, const uint16_t& style=0);
	/**@LEVEL2
	 * @brief 获取LVGL自带的默认字体
	 * @param size 暂时无效。保留。
	 */
	virtual void * getFont(int size);
};
typedef IFontManager* IFntMgr;

/**
 * @brief 运行时接口
 */
interface IRuntime{
public:
	/**
	 * @HideCA
	 */
    virtual ~IRuntime();
    /**
     * @brief 发消息
     * @param type	消息类型
     * @param content	消息内容
     */
    virtual void doSndMsg(std::string type, std::string content);
    /**
     * @brief 取默认字体
     * @return 返回字体指针
     */
	virtual void *getAppDefaultFnt();
	/**
	 * @brief 设置默认字体
	 * @param f	字体指针
	 */
	virtual void setAppDefaultFnt(void * f);
	/**
	 * @brief 设置页面管理器
	 * @param pm 页面管理器指针
	 */
	virtual void setIPageMgr(IPageMgr pm);
	/**
	 * @brief 取页面管理器指针
	 */
	virtual IPageMgr getIPageMgr();
	/**
	 * @brief 设置字体管理器
	 * @param fm 字体管理器指针
	 */
	virtual void setIFntMgr(IFntMgr fm);
	/**
	 * @brief 获取字体管理器指针
	 */
	virtual IFntMgr getIFntMgr();
	/**
	 * @brief 设置设备管理器
	 * @param dm 设备管理器指针
	 */
	virtual void setIDvcMgr(IDvcMgr dm);
	/**
	 * @brief 获取设备管理器指针
	 */
	virtual IDvcMgr getIDvcMgr();
	/**
	 * @brief 注册设备
	 * @param name	指定设备名称
	 * @param settings	JSON格式的设备参数
	 */
	virtual Idevice regDevice(std::string name, std::string settings);
	/**
	 * @brief 设置变量管理器
	 * @param vm	指定设备管理器指针
	 */
	virtual void setIVarMgr(IVarMgr vm);
	/**
	 * @brief 获取变量管理器
	 */
	virtual IVarMgr getIVarMgr();
	/**
	 * @brief 设置多语言管理器
	 * @param lm	指定多语言管理器
	 */
	virtual void setILanguageMgr(ILanguageMgr lm);
	/**
	 * @brief 获取多语言管理器
	 */
	virtual ILanguageMgr getILanguageMgr();
	/**
	 * @brief 获取当前语言
	 */
	virtual string getLanguage() = 0;
	/**
	 * @brief 设置当前语言
	 * @param lan	当前语言
	 */
	virtual bool setLanguage(string lan);
	/**
	 * @brief 设置或添加参数
	 * @param kvs	参数(json)
	 */
	virtual void setParameter(const string& kvs);
	/**
	 * @brief 获取参数值
	 * @param k	参数(string)
	 */
	virtual string getParameter(const string& k);
	/**
	 * @brief 添加或移除App
	 * @param id	线程ID
	 * @param app	APP对象	(nullptr时表示移除)
	 */
	virtual void setAppInstance(const std::thread::id& id, HmiAppBase* app);
	/**
	 * @brief 获取App
	 * @param id	线程ID
	 * @return		APP对象
	 */
	virtual HmiAppBase* getAppInstance(const std::thread::id& id);
};

/**
 * @brief 变量操作
 */
interface IVarOperate{
public:
	/**
	 * @HideCA
	 */
	virtual ~IVarOperate(){}
	/**
	 * @brief 发送事件
	 * @param e	事件代码，见[常量Event]
	 * @param ev	见LvEvent*
	 */
	virtual void sendEvent(uint8_t e, void * ev) = 0;
	/**
	 * @brief 接收时间心跳
	 * @param tick 单位ms
	 */
	virtual void sendTick(uint64_t tick) = 0;
	/**
	 * @brief 业务类型
	 * @return
	 */
	virtual uint8_t type() = 0;
	/**
	 * @brief 事件类型
	 * @return
	 */
	virtual uint8_t event() = 0;
};

/**
 * @brief 外设接口
 */
interface IMISCDevice{
public:
	//设备状态：-1:未初始化或设置错误  0:未启动 1:启动 2:暂停 3:通讯失败 10:写成功 11：收到数据
	enum STATUS{
		UNINITIAL 	= -1,
		READY 		=  0,
		RUN 		=  1,
		PAUSE		=  2,
		COMM_FAIL	=  3,
		WR_SUCCESS	= 10,
		RD_SUCCESS	= 11,
	};
public:
	/**
	 * @HideCA
	 */
	virtual ~IMISCDevice(){};
	/**
	 * @LEVEL0
	 * @brief 参数设置
	 * @param settings
	 * @return
	 */
	virtual bool set(const string& settings)=0;
	/**
	 * @LEVEL0
	 * @brief 启动
	 * @return	是否成功
	 */
	virtual bool open()=0;
	/**
	 * @LEVEL0
	 * @brief 	关闭
	 * @return	是否成功
	 */
	virtual bool close()=0;
	/**
	 * @LEVEL0
	 * @brief 暂停
	 * @return	是否成功
	 */
	virtual bool pause()=0;
	/**
	 * @LEVEL0
	 * @brief 继续
	 * @return	是否成功
	 */
	virtual bool resume()=0;
	/**
	 * @LEVEL0
	 * @brief 获取状态
	 * @return <0: 设置错误  0:未启动 1:启动 2:暂停
	 */
	virtual int status()=0;
	/**
	 * @LEVEL0
	 * @brief 向外围设备写数据
	 * @return	>0:写数据长度  <=0:设备状态不对或错误
	 */
	virtual int wr(const vector<uint8_t>& pData)=0;
	/**
	 * @LEVEL0
	 * @brief 从外围设备读数据
	 * @param pData		存放读出的数据
	 * @return	>0:读出的数据长度  <0:设备状态不对 0:未读到数据
	 */
	virtual int rd(vector<uint8_t>& pData)=0;
};
typedef IMISCDevice* IMiscDevice;

/**
 * @brief 错误异常封装
 */
class Error : public std::exception
{
public:
	explicit Error(int error_code);
	Error(const Error& src) = default;
	virtual ~Error() throw();
	virtual const char* what() const throw() override;
	int code() const throw();

private:
	int m_errno;
	string m_errmsg;
};

/**
 * @brief APP接口定义
 */
interface IApplication{
public:
    virtual ~IApplication(){}
    virtual void init() = 0;
    virtual void suspend() = 0;
    virtual void resume() = 0;
    virtual void quit() = 0;
    virtual IRuntime* getRuntime() = 0;
    virtual IPagePtr buildPage(string id, uint8_t mode = PageMode::NEWWIN, STTWidget* target=nullptr) = 0;
};

/**
 * APP基类
 */
class HmiAppBase : public SttObject, implements IApplication {
public:
	HmiAppBase(IRuntime * rt);
	virtual ~HmiAppBase();

	void init();
	void suspend();
	void resume();
	void quit();
	IRuntime* getRuntime();

public:
	/**
	 * 程序初始化时调用，供派生类继承
	 */
	virtual void onInit();
	/**
	 * 构造应用程序，派生类继承
	 */
	virtual void construct();
	/**
	 * 构成应用程序完成后调用，派生类继承
	 */
	virtual void onReady();
	/**
	 * 暂停应用时调用
	 */
	virtual void onSuspend();
	/**
	 * 继续应用时调用
	 */
	virtual void onResume();
	/**
	 * 退出应用时调用
	 */
	virtual void onQuit();

public:
	/**
	 * 钩子函数
	 */
	virtual void fucHook(IAsynWorker* asynWorker, function<void()> f);

protected:
	/**
	 * 读取配置(保存在S_CFG表中)
	 */
	void loadValue();
	/**
	 * APP运行
	 * @return
	 */
	bool onThreadRun();
    /**
     * @HideCA
     * @brief 绑定的变量变化时，响应函数
     * @param org	控件
     * @param data	变量
     */
    virtual void onDataChanged(SttObject * org, IVariable* data);
    /**
     * @HideCA
     * @brief 打开期限设置页面
     * @param data	变量
     */
    virtual void Lockpage(SttObject * org, IVariable* data);
    /**
     * 注册公共窗体
     * @param page		窗体对象
     */
    void regCmdWindow(STTPage*& wnd, STTPage* page);
    /**
     * 注册键盘窗体
     * @param page		页面Id
     */
    void regNumPadPage(const string& page);
    /**
     * 启用分期授权
     * @param num		分期数
     */
    void enableStagesAuth(int num);
protected:
	IPageMgr pm;
	IDvcMgr dm;
	IVarMgr vm;
	IFntMgr fm;
	IRuntime *rt;
	bool m_running;
	map<IAsynWorker*, function<void()>> mHook;
};


/**
 * 异步（后台）工作接口
 */
interface IAsynWorker{
public:
	virtual ~IAsynWorker();
	/**
	 * @brief 注入后台长任务处理（禁止UI操作!!）
	 * @param f
	 * @return
	 */
	virtual IAsynWorker* invoke(function<void()> f);
	/**
	 * @brief 注入任务完成后续处理（可UI操作）
	 * @param f
	 * @return
	 */
	virtual IAsynWorker* onFinished(function<void()> f);
	/**
	 * @brief 启动任务
	 * @return
	 */
	virtual bool start();
	/**
	 * @brief 获取标识
	 * @return
	 */
	virtual int flag();
};

/**
 * @brief 异步（后台）工作类
 */
class AsynWorker : public SttObject, implements IAsynWorker{
public:
	AsynWorker(HmiAppBase* app);
	virtual ~AsynWorker();
	/**
	 * @brief 注入后台长任务处理（禁止UI操作!!）
	 * @param f
	 * @return
	 */
	virtual IAsynWorker* invoke(function<void()> f);
	/**
	 * @brief 注入任务完成后续处理（可UI操作）
	 * @param f
	 * @return
	 */
	virtual IAsynWorker* onFinished(function<void()> f);
	/**
	 * @brief 启动任务
	 * @return
	 */
	virtual bool start();
	/**
	 * @brief 获取标识
	 * @return
	 */
	virtual int flag();
public:
	void * mWorker;
};

/**
 * 获取标签本地语言
 * @param source
 * @return
 */
string localeLaungauge(const string& source);

/**
 * @brief 获取运行环境
 * @return	运行环境对象
 */
IRuntime* getRuntime();

}

