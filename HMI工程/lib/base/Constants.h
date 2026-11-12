/*
 * 类库常量
 *
 *  Created on: 2023年7月27日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>

using namespace std;

/**
 * @ingroup constant
 * @namesapce stt::constant
 * @brief 常量(如系统变量，控件类型、数据类型、对齐方式等)
 */
namespace stt::constant {

/**
 * @brief 系统变量
 */
class SysVar{
public:
	/**当前时间*/
	static const string Now;
	/**当前小时（24H）*/
	static const string Hour;
	/**当前小时（12H）*/
	static const string Hour12;
	/**当前分钟*/
	static const string Minute;
	/**当前秒*/
	static const string Second;
	/**登录秒数*/
	static const string LoginTime;
	/**累计工作秒数*/
	static const string WorkTime;
	/**上电时间*/
	static const string PoweronTime;
	/**当前用户ID*/
	static const string UserID;
	/**当前用户名*/
	static const string UserName;
	/**当前用户组*/
	static const string UserGrp;
	/**当前语言*/
	static const string Language;
	/**总线消息（收）*/
	static const string RcvMsg;
	/**总线消息（发）*/
	static const string SndMsg;
	/**NULL变量*/
	static const string NullVar;
	/**进度条百分值*/
	static const string PrgressRat;
	/**进度条提示消息*/
	static const string PrgressMsg;
	/**版本时间戳*/
	static const string VerStamp;
	/**内部预留整型0*/
	static const string ResvInt0;
	/**内部预留整型1*/
	static const string ResvInt1;
	/**内部预留整型2*/
	static const string ResvInt2;
	/**内部预留整型3*/
	static const string ResvInt3;
	/**WIFI状态*/
	static const string WifiStatus;
	/**当前WIFI*/
	static const string WifiSsid;
	/**4G/5G状态*/
	static const string FGStatus;
	/**当前4G/5G*/
	static const string FGSsid;
	/**网口_0 IP*/
	static const string ETH0_IP;
	/**网口_1 IP*/
	static const string ETH1_IP;
	/**WIFI IP*/
	static const string WLAN_IP;
	/**移动网络IP*/
	static const string FG_IP;
	/**最后触屏时间*/
	static const string LAST_TOUCH;
	/**期限状态*/
	static const string LOCKStatus;
	/**当前输入值*/
	static const string CurrentInput;
	/**输入键盘上限*/
	static const string InputUL;
	/**输入键盘下限*/
	static const string InputLL;
	/**OTA状态*/
	static const string OTAStatus;
	/**OTA提示信息*/
	static const string OTAMsg;
	/**屏序列号*/
	static const string SN;
	/**当前报警数*/
	static const string WarningQty;
	/**云连接状态*/
	static const string CLOUDSTATUS;
	/**云连接二维码*/
	static const string CLOUDBARCODE;
};
/**
 * @brief 文本标签格式
 */
class LabelFormat{
public:
	enum{
		/**常规文本*/
	    TEXT 		= 	0,
		/**数值*/
		NUMERICAL 	= 	1,
		/**货币*/
		CURRENCY 	= 	2,
		/**日期*/
		DATETIME	=	3,
		/**百分比*/
		PERCENT		=	4,
		/**科学计数*/
		SECIENCE	=	5,
	};
};
/**
 * @brief 控件类型
 */
class WidgetType{
public:
	enum{
		/**弧形进度条*/
		Arc 	= 1,
		/**进度条*/
		Bar		= 2,
		/**按钮*/
		Button  = 3,
		/**扩展按钮(保留)*/
		ButtonEx= 4,
		/**画布*/
		Canvas	= 5,
		/**图表*/
		Chart	= 6,
		/**下拉框*/
		Dropdown= 7,
		/**图片*/
		Image	= 8,
		/**标签*/
		Label	= 9,
		/**LED灯*/
		Led		= 10,
		/**列表*/
		List	= 11,
		/**码表*/
		Meter	= 12,
		/**页面*/
		Page	= 13,
		/**二维码*/
		QRCode	= 14,
		/**滚轮*/
		Roller	= 15,
		/**滑动器*/
		Slider	= 16,
		/**圆形进度*/
		Spinner	= 17,
		/**开关*/
		Switch	= 18,
		/**基础表格*/
		Table	= 19,
		/**扩展表格*/
		TableEx = 20,
		/**输入框*/
		TextArea= 21,
		/**视频*/
		Video	= 22,
		/**画线*/
		Line	= 23,
		/**面板*/
		Panel	= 24,
		/**贴磁*/
		Tile	= 25,
		/**贴磁视图*/
		TileView= 26,
	};

};
/**
 * @brief LV部件
 */
class Part{
public:
	enum{
		/**主体*/
		Main         = 0x000000,
		/**滚动条*/
		ScrollBar    = 0x010000,
		/**指示条或块*/
		Indic    	 = 0x020000,
		/**滑块或下拉钮*/
		Knob         = 0x030000,
		/**选中项*/
		Selected     = 0x040000,
		/**子项*/
		Items        = 0x050000,
		/**刻度*/
		Ticks        = 0x060000,
		/**光标*/
		Cursor       = 0x070000,
		/**自定义*/
		Custom_First = 0x080000,
		/**以上的集合，特定使用*/
		Any          = 0x0F0000,
	};
};
/**
 * @brief 语言
 */
class Language{
public:
	/**简体中文*/
	static const string zh_CN;
	/**繁体中文*/
	static const string zh_TW;
	/**Русский язык*/
	static const string ru_RU;
	/**English*/
	static const string en_US;
	/**اللغة العربية*/
	static const string ar_AR;
	/**Français*/
	static const string fr_FR;
	/**Deutsch*/
	static const string de_DE;
	/**한국어*/
	static const string ko_KR;
	/**Español*/
	static const string es_ES;
	/**にほんご*/
	static const string ja_JP;
	/**Tiếng Kinh*/
	static const string vi_VN;
};
/**
 * @brief 控件状态
 */
class WidgetState{
public:
	enum {
		/**默认松开*/
	    DEFAULT     =  0x0000,
		/**Checked*/
	    CHECKED     =  0x0001,
		/**获得焦点*/
	    FOCUSED     =  0x0002,
		/**通过键盘获得焦点*/
	    FOCUS_KEY   =  0x0004,
		/**由编码器编辑*/
	    EDITED      =  0x0008,
		/**鼠标悬停*/
	    HOVERED     =  0x0010,
		/**按下*/
	    PRESSED     =  0x0020,
		/**正在滚动*/
	    SCROLLED    =  0x0040,
		/**禁用*/
	    DISABLED    =  0x0080,
		/**自定义*/
	    USER_1      =  0x1000,
		/**自定义*/
	    USER_2      =  0x2000,
		/**自定义*/
	    USER_3      =  0x4000,
		/**自定义*/
	    USER_4      =  0x8000,
		/**任意状态，特殊用途*/
	    ANY = 0xFFFF,
	};
};
/**
 * @brief LV事件
 * （对应LVGL事件，详细说明请参考官网Ver8.3）
 */
class Event{
public:
	enum{
	    ALL = 0,

