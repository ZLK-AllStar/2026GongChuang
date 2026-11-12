/*
 * STTPage.h
 *
 *  Created on: 2023年2月16日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"
#include "base/UARTBase.h"
#include "misc/Printer.h"
#include "widgets/STTFFmpeg.h"

using namespace stt::misc;

namespace stt{

/**
 * @ingroup widget
 * @brief 面板
 */
class STTPanel : public STTWidget{
public:
	/**
	 * @HideCA
	 * @brief 面板控件构造函数
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
	STTPanel(IRuntime* rt, STTWidget* par, const string& id = "");
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTPanel();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h 	位置、大小
     * @param type		控件类型 0:面板  1:图片  2:页面
     * @note 面板和图片默认关闭滚动条，页面则当有控件超出边界会自动激活
     */
    void setMainArg(int x, int y, int w, int h, int type=2);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色(渐变时为起始色)
     * @param c2		背景渐变时的终止色
     * @param dir		渐变类型 0~不渐变 1~上下 2~左右
     * @param opa		不透明度0~255
     * @param w_border	边框线粗
     * @param c_border	边框线色
     * @param img		背景图
     */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=0, int w_border=0, int c_border=0xdddddd, const string& img="");
    /**
     * @LEVEL1
     * @brief 设置背景图片。可设置GIF(仅STTPage控件可设置GIF)
     */
    STTWidget* setBgImg(const string& path, int part = Part::Main);
    /**
     * @HideCA
     * @brief 设置尺寸(由于此控件可能有图片，需要在resize前，将图片清空。resize后，再将图片设置回来)
     */
    STTWidget* setSize(int w, int h);
private:
    /**@HideCA
     * @brief 内部指针
     */
    void* mMux;
};

typedef STTPanel* SttPanel;

class STTTileView;
/**
 * @ingroup widget
 * @brief 贴磁
 */
class STTTile : public STTWidget{
public:
	/**@HideCA*/
	STTTile(IRuntime* rt, STTTileView* par, uint8_t col, uint8_t row, uint8_t dir);
	/**@HideCA*/
	virtual ~STTTile();
	/**@HideCA*/
	int8_t col;
	/**@HideCA*/
	int8_t row;
};

/**
 * @ingroup widget
 * @brief 贴磁视图
 */
class STTTileView : public STTWidget{
public:
	/**
	 * @HideCA
	 * @biref 默认水平，3个。 cols*rows>0且<12
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 * @param cols	列数<10
	 * @param rows	行数<10
	 */
	STTTileView(IRuntime* rt, STTWidget* par, const string& id="", int cols=3, int rows=1);
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTTileView();
    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h 	位置、大小
     */
    void setMainArg(int x, int y, int w, int h);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色(渐变时为起始色)
     * @param c2		背景渐变时的终止色
     * @param dir		渐变类型 0~不渐变 1~上下 2~左右
     * @param opa		不透明度0~255
     * @param w_border	边框线粗
     * @param c_border	边框线色
     */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=0, int w_border=0, int c_border=0xdddddd);
    /**
     * @LEVEL2
     * @brief 获取tile控件
     * @return tile控件
     */
    STTTile* getTile(int8_t index);
    /**
     * @LEVEL2
     * @brief 设置当前tile序号
     * @param index	序号
     * @param an	启用动画
     */
    void setTileIndex(int8_t index, bool an);
    /**
     * @LEVEL2
     * @brief 获取当前tile序号
     * @return 当前tile序号
     */
    int8_t getTileIndex();
    /**
     * @LEVEL2
     * @brief 获取当前tile控件
     * @return 当前tile控件
     */
    STTTile* getActTile();
    /**@HideCA*/
    void* mMux;
protected:
    /**@HideCA*/
    void onDataChanged(SttObject * org, IVariable* data);
    /**@HideCA*/
    void build();
};

typedef STTTileView* SttTileView;


/**
 * @ingroup widget
 * @brief 页面
 */
