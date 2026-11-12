/*
 * STTFFmpeg.h
 *
 *  Created on: 2023年2月19日
 *      Author: SIMTUTAI
 */
#pragma once

#include "widgets/STTWidget.h"
#include "widgets/STTLabel.h"

#define STT_USE_FFMPEG 0

namespace stt{
/**
 * @ingroup widget
 * @brief 视频控件
 */
class STTFFmpeg: public STTWidget {
public:
	/**
	 * @HideCA
	 * @brief 视频播放控件构造函数
	 * @param rt 	运行时接口指针
	 * @param par 	父控件
	 * @param id 	控件ID
	 */
    STTFFmpeg(IRuntime* rt, STTWidget* par, const string& id = "");
    /**
	 * @HideCA
	 * @brief 视频播放控件析构函数
	 */
    ~STTFFmpeg();
    /**
     * @HideCA
     * @brief 初始化FFmpeg，要调用一次
     */
    static void init(void);

    /**
     * @LEVEL1
     * @brief 通过解析文件读取帧数
     * @param path 文件路径
     */
     static int getFrameNum(const char* path);
     /**
      * @LEVEL1
      * @brief 读取当前正在播放视频的帧数、视频宽、视频高、帧率
      * @param path 文件路径
      * @param vw (输出参数) 视频宽
      * @param vh (输出参数) 视频高
      * @param vrate (输出参数) 视频帧率
      */
     static int getFrameNum(string path,int& vw, int& vh, int& vrate);

     /**
      * @LEVEL1
      * @brief 获取已经打开的视频帧数
      */
     int getFrameNum();

     /**
      * @HideCA 弃用。请使用setMainArg(int,int,int,int,string)或setPlayList()
      * @brief 指定视频路径（文件、摄相头、流媒体）
      * @param path 文件路径
      */
     virtual bool setSrc(const char* path);

     /**
      * @LEVEL0
      * @brief 设置视频播放状态
      * @param cmd （0~开始、1~停止、2~暂停、3~继续）
      */
     STTFFmpeg* setCmd(int cmd);
     /**
      * @brief 获取当前播放状态命令
      * （0~开始、1~停止、2~暂停）*/
     int getCmd();
     /**
      * @LEVEL0
      * @brief 开始播放第i个视频
      */
     STTFFmpeg* play();
     /**
      * @LEVEL0
      * @brief 延时启动
      * @param ms 延时的毫秒数
      */
     STTFFmpeg* play(int ms);
     /**
      * @LEVEL0
      * @brief 停止
      * @param ms 当ms=0时，直接stop；当ms>0时(一般取200)，先调用pause，再延时ms后，调用stop
      */
     STTFFmpeg* stop(int ms = 0);
     /**
      * @LEVEL0
      * @brief 暂停
      */
     STTFFmpeg* pause();
     /**
      * @LEVEL0
      * @brief 继续
      */
     STTFFmpeg* resume();

     /**
      * @LEVEL0
      * @brief 视频跳转至指定秒数
      */
     STTFFmpeg* gotoSec(int sec);

     /**
      * @LEVEL0
      * @brief 获取视频时长(秒)
      */
     double getDuration();
     /**
      * @LEVEL0
      * @brief 获取已播放的秒数
      */
     double getPlayedSecond();

     /**
      * @LEVEL0
      * @brief 设置位置
      */
     STTFFmpeg* setPos(int x, int y);

     /**
      * @LEVEL0
      * @brief 设置控件的尺寸
      */
     STTFFmpeg* setSize(int w, int h);

     /**
      * @LEVEL0
      * @brief 设置图片区域大小，一般为传入视频源的原始尺寸
      */
     STTFFmpeg* setImgSize(int w, int h);

     /**
      * @LEVEL0
      * @brief 设置图片控件的缩放。取值范围1/256 ~ 256
      */
     STTFFmpeg* setImgZoom(float z);
     /**
      * @LEVEL1
      * @brief (弃用)设置视频源属性
      */
     STTFFmpeg* setVideoProp(int resolutionX,int resolutionY,int frameRate);
     /**
      * @LEVEL1
      * @brief 设置音量(arm有效，模拟器无效)
      * @param value 播放音量(0~100)
      */
     STTFFmpeg* setVolume(int value);

