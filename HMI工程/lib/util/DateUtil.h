/*
 * 日期辅助类
 *
 *  Created on: 2023年7月28日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace stt::util{

/**
 * @brief 日期类
 */
class DateUtil{
public:
	/**
	 * @LEVEL0
	 * @brief 获取系统当前日、月、年、时、分、秒
	 * @param f	[Y,M,D,hh,mm,ss] Y取年；M取月[1..12]；D取日期；hh取小时；mm取分钟；ss取秒数
	 * @return 指定的f相应的值
	 * @usage 如getDate("Y")得到2024
	 */
	static int getDate(string f);
	/**
	 * @LEVEL0
	 * @brief 返回本地今天日期yyyyMMdd整数
	 */
	static int today();
	/**
	 * @HideCA
	 * @brief 获取指定时间的time_t时间(要先转换成标准时区的时间，如当前是北京时间11:00，则hour=11-8=3。可再写一个addHour方法)
	 */
	static time_t getUnixTime(int year,int month,int day,int hour = 0,int minute = 0,int second = 0);
	/**
	 * @LEVEL0
	 * @brief 增减天数
	 * @param year 增减后会被修改
	 * @param month 增减后会被修改
	 * @param day 增减后会被修改
	 * @param num  增加的天数，负数减少
	 */
	static void addDays(int& year, int& month, int& day, int num);
	/**
	 * @brief 返回yyyymmdd的前一天
	 */
	static int prevDay(int yyyymmdd);
	/**
	 * @brief 返回yyyymmdd的后一天
	 */
	static int nextDay(int yyyymmdd);
	/**
	 * @LEVEL0
	 * @brief 获取时间戳(毫秒，从开机开始计算)
	 */
	static uint64_t getMsTimeStamp();
	/**
	 * @LEVEL0
	 * @brief 获取时间戳(毫秒，从1970开始计算)
	 */
	static uint64_t getMsTimeStamp2();
	/**
	 * @LEVEL0
	 * @brief 获取指定时刻时间戳(毫秒，从1970开始计算，会减掉时区)
	 * @param ymd,hour,minute... 本地时间
	 */
	static uint64_t getMsTimeStamp2(int ymd,int hour,int minute,int second,int ms);
	/**
	 * @LEVEL0
	 * @brief 获取指定时刻时间戳(毫秒)
	 * @param strdate 日期的字符串，格式如2025-01-01 12:13:14(由"-"、" "、":"分隔)
	 * @param isstart true则补0ms，false则补999ms
	 */
	static uint64_t getMsTimeStamp2(string strdate,bool isstart=true);
	/**
	 * @LEVEL0
	 * @brief 获取时间戳(秒，从1970开始计算)
	 */
	static uint64_t getTimeStamp();
	/**
	 * @LEVEL0
	 * @brief 获取星期几
	 * @param y		年
	 * @param m		月
	 * @param d		日
	 * @return 返回[0..6]
	 */
	static uint8_t getWeekDay(uint32_t y, uint8_t m, uint8_t d);
	/**
	 * @LEVEL0
	 * @brief 获取月份的天数
	 * @param y		年
	 * @param m		月
	 * @return 天数
	 */
	static uint8_t getMonthMaxDay(uint16_t y, uint8_t m);
	/**
	 * @LEVEL1
	 * @brief 获取系统当前时间字符串
	 * @param f  格式字符串, 类似"%Y-%m-%d %H:%M:%S"
	 * 详细参数，如下：
		%a 星期几的简写
		%A 星期几的全称
		%b 月份的简写
		%B 月份的全称
		%c 标准的日期的时间串
		%C 年份的前两位数字
		%d 十进制表示的每月的第几天
		%D 月/天/年
		%e 在两字符域中，十进制表示的每月的第几天
		%F 年-月-日
		%g 年份的后两位数字，使用基于周的年
		%G 年份，使用基于周的年
		%h 简写的月份名
		%H 24小时制的小时
		%I 12小时制的小时
		%j 十进制表示的每年的第几天
		%m 十进制表示的月份
		%M 十时制表示的分钟数
		%n 新行符
		%p 本地的AM或PM的等价显示
		%r 12小时的时间
		%R 显示小时和分钟：hh:mm
		%S 十进制的秒数
		%t 水平制表符
		%T 显示时分秒：hh:mm:ss
		%u 每周的第几天，星期一为第一天 （值从1到7，星期一为1）
		%U 第年的第几周，把星期日作为第一天（值从0到53）
		%V 每年的第几周，使用基于周的年
		%w 十进制表示的星期几（值从0到6，星期天为0）
		%W 每年的第几周，把星期一做为第一天（值从0到53）
		%x 标准的日期串
		%X 标准的时间串
		%y 不带世纪的十进制年份（值从0到99）
		%Y 带世纪部分的十制年份
		%z，%Z 时区名称，如果不能得到时区名称则返回空字符。
		%% 百分号
	 常用： yyyy-mm-dd hh:mm:ss = %Y-%m-%d %H:%M:%S
	 * @return
	 */
	static string getTimeString(const std::string& format="%Y-%m-%d %H:%M:%S");
	/**
	 * @LEVEL1
	 * @brief 设置系统当前时间
	 * @param year
	 * @param month
	 * @param day
	 * @param hour
	 * @param min
	 * @param sec
	 * @return
	 */
	static bool setDateTime(int year,int month,int day,int hour,int min,int sec);
	/**
	 * @LEVEL1
	 * @brief 时区获取
	 * @return -12 ~ +12(- is west, + is east)
	 */
	static int getTimezone(void);
	/**
	 * @LEVEL1
	 * @brief 设置时区
	 * @param tz	-12 ~ +12(- is west, + is east)
	 */
	static void setTimezone(int tz);
	/**
	 * @LEVEL0
	 * @brief 将秒数转换为时:分:秒
	 * @param seconds 秒数
	 * @param pad0	只有个位数时，是否补0
	 */
	static string convert2HMS(int seconds,bool pad0=true,bool mod24=false);
	/**
	 * @LEVEL0
	 * @brief 字符串转日期时间
	 * @param timeStr	日期时间字符串
	 * @param format	格式（完整：%Y-%m-%d %H:%M:%S）
	 * @return  日期
	 */
	static system_clock::time_point parse(const string& timeStr, const string& format="%Y-%m-%d %H:%M:%S");
	/**
	 * @LEVEL0
	 * @brief 日期秒数差值(time1-time2, 单位：秒)
	 * @param time1	日期1,如 2025-01-20 15:00:03
	 * @param time2	日期2,如 2025-01-20 15:00:03
	 * @return	秒数
	 */
	static int diffInSeconds(const string& time1, const string& time2);
	/**
	 * @LEVEL0
	 * @brief 将13位的毫秒数转为日期时间格式
	 * @param ms 毫秒数
	 * @param format sqlite的日期格式，如%Y-%m-%d %H:%M:%S，这也是为空时的默认格式。
	 */
	static string toString(uint64_t ms, const string& format="");
};

}