class STTPage : public STTWidget, implements IPage{
public:
	/**
	 * @HideCA
	 * @brief 页面构造函数
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 * @param ext 	扩展字段
	 */
	STTPage(IRuntime* rt, STTWidget* par, const string& id = "", const string& ext = "");
	/**
	 * @HideCA
	 * @brief 页面构造函数
	 * @param rt 	运行时接口指针
	 * @param layer	所在层
	 * @param id 	控件ID
	 * @param ext 	扩展字段
	 */
	STTPage(IRuntime* rt, int layer, const string& id = "", const string& ext = "");
	/**
	 * @HideCA
	 */
	virtual ~STTPage();
    /**
     * @brief 获取编码器控制组，详见lv_group
     */
    vector<STTWidget*>* getEncoderGroup();
    /**
     * @brief 将控件设为本页面的最后焦点控件，用于编码器或外部按键，打开另一个页面再返回时聚焦于此控件
     */
    void setLastFocused(STTWidget* wid);
    /**
     * @brief 获取本页面的最后焦点控件，用于编码器或外部按键，打开另一个页面再返回时聚焦于此控件
     */
    STTWidget* getLastFocused();

    /**
     * @LEVEL2
     * @brief 设置基本参数
     * @param x,y,w,h 	位置、大小
     * @param type		控件类型 0:面板  1:图片  2:页面
     */
    void setMainArg(int x, int y, int w, int h, int type=2);
    /**
     * @LEVEL2
     * @brief 设置基本样式
     * @param r			圆角半径
     * @param c1		背景主色(渐变时为起始色)
     * @param c2		背景渐变时的终止色
     * @param dir		渐变类型 0~不渐变 1~上下 2~左右
     * @param opa		不透明度0~255
     * @param w_border	边框线粗
     * @param c_border	边框线色
     * @param img		背景图
     */
    void setMainStyle(int r=0, int c1=0xffffff, int c2=0xffffff, int dir=0, int opa=0, int w_border=0, int c_border=0xdddddd, const string& img="");
    /**
     * @LEVEL1
     * @brief 设置背景图片。可设置GIF(仅STTPage控件可设置GIF)
     */
    STTWidget* setBgImg(const string& path, int part = stt::constant::Part::Main);
    /**
	 * @LEVEL0
	 * @brief 关闭当前页面
	 */
	void close();
    /**
     * @HideCA
     * @brief 刷新显示
     */
	void display();
	/**
	 * @brief 带视频的截图
	 * @param path	保存文件的路径
	 * @param isBGR 是RGB还是BGR
	 * @param v1,v2,v3,v4	视频控件1
	 * @param vecWidget 放在视频中的控件
	 */
	void snapshotToPng(const string& path, bool isBGR=false, STTFFmpeg* v1=NULL, STTFFmpeg* v2=NULL, STTFFmpeg* v3=NULL, STTFFmpeg* v4=NULL, vector<STTWidget*>* vecWidget=NULL);
	/**
	 * @brief 变量的页面驻留
	 */
	void addResideVar(const string& varname);
	/**
	 * @brief 增加变量消息
	 */
	void addVarMsg(std::function<void()>& cb);
public:
    /**@LEVEL1
     * @brief 页面管理器
     * */
	IPageMgr pm;
	/**@LEVEL1
	 * @brief 设备管理器
	 * */
	IDvcMgr dm;
	/**@LEVEL1
	 * @brief 变量管理器
	 * */
	IVarMgr vm;
	/**@LEVEL1
	 * @brief 字体管理器
	 * */
	IFntMgr fm;
	/**@LEVEL1
	 * @brief 运行环境
	 * */
	IRuntime* p_rt;
private:
    /**@HideCA*/
    void* mMux;
    /**@HideCA*/
    void refrChild(SttWidget wgt);
};
typedef STTPage* SttPage;

/**
 * @ingroup widget
 * @brief 定时任务
 */
class STTTimer : public SttObject, implements IVarOperate{
public:
	/**
	 * @HideCA
	 * @brief 构造
	 */
	STTTimer(IRuntime* rt, STTWidget *par, const string& id, bool runInBackground=false);
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTTimer();
	/**
	 * @HideCA
	 * @brief 无用
	 */
	void sendEvent(uint8_t e, void * ev);
	/**
	 * @LEVEL2
	 * @brief 发送时间戳，执行符合条件的定时任务
	 */
	void sendTick(uint64_t tick);
	/**
	 * @HideCA
	 * @brief 类型返回102，用于判断当前IVarOperate是否定时器任务
	 */
	uint8_t type();
	/**
	 * @HideCA
	 * @brief 无用
	 */
	uint8_t event();