	    /**以下为输入设备事件*/
		/**被按下*/
	    PRESSED,
		/**正在被按下 (按下时，连续发出本事件)*/
	    PRESSING,
		/**仍在被按下，但光标/手指滑出了对象*/
	    PRESS_LOST,
		/**按下后在短时间(<400ms)内松开。滚动时不调用*/
	    SHORT_CLICKED,
		/**被长时间(>400ms)按下。滚动时不调用。*/
	    LONG_PRESSED,
		/**被长时间(>400ms)按下，每100ms发送一次。滚动时不调用。*/
	    LONG_PRESSED_REPEAT,
		/**被点击。滚动时不调用，不管长按。*/
	    CLICKED,
		/**在松开对象时调用*/
	    RELEASED,
		/**滚动开始, 事件参数是指向滚动动画的指针, 可以修改*/
	    SCROLL_BEGIN,
		/**滚动结束*/
	    SCROLL_END,
		/**滚动中*/
	    SCROLL,
		/**触摸屏手势*/
	    GESTURE,
		/**键盘按下键值*/
	    KEY,
		/**获得焦点*/
	    FOCUSED,
		/**失去焦点*/
	    DEFOCUSED,
		/**失去焦点但仍然被选中*/
	    LEAVE,
		/**执行高级碰撞测试*/
	    HIT_TEST,