     /**
      * @HideCA 弃用(作为private函数)
      * @brief 终合设置，自动计算图像缩放比例，适应控件大小
      * @param resolutionX 视频的水平分辨率
      * @param resolutionY 视频的垂直分辨率
      * @param frameRate 视频的帧率
      * @param src 视频源，一般为路径
      */
     STTFFmpeg* setMainArg(int x,int y,int w,int h,int resolutionX,int resolutionY,int frameRate,const char* src);
     /**
      * @LEVEL2
      * @brief 终合设置，自动计算图像缩放比例，适应控件大小
      * @param src 视频源，一般为路径
      */
     STTFFmpeg* setMainArg(int x,int y,int w,int h,const string& src);
     /**
      * @LEVEL2
      * @brief 异步调用setMainArg。用于自动启动播放rtsp，避免打开页面时卡顿
      * @param delayms 延迟ms后开始播放，小于0时，不播放
      * 	即需要自动播放时，delayms>=0，否则delayms设为负值
      * 	适用情况：rtsp流
      */
     STTFFmpeg* setMainArgAsync(int x,int y,int w,int h,const string& src,int delayms);
     /**
      * @LEVEL2
      * @brief 如已经设置过位置和大小，且不变，可使用本方法
      * @param src 视频源，一般为路径
      */
     STTFFmpeg* setMainArg(const string& src);
     /**
      * @LEVEL1
      * @brief 设置自动重播。开启后，多个文件时，一个播完会播放下一个
      */
     STTFFmpeg* setAutoRestart(bool en);

     /**
      * @LEVEL2
      * @brief 设置声音设备的初始化函数（如不需要在FFMPEG内部初始化，则设为NULL）
      * @params  采样率、是否单声道
      */
     STTFFmpeg* setAudioInitCb(void (*init_func)(int,bool));
     /**
      * @LEVEL2
      * @brief 设置声音设备的销毁函数（如不需要在FFMPEG内部销毁，则设为NULL）
      */
     STTFFmpeg* setAudioDeInitCb(void (*deinit_func)(void));
     /**
      * @LEVEL2
	   * @brief 设置声音设备的播放函数
	   */
     STTFFmpeg* setAudioPlayCb(int (*play_func)(uint8_t *data, int length));
     /**
      * @LEVEL2
      * @brief 设置声音设备的音量调节函数（如不通过FFMPEG方法调节，则设为NULL）
      * @param volume 音量0~100
      */
     STTFFmpeg* setAudioVolumeCb(void (*Volume_func)(int volume));