    /**
     * @LEVEL1
     * @brief 设置定时任务
     * @param period			执行周期(ms) 取值范围大于100
     * @param count				重复次数(小于1时，不限次数)
     * @param cron				cron表达式
     * @return
     */
	STTTimer* set(int period, int count=0, const string& cron="");
    /**
     * @LEVEL0
     * @brief 启动定时任务
     * @return
     */
    bool start();
    /**
     * @LEVEL1
     * @brief 暂停定时任务
     * @return
     */
    bool pause();
    /**
     * @LEVEL0
     * @brief 继续定时任务
     * @return
     */
    bool resume();
    /**
     * @LEVEL0
     * @brief 结束定时任务
     * @return
     */
    bool stop();
    /**
     * @LEVEL0
     * @brief 获取启停状态
     * @return
     */
    bool status();
    /**
     * @LEVEL1
     * @brief 是否后台进程
     * @return
     */
    bool isBackground();
    /**
     * @LEVEL1
     * @brief 定时任务
     * @param code
     * @param object
     * @param handler
     */
    void onEventHandler(uint16_t code, SttObject* object, EHandler handler);
    /**
     * @LEVEL0
     * @brief 获取当前周期(ms)
     */
    uint32_t getPeriod();

private:
	/**@HideCA*/
    int calc();
	/**@HideCA*/
    void onTriHandler(SttObject* obj, IVariable* v);
    /**@HideCA*/
    void* mMux;
};
typedef STTTimer* SttTimer;

/**
 * @ingroup widget
 * @brief 串口
 */
class STTCom : public SttObject{
public:
	/**
	 * @HideCA
	 */
	STTCom(IRuntime* rt, SttObject *par, const string& id, bool runInBackground=false);
	/**
	 * @HideCA
	 */
	virtual ~STTCom();
    /**
     * @LEVEL1
     * @brief 设置参数
     * @param settings	串口控件参数
     * @return 是否成功
     */
	bool set(const string& settings="");
	/**
	 * @LEVEL1
	 * @biref 打开串口控件
	 * @return
	 */
	bool open();
	/**
	 * @LEVEL1
	 * @biref 暂停
	 * @return
	 */
	bool pause();
	/**
	 * @LEVEL1
	 * @biref 继续
	 * @return
	 */
	bool resume();
	/**
	 * @LEVEL1
	 * @biref 清除缓冲区
	 * @param t	0:读缓冲区  1:写缓冲区
	 * @return
	 */
	bool clean(int t=0);
	/**
	 * @LEVEL1
	 * @biref 关闭串口控件
	 * @return
	 */
	bool close();
	/**
	 * @LEVEL1
	 * @biref 向串口写数据
	 * @param pData		写入数据
	 * @return 是否成功
	 */
	bool writeData(const vector<uint8_t>& cmd);
	/**
	 * @LEVEL1
	 * @biref 读取串口数据（阻塞式）
	 * @param pData		存放读出的数据
	 * @param waittime(ms)	等待时间
	 * @return 是否成功
	 */
	bool readData(vector<uint8_t>& pData, int waittime);
    /**
     * @LEVEL1
     * @brief 收到数据处理
     * @param code
     * @param object
     * @param handler
     */
    void onEventHandler(uint16_t code, SttObject* object, EHandler handler);

private:
	/**@HideCA*/
    void onDataReceiveHandler(SttObject* obj, IVariable* v);
    /**@HideCA*/
    void* mMux;
};
typedef STTCom* SttCom;

/**
 * 条码打印机控件
 */
class STTBarcodePrinter : public SttObject{
public:
    enum PrinterType {
        LABEL_PRINTER,
        BARCODE_PRINTER
    };