	    /**以下为绘制控件事件*/
	    COVER_CHECK,
	    REFR_EXT_DRAW_SIZE,
	    DRAW_MAIN_BEGIN,
	    DRAW_MAIN,
	    DRAW_MAIN_END,
	    DRAW_POST_BEGIN,
	    DRAW_POST,
	    DRAW_POST_END,
	    DRAW_PART_BEGIN,
	    DRAW_PART_END,

	    /** 以下是特殊事件*/
	    VALUE_CHANGED,       /**< 值变化（比如滑块滑动）*/
	    INSERT,              /**< 插入文本（事件数据是char *）*/
	    REFRESH,             /**< （用户）通知控件刷新*/
	    READY,               /**< 处理完成*/
	    CANCEL,              /**< 取消处理*/

	    /** 其他事件*/
	    DELETE,              /** 控件正在被删除*/
	    CHILD_CHANGED,       /**< 子控件（增、删、改）变化*/
	    CHILD_CREATED,       /**< 子控件（增）变化*/
	    CHILD_DELETED,       /**< 子控件（删）变化*/
	    SCREEN_UNLOAD_START, /**< 屏幕卸载加载*/
	    SCREEN_LOAD_START,   /**< 屏幕开始加载*/
	    SCREEN_LOADED,       /**< 屏幕加载完成*/
	    SCREEN_UNLOADED,     /**< 屏幕卸载完成*/
	    SIZE_CHANGED,        /**< 控件位置、尺寸变化*/
	    STYLE_CHANGED,       /**< 控件风格样式变化*/
	    LAYOUT_CHANGED,      /**< 布局变化造成子控件位置改变*/
	    GET_SELF_SIZE,       /**< 获取控件内尺寸*/

	    _LAST,               /** 上述定义的事件数量*/