     /**
      * @HideCA
      * @brief 是否调用过init()
      */
     static bool IsInited;

public:
     /**
      * @LEVEL1
      * @brief 隐藏控件
      */
     STTWidget* setHidden(bool ishidden);
     /**
      * @LEVEL0
      * @brief 设置视频旋转，设置后需调用play生效(一般跟随屏幕旋转，如屏幕旋转270度，视频要调用本方法旋转270度)
      * @param r [0..3]或[0,90,180,270] (0~0度 1~90度 2~180度 3~270度)
      * @note 本方法在屏幕旋转时相应调用，如仅旋转视频内容，请使用setRotateContent
      */
     STTFFmpeg* setRotate(int r);
     /**
      * @LEVEL0
      * @brief 设置视频内容旋转，设置后需调用play生效（目前仅对屏幕有效，对模拟器无效）
      * @param r [0..3]或[0,90,180,270] (0~0度 1~90度 2~180度 3~270度)
      * @note 本方法仅旋转视频内容，如需随屏幕旋转，请使用setRotate
      */
     STTFFmpeg* setRotateContent(int r);
     /**
      * @LEVEL0
      * @brief 设置播放清单(多个文件名),会对当前列表重新设置，调用时需要关闭视频
      * @param vec 播放清单
      * @param resetSrc 是否调用setsrc重新设置当前列表中的第1个文件为当前播放文件
      */
     STTFFmpeg* setPlayList(vector<string> vec, bool resetSrc = true);
     /**
      * @LEVEL0
      * @brief 设置播放清单(单个文件名),会对当前列表重新设置，调用时需要关闭视频
      */
     STTFFmpeg* setPlayList(string path);
     vector<string>* getPlayList();
     /**
      * @LEVEL0
      * @brief 添加路径至播放清单(如果没有调用过setPlayList，则需调用PlayNext或PlayPrev进行播放)
      * @param path 媒体文件路径
      * @param allowSame 是否允许相同文件
      */
     STTFFmpeg* addPlayList(string path, bool allowSame=false);
     /**
      * @LEVEL0
      * @brief 添加多个路径至播放清单(如果没有调用过setPlayList，则需调用PlayNext或PlayPrev进行播放)
      * @param paths 多个媒体文件路径
      * @param allowSame 是否允许相同文件
      */
     STTFFmpeg* addPlayList(vector<string> paths, bool allowSame=false);
     /**
      * @LEVEL0
      * @brief 移出播放清单(如果该文件正在播放，则不移出)
      * @param path 要移出的文件，为空时，移除全部(如果正在播放，则不会移除在播文件)
      * @param once 是否是移出1次（当有多个相同都要移出时，要设为false）
      */
     STTFFmpeg* removePlayList(string path="", bool once=true);
     /**
      * @LEVEL0
      * @brief 播放清单中的下一个文件(到最后了则播放第一个)
      */
     STTFFmpeg* playNext();//播放列表中的下一个文件
     /**
      * @LEVEL0
      * @brief 播放清单中的上一个文件(到首了则播放最后一个)
      */
     STTFFmpeg* playPrev();//播放列表中的上一个文件
     /**
      * @HideCA
      * @brief 内部方法。（跳转到第n秒）
      */
     virtual void onDataChanged(SttObject * org, IVariable* data);
public:
     /**
      * @HideCA
      * @brief 当前要播放的文件
      */
     string filePath;
     /**
      * @HideCA
      * @biref 当前正在播放的文件的列表索引
      */
     int iPlaying;
     /**
      * @HideCA
      * @brief 当前播放的是否是视频文件(用于arm情况，mp3使用lv_ffmpeg, mp4使用ffplayer)
      */
     bool isAV;
     /**
      * @HideCA
      * @brief 指向最后构造的STTFFmpeg，目前用于win下的视频播放列表
      */
     static STTFFmpeg* instance;
     uint8_t channel;
     /**
      * @HideCA
      * @brief 是否开启硬解码
      * @note 只能使用一路硬解码，其他路只能使用软解码。构造后，先设置此项
      */
     bool enHardDecode;
     /**
      * @HideCA
      * @brief	是否开启水平镜像
      * @example	wMVideo1->stop(); wMVideo1->enFlipHor = true; wMVideo1->play();
      */
     bool enFlipHor;
public:
     /**
      * @HideCA
      * @brief 视频分辨率x
      */
     int resX;
     /**
      * @HideCA
      * @brief 视频分辨率y
      */
     int resY;
     /**
      * @HideCA
      * @brief 是否视频
      */
     bool isAv();
     /**
      * @LEVEL2
      * @brief 保存当前播放帧图片(正在播放时有效)
      * @param path 保存路径(后缀名可为png或jpg)
      */
     bool saveJpg(string path);
     void setImage(string path);
     /**
      * @LEVEL2
      * @brief 是否忽略音频/是否向硬件发送音频数据(模拟器无效)
      */
     void ignoreAudio(bool b);
     /**
      * @LEVEL2
      * @brief 是否忽略音频/是否向硬件发送音频数据(模拟器无效)
      */
     static void ignoreAudioHard(bool b, uint8_t ch=0);
     /**
      * @LEVEL2
      * @brief 插播wav声音文件
      */
     static void intercutWav(string path,uint8_t ch=0);
     /**
      * @brief 设置rtsp连接类型，默认为"tcp"，可设置为"udp"
      */
     static string rtsp_transport;
     /**
      * @brief 获取状态(只判断rtsp状态)
      * @return
      * (0x0000~无状态)
      * (0x0040~超时)
      * (0x2000~正在加载)
      * (0x4000~网络中断)
      */
     int getRtspStatus();
     /**
      * @brief rtsp流媒体状态标签。可直接修改其样式、隐藏
      */
     STTLabel* statusLabel;
private:
public:
     /**指定绑定命令的变量 0开始 1停止
      * 用于在循环播放时，一首播放完成则变量变为1。下一首开始时变量变为0。需配合变量消息使用*/
     void setVarOfCmd(string vname);
     /**获取绑定命令的变量 0开始 1停止
      * 用于在循环播放时，一首播放完成则变量变为1。下一首开始时变量变为0。需配合变量消息使用*/
     Ivariable getVarOfCmd();
     /**设置是否随机播放(当播放清单中的文件大于1个时有效)*/
     void setRandPlay(bool b);
     /**获取是否随机播放*/
     bool isRandPlay();
     /**是否循环播放(只对arm有效)，用户使用setAutoRestart即可*/
     void enLoop(bool b);
     /**是否循环播放*/
     bool isLoop();
     /**清空缓冲帧，降低延迟*/
     void flushBuffer();
     /**
      * @brief 延时加载rtsp
      * @param vecffmpeg 视频控件列表
      * @param vecms 对应视频控件的延时播放时间ms
      * @param vecurl 视频控件对应的播放rtsp地址
      */
     static void delayLoadRtsp(vector<STTFFmpeg*> vecffmpeg,vector<int> vecms, vector<string> vecurl);

};

typedef STTFFmpeg* SttFFmpeg;

/**
 * @brief 软解码
 */
class STTFFmpegSoft: public STTFFmpeg {
public:
	STTFFmpegSoft(IRuntime* rt, STTWidget* par, string desc = "");
	static int getFrameNum(const char* path);
	static int getFrameNum(string path,int& vw, int& vh, int& vrate);
	int getFrameNum();
	bool setSrc(const char *path);
	STTFFmpeg* setCmd(int cmd);
	STTFFmpeg* play();
	STTFFmpeg* stop();
	STTFFmpeg* pause();
	STTFFmpeg* resume();
	STTFFmpeg* gotoSec(int sec);
	double getDuration() ;
	double getPlayedSecond() ;
	STTFFmpeg* setAutoRestart(bool en);
	STTFFmpeg* setVolume(int value);
	STTFFmpeg* setRotate(int r);
	STTFFmpeg* setMainArg(int x, int y, int w, int h, int resolutionX,
			int resolutionY, int frameRate, const char *src);
	STTFFmpeg* setMainArg(int x,int y,int w,int h,const string& src);
};

typedef STTFFmpegSoft* SttFFmpegSoft;

}