	/**
	 * @HideCA
	 */
	STTBarcodePrinter(IRuntime* rt, STTWidget *par, const string& id);
	/**
	 * @HideCA
	 */
	virtual ~STTBarcodePrinter();
    /**
     * @LEVEL1
     * @brief 设置参数
     * @param settings	串口控件参数
     * @return 是否成功
     */
	bool set(const string& settings="");
	/**
	 * @LEVEL1
	 * 打开控件
	 * @return
	 */
	bool open();
	/**
	 * @LEVEL1
	 * 暂停
	 * @return
	 */
	bool pause();
	/**
	 * @LEVEL1
	 * 继续
	 * @return
	 */
	bool resume();
	/**
	 * @LEVEL1
	 * @biref关闭控件
	 * @return
	 */
	bool close();
	/**
	 * @LEVEL1
	 * @biref 获取标签打印机对象
	 * @return
	 */
	ILabelPrinter* getLabelPrinter();
	/**
	 * @LEVEL1
	 * @biref 获取标签打印机对象
	 * @return
	 */
	IBarcodePrinter* getBarcodePrinter();
    /**
     * @LEVEL1
     * @brief 收到数据处理
     * @param code
     * @param object
     * @param handler
     */
    void onEventHandler(uint16_t code, SttObject* object, EHandler handler);

private:
	/**@HideCA*/
    void onDataReceiveHandler(SttObject* obj, IVariable* v);
    /**@HideCA*/
    PrinterType mType;
    /**@HideCA*/
    IBarcodePrinter* mBarcodePrinter;
    /**@HideCA*/
    ILabelPrinter* mLabelPrinter;
};
typedef STTBarcodePrinter* SttBarcodePrinter;


/**
 * @ingroup widget
 * @brief 带进度条异步任务
 */
class STTProgressWorker : public SttObject, implements IAsynWorker{
public:
	/**@HideCA*/
	STTProgressWorker(HmiAppBase* app, STTWidget *par, const string& id, int timeout=5);
	/**@HideCA*/
	virtual ~STTProgressWorker();
	virtual IAsynWorker* invoke(function<void()> f);
	virtual IAsynWorker* onFinished(function<void()> f);
	virtual bool start();
	/**
	 * 更新显示进度
	 * @param rate	1-100 0：则不显示
	 * @param msg	为空则不显示
	 */
	virtual void updateProgress(int rate=-1, const string& msg="");
public:
	void * mWorker;
    /**
     * @HideCA
     * @brief 绑定的变量变化时，响应函数
     * @param org	控件
     * @param data	变量
     */
    virtual void onDataChanged(SttObject * org, IVariable* data);
};
typedef STTProgressWorker* SttProgressWorker;

/**
 * @ingroup widget
 * @brief 键盘
 */
class STTKeyboard : public SttObject{
public:
	/**
	 * @HideCA
	 */
	STTKeyboard(IRuntime* rt, STTWidget *par, const string& id,uint8_t m_type=0);
	/**
	 * @HideCA
	 */
	virtual ~STTKeyboard();
	/**
	 * @LEVEL1
	 * 暂停
	 * @return
	 */
	bool pause();
	/**
	 * @LEVEL1
	 * 继续
	 * @return
	 */
	bool resume();
    /**
     * @LEVEL1
     * @brief 收到数据处理
     * @param code
     * @param object
     * @param handler
     */
    void onEventHandler(uint16_t code, SttObject* object, EHandler handler);

protected:
	/**@HideCA*/
    void onDataReceiveHandler(SttObject* obj, IVariable* v);
    /**@HideCA*/
    void* mMux;
};
typedef STTKeyboard* SttKeyboard;

/**
 * @ingroup widget
 * @brief 扫码枪
 */
class STTBarcodeScanner : public STTKeyboard{
public:
	/**
	 * @HideCA
	 */
	STTBarcodeScanner(IRuntime* rt, STTWidget *par, const string& id);
	/**
	 * @HideCA
	 */
	virtual ~STTBarcodeScanner();
};
typedef STTBarcodeScanner* SttBarcodeScanner;

/**
 * @ingroup widget
 * @brief HTTP控件
 */