	    PREPROCESS = 0x80,
	};
};
/**
 * @brief 控件对齐方式
 */
class ALIGN{
public:
	enum {
		/**不对齐*/
	    DEFAULT = 0,
		/**内侧上左*/
	    TOP_LEFT,
		/**内侧上中*/
	    TOP_MID,
		/**内侧上右*/
	    TOP_RIGHT,
		/**内侧下左*/
	    BOTTOM_LEFT,
		/**内侧*/
	    BOTTOM_MID,
		/**内侧*/
	    BOTTOM_RIGHT,
		/**内侧*/
	    LEFT_MID,
		/**内侧*/
	    RIGHT_MID,
		/**中心*/
	    CENTER,
		/**外侧上左，上边界的上侧*/
	    OUT_TOP_LEFT,
		/**外侧上中，上边界的上侧*/
	    OUT_TOP_MID,
		/**外侧上右，上边界的上侧*/
	    OUT_TOP_RIGHT,
		/**外侧下左，下边界的下侧*/
	    OUT_BOTTOM_LEFT,
		/**外侧下中，下边界的下侧*/
	    OUT_BOTTOM_MID,
		/**外侧下右，下边界的下侧*/
	    OUT_BOTTOM_RIGHT,
		/**外侧左上，左边界的左侧*/
	    OUT_LEFT_TOP,
		/**外侧左中，左边界的左侧*/
	    OUT_LEFT_MID,
		/**外侧左下，左边界的左侧*/
	    OUT_LEFT_BOTTOM,
		/**外侧右上，右边界的右侧*/
	    OUT_RIGHT_TOP,
		/**外侧右中，右边界的右侧*/
	    OUT_RIGHT_MID,
		/**外侧右下，右边界的右侧*/
	    OUT_RIGHT_BOTTOM,
	};
};

/**
 * @brief 文本对齐方式
 */
class TextAlign{
public:
	enum {
		/**自动*/
	    AUTO,
		/**靠左*/
	    LEFT,
		/**居中*/
	    CENTER,
		/**靠右*/
	    RIGHT,
	};
};
/**
 * @brief 变量类型
 */
class DataType{
public:
	enum {
		/**32位带符号整型*/
		T_INT 		= 0,
		/**浮点*/
		T_FLOAT 	= 1,
		/**字符串*/
		T_VCHAR 	= 2,
		/**64位带符号整型*/
		T_LONG 		= 3,
		/**配方对象*/
		T_RCP 		= 4,
		/**表格*/
		T_TABLE		= 5,
		/**表视图*/
		T_TABLEVIEW	= 6,
		/**32位带符号整型*/
		D_INT 		= 10,
		/**32位无符号整型*/
		D_UINT 		= 11,
		/**浮点*/
		D_FLOAT 	= 12,
		/**字符串*/
		D_VCHAR 	= 13,
		/**布尔型/开关型*/
		D_BOOL 		= 14,
		/**64位带符号整型*/
		D_LONG 		= 15,
		/**16位符号整型*/
		D_INT16		= 16,
		/**16位无符号整型*/
		D_UINT16	= 17,
		/**16位BCD*/
		D_BCD16		= 18,
		/**32位BCD*/
		D_BCD32		= 19,
		/**未知*/
		T_NONE 		= 60,
	};
};
/**
 * @brief 设备状态
 */
class MachineSta{
public:
	enum {
		/**离线*/
		MACHINE_OFFLINE	= 0,
		/**在线*/
		MACHINE_ONLINE 	= 1,
		/**未初始化*/
		MACHINE_UNINIT	= 2
	};
};
/**
 * @brief 窗体打开方式
 */
class PageMode{
public:
	enum {
		/**打新窗体并关闭已有窗体*/
		NEWWIN	= 0,
		/**在现有窗体上弹窗*/
		MODAL 	= 1,
		/**嵌入窗体到控件*/
		EMBEDDED= 2,
		/**悬浮*/
		DRIFT 	= 3,
	};
};
/**
 * @brief 消息对话框大小
 */
class MSGBOX_SIZE{
public:
	enum {
		/**小*/
		small,
		/**中*/
		medium,
		/**大*/
		large,
		/**最大*/
		larger,
	};
};
/**
 * @brief 消息对话框字体大小
 */
class MSGBOX_FONT{
public:
	enum {
		/**小*/
		small,
		/**中*/
		medium,
		/**大*/
		large,
	};
};
/**
 * @brief 消息框类型
 */
class MSGBOX_TYPE{
public:
	enum {
		/**提示*/
		TIP,
		/**警告*/
		WARNING,
		/**确认*/
		CONFIRM,
		/**输入*/
		INPUT,
	};
};
/**
 * @brief 消息框类型
 */
class PROGRESS_TYPE{
public:
	enum {
		/**倒计时*/
		COUNT_DOWN,
		/**变量值*/
		VARIABLE,
	};
};
/**
 * @brief 消息框按钮
 */
class MSGBOX_BTN{
public:
	enum {
		/**取消/关闭*/
		CLOSE,
		/**确认*/
		OK,
		/**保留1*/
		EXT1,
		/**保留2*/
		EXT2,
	};
};
/**
 * @brief 键盘模式
 */
class IME{
public:
	enum {
		/**小写字母*/
		LC 		=	0,
		/**大写字母*/
		UC		= 	1,
		/**纯数字*/
		DIGITAL	=	2,
		/**符号及数字*/
		SYMBOL	= 	3,
		/**T9拼音*/
		T9		=	4,
		/**全键盘拼音*/
		K26		=	5,
		/**无*/
		NA		=	6,
		/**用户自定义*/
		CUSTOM	=	7,
		/**用代码改写默认键盘*/
		CODE = 8
	};
};
/**
 * @brief 变量动作
 */
class SIT_DBOBJ{
public:
	enum {
		/**置ON*/
		ON,
		/**置OFF*/
		OFF,
		/**取反*/
		CNV,
		/**按1松0*/
		PON,
		/**按0松1*/
		POF,
		/**周期切换*/
		CSW,
		/**写入常数*/
		DO_WRC,
		/**递加*/
		DO_ADD,
		/**递减*/
		DO_SUB,
		/**按住按钮时周期递加（至上限）*/
		PR_ADD,
		/**按住按钮时周期递减（至下限）*/
		PR_SUB,
		/**周期往复变化*/
		CC_SWT,
		/**周期递增（至上限）*/
		AU_ADD,
		/**周期递减（至下限）*/
		AU_SUB,
		/**周期循环递增*/
		CC_ADD,
		/**周期循环递减*/
		CC_SUB,
		/**往复递增（从低到高）*/
		SW_ADD,
		/**往复递减（从高到低）*/
		SW_SUB,
		/**按住按钮时周期递加（往复）*/
		PC_ADD,
		/**按住按钮时周期递减（往复）*/
		PC_SUB,
		/**按住按钮时周期递加（循环）*/
		PB_ADD,
		/**按住按钮时周期递减（循环）*/
		PB_SUB,
		/**循环递增（从低到高）*/
		CB_ADD,
		/**循环递减（从高到低）*/
		CB_SUB,
		/**枚举计数用*/
		MAX
	};
};
/**
 * @brief 系统功能
 */
class SIT_EPOBJ {
public:
	enum {
		/**设置界面*/
		SETTINGS,
		/**系统设置*/
		SYSINFO,
		/**日期时间*/
		LOCALTIME,
		/**背光*/
		BACKLIGHT,
		/**以太网*/
		IP,
		/**wifi*/
		WIFI,
		/**4g*/
		FG,
		/**物联网*/
		WEB,
		/**密码*/
		PASS,
		/**网络测试*/
		NETTEST,
		/**wifi连接*/
		WIFILINK,
		/**重启系统*/
		REBOTT,
		/**OTA设置*/
		NETOTA,
		/**文件管理器*/
		FILEBROWER,
		/**串口测试*/
		COMTEST,
		/**分期*/
		STAGES,
		/**密码*/
		CODE,
		/**枚举计数用*/
		MAX
	};
};
/**
 * @brief 长文本显示模式
 */
class LabelLongMode{
public:
	enum {
		/**宽度不变，高度撑开*/
		WRAP 			=	0,
		/**宽度不变，超出部分显示...*/
		DOT				= 	1,
		/**宽度不变，超出可滑动*/
		SCROLL			=	2,
		/**宽度不变，循环滚动*/
		SCROLL_CIRCULAR	= 	3,
		/**宽度不变，切除超出部分*/
		CLIP			=	4,
	};
};
/**
 * @brief 数据库类型
 */
class DBTYPE{
public:
	enum {
		/**Sqlite*/
		SQLITE3,
		/**MySQL*/
		MYSQL,
		/**PostgreSQL*/
		POSTGRESQL,
		/**MSSql*/
		MSSQL,
		/**Redis*/
		REDIS,
	};
};
/**
 * @HIDECA 内置图标
 * @brief 内置图标
 */
class ICON{
public:
	enum {
		/**子弹*/
		BULLET,
		/**音频*/
		AUDIO,
		/**视频*/
		VIDEO,
		/**列表*/
		LIST,
		/**√*/
		OK,
		/**×*/
		CLOSE,
		/**电源*/
		POWER,
		/**设置*/
		SETTINGS,
		/**HOME*/
		HOME,
		/**下载*/
		DOWNLOAD,
		/**驱动*/
		DRIVE,
		/**刷新*/
		REFRESH,
		/**静音*/
		MUTE,
		/**中音*/
		VOLUME_MID,
		/**最大音*/
		VOLUME_MAX,
		/**图像*/
		IMAGE,
		/**TINT*/
		TINT,
		/**上一个*/
		PREV,
		/**播放*/
		PLAY,
		/**暂停*/
		PAUSE,
		/**停止*/
		STOP,
		/**下一个*/
		NEXT,
		/**弹出*/
		EJECT,
		/**左*/
		LEFT,
		/**右*/
		RIGHT,
		/**加*/
		PLUS,
		/**减*/
		MINUS,
		/**睁眼*/
		EYE_OPEN,
		/**闭眼*/
		EYE_CLOSE,
		/**警告*/
		WARNING,
		/**随机播放*/
		SHUFFLE,
		/**上*/
		UP,
		/**下*/
		DOWN,
		/**循环*/
		LOOP,
		/**目录*/
		DIRECTORY,
		/**上传*/
		UPLOAD,
		/**电话*/
		CALL,
		/**剪切*/
		CUT,
		/**复制*/
		COPY,
		/**保存*/
		SAVE,
		/**条*/
		BARS,
		/**信封*/
		ENVELOPE,
		/**充电*/
		CHARGE,
		/**粘贴*/
		PASTE,
		/**铃铛*/
		BELL,
		/**键盘*/
		KEYBOARD,
		/**GPS*/
		GPS,
		/**文件*/
		FILE,
		/**WIFI*/
		WIFI,
		/**电量满*/
		BATTERY_FULL,
		/**电量3*/
		BATTERY_3,
		/**电量2*/
		BATTERY_2,
		/**电量1*/
		BATTERY_1,
		/**电量空*/
		BATTERY_EMPTY,
		/**USB*/
		USB,
		/**蓝牙*/
		BLUETOOTH,
		/**垃圾*/
		TRASH,
		/**编辑*/
		EDIT,
		/**退格*/
		BACKSPACE,
		/**SD卡*/
		SD_CARD,
		/**换行*/
		NEW_LINE,
		/**DUMMY*/
		DUMMY,
	};
	/**
	 * @LEVEL0
	 * @brief 获取内置字体图标资源
	 * @param icon
	 * @return
	 */
	static const char * get(uint8_t icon);
};
/**
 * @brief 事件采集中的比较条件
 */
class EVENT_COMPARE{
public:
	enum {
		/**置OFF*/
		OFF,
		/**置ON*/
		ON,
		/**OFF->ON*/
		OFF_ON,
		/**ON->OFF*/
		ON_OFF,
		/**小于*/
		LE,
		/**大于*/
		GR,
		/**等于*/
		EQ,
		/**大于等于*/
		GR_EQ,
		/**小于等于*/
		LE_EQ,
		/**不等于*/
		NotEQ,
	};
};
/**
 * @brief LV标志
 */
class FLAG {
public:
	enum {
		/**使能隐藏*/
		HIDDEN          = (1L << 0),
		/**使能点击*/
		CLICKABLE       = (1L << 1),
		/**使能点击时聚焦*/
		CLICK_FOCUSABLE = (1L << 2),
		/**使能切换*/
		CHECKABLE       = (1L << 3),
		/**使能滚动*/
		SCROLLABLE      = (1L << 4),
		/**使能弹性滚动*/
		SCROLL_ELASTIC  = (1L << 5),
		/**使能惯性滚动*/
		SCROLL_MOMENTUM = (1L << 6),
		/**只允许滚动一个捕获子控件*/
		SCROLL_ONE      = (1L << 7),
		/**允许传递水平滚动到父控件*/
		SCROLL_CHAIN_HOR = (1L << 8),
		/**允许传递垂直滚动到父控件*/
		SCROLL_CHAIN_VER = (1L << 9),
		/**允许传递滚动到父控件*/
		SCROLL_CHAIN     = (SCROLL_CHAIN_HOR | SCROLL_CHAIN_VER),
		/**当聚焦时自动滚动来让对象可见*/
		SCROLL_ON_FOCUS = (1L << 10),
		/**可有上下键滚动*/
		SCROLL_WITH_ARROW  = (1L << 11),
		/**父控件可滑动时可捕获此控件*/
		SNAPPABLE       = (1L << 12),
		/**按压锁定*/
		PRESS_LOCK      = (1L << 13),
		/**将所有事件传递到父控件*/
		EVENT_BUBBLE    = (1L << 14),
		/**将所有手势传递到父控件*/
		GESTURE_BUBBLE  = (1L << 15),
		/**运行更准确的点击测试*/
		ADV_HITTEST     = (1L << 16),
		/**使能控件位置可由布局管理器调整*/
		IGNORE_LAYOUT   = (1L << 17),
		/**父控件滑动时不滑动此子控件且忽略布局调整*/
		FLOATING        = (1L << 18),
		/**不以父控件边界剪切子控件内容*/
		OVERFLOW_VISIBLE = (1L << 19),
		/**自定义标志，由布局使用*/
		LAYOUT_1        = (1L << 23),
		/**自定义标志，由布局使用*/
		LAYOUT_2        = (1L << 24),
		/**自定义标志，由控件使用*/
		WIDGET_1        = (1L << 25),
		/**自定义标志，由控件使用*/
		WIDGET_2        = (1L << 26),
		/**自定义标志，由用户使用*/
		USER_1          = (1L << 27),
		/**自定义标志，由用户使用*/
		USER_2          = (1L << 28),
		/**自定义标志，由用户使用*/
		USER_3          = (1L << 29),
		/**自定义标志，由用户使用*/
		USER_4          = (1L << 30),
	};
};
/**
 * @brief 圆弧模式
 */
class ARC_Mode{
public:
	enum {
		/**常规，正向*/
	    NORMAL,
		/**对称*/
	    SYMMETRICAL,
		/**反向*/
	    REVERSE
	};
};
/**
 * @brief 指示条模式
 */
class BAR_Mode{
public:
	enum {
		/**常规 */
	    NORMAL,
		/**对称模式: 可以指定下限值为负，从中间位置(对应零值)开始绘制，小于零往左绘制，大于零往右绘制。 */
		SYMMETRICAL,
		/**范围模式: 与对称模式类似，但可以通过setStartValue设置起始值，然后可以从这个起始值绘制指示器。 */
	    RANGE
	};
};
/**
 * @brief 动画效果
 */
class ANIM_TYPE {
public:
	enum {
		/**文字内容变化*/
		TEXT,
		/**文字(前景)颜色变化*/
		FOREGROUND,
		/**背景颜色变化*/
		BACKGROUND,
		/**边框颜色变化*/
		BORDER,
		/**水平移动*/
		HORIZONAL_MOVE,
		/**垂直移动*/
		VERTICAL_MOVE,
		/**大小变化*/
		SIZE_CHANGE
	};
};
/**
 * @brief 渐变方向
 */
class GradDir {
public:
	enum {
		/**不渐变*/
	    NONE,
		/**从上到下*/
	    VER,
		/**从左到右*/
	    HOR,
	};
};
/**
 * @brief 滚动条方向
 */
class ScrollDir {
public:
	enum {
		/**不滚动*/
	    NONE     = 0x00,
		/**向左1*/
	    LEFT     = (1 << 0),		//1
		/**向右2*/
	    RIGHT    = (1 << 1),		//2
		/**向上3*/
	    TOP      = (1 << 2),		//4
		/**向下8*/
	    BOTTOM   = (1 << 3),		//8
		/**水平滚动3*/
	    HOR      = LEFT | RIGHT,	//3
		/**垂直滚动12*/
	    VER      = TOP 	| BOTTOM,	//12
		/**水平+垂直15*/
	    ALL      = HOR 	| VER,		//15
	};
};
/**
 * @brief 日志等级
 */
class DebugLevel{
public:
	enum {
		/**关闭*/
		NO,
		/**只输出LOG*/
	    LOG,
		/**输出LOG、WARN*/
	    WARN,
		/**输出LOG、WARN、ERR*/
	    ERR,
	};
};

/**
 * @brief 系统内置外部按键码值
 */
class SysKeyCode{
public:
	enum{
		/**
		 * @brief ok/确认键
		 */
		OK=240,
		/**
		 * @brief cancel/取消键
		 */
		CANCEL=241
	};
};
}