class STTHttp : public SttObject{
public:
 	/** 响应编码 */
 	long mResponseCode;
 	/** 响应内容 */
 	string mResponse;
 	/** 执行返回代码 */
 	int mRetCode;
public:
	/**
	 * @HideCA
	 */
	STTHttp(IRuntime* rt, SttObject *par, const string& id);
	/**
	 * @HideCA
	 */
	virtual ~STTHttp();
	/**
	 * @LEVEL1
     * @brief 设置请求头
	 * @param header	参数字符串
	 * @return 是否成功
	 */
	void setHeader(const string& header);
	/**
	 * @LEVEL1
     * @brief 删除请求头
	 */
	void clearHeader();
    /**
     * @LEVEL1
     * @brief 设置选项参数
     * @param op	选项
     * @param para	参数
     * @return 是否成功
     */
	int setOption(int op, string para);
	int setOption(int op, int64_t para);
	int setOption(int op, void* para);
	/**
	 * @LEVEL1
	 * @biref 		GET方法
	 * @param url	请求地址
	 * @return		成功：返回sid(>10000) 失败：其它值
	 */
	int get(const string& url);
	/**
	 * @LEVEL1
	 * @biref 		POST方法
	 * @param url	请求地址
	 * @param data	数据
	 * @return		成功：返回sid(>10000) 失败：其它值
	 */
	int post(const string& url, const string& data);
	/**
	 * @LEVEL1
	 * @biref 		上传
	 * @param url	请求地址
	 * @param file	上传文件路径
	 * @return		成功：返回sid(>10000) 失败：其它值
	 */
	int upload(const string& url, const string& file);
	/**
	 * @LEVEL1
	 * @biref 		上传
	 * @param url	请求地址
	 * @param path	下载路径
	 * @return		成功：返回sid(>10000) 失败：其它值
	 */
	int download(const string& url, const string& path);
    /**
     * @LEVEL1
     * @brief 收到数据处理
     * @param object
     * @param handler
     */
    void onEventHandler(SttObject* object, EHandler handler);
protected:
    /**@HideCA*/
    void* mMux;
    /**@HideCA*/
    void onTriHandler(SttObject* obj, uint16_t code);
    /**@HideCA*/
    void doPerform();
};
typedef STTHttp* SttHttp;


/**
 * @ingroup widget
 * @brief 异步工作控件
 */
class STTAsyncWorker : public SttObject{
public:
	/**
	 * @HideCA
	 * @brief 构造
	 */
	STTAsyncWorker(IRuntime* rt, STTWidget *par, const string& id);
	/**
	 * @HideCA
	 * @brief 析构
	 */
	virtual ~STTAsyncWorker();
	/**
	 * @LEVEL0
     * @brief 设置间隔时间
	 * @param ms	间隔时间ms
	 */
	void setPeriod(int ms);
    /**
     * @LEVEL0
     * @brief 启动异步任务
     * @return
     */
    bool start();
    /**
     * @LEVEL1
     * @brief 暂停异步任务
     * @return
     */
    bool pause();
    /**
     * @LEVEL0
     * @brief 继续异步任务
     * @return
     */
    bool resume();
    /**
     * @LEVEL0
     * @brief 结束异步任务
     * @return
     */
    bool stop();
    /**
     * @LEVEL0
     * @brief 获取异步任务状态
     * @return
     */
    int status();
    /**
     * @LEVEL1
     * @brief 定义异步任务内容
     * @param code
     * @param object
     * @param handler
     */
    void onActionHandler(SttObject* object, EHandler handler);
    /**
     * @LEVEL1
     * @brief 异步任务结束时触发
     * @param code
     * @param object
     * @param handler
     */
    void onFinishHandler(SttObject* object, EHandler handler);
protected:
	/**@HideCA*/
    void onAct(SttObject* obj, uint16_t code);
	/**@HideCA*/
    void onFinish(SttObject* obj, uint16_t code);
    /**@HideCA*/
    void* mMux;
};
typedef STTTimer* SttTimer;

class STTFx : public SttObject {
public:
	function<void()> mFuc;
    string m_varname;

    void onwDataChanged(SttObject *org, IVariable *data) {
    	/*变量消息*/
    	mFuc();
    }
    STTFx(IRuntime* rt, STTWidget* par, string varname, function<void()> func): SttObject(par) {
    	mFuc = func;
        m_varname = varname;
        getRuntime()->getIVarMgr()->get(m_varname)->valueChanged.connect(this,&STTFx::onwDataChanged);
    }

    ~STTFx() {
        getRuntime()->getIVarMgr()->get(m_varname)->valueChanged.disconnect(this,&STTFx::onwDataChanged);
    }

};
typedef STTFx* SttFx;

}
