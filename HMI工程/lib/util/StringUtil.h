/*
 * 字符串处理辅助类
 *
 *  Created on: 2023年7月25日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"

using namespace std;

namespace stt::util {

/**
 * @brief 字符串类
 */
class StringUtil {
public:
	/**
	 * 规则匹配检查
	 *
	 * 常用表达式
	 * 1. 校验数字的表达式
	 * 		数字 :"^[0-9]*$"
	 * 		n位的数字 :"^[0-9]{4}$"
	 * 		至少 n 位的数字 :"^[0-9]{n,}$"
	 * 		m-n 位的数字 :"^[0-9]{m,n}$"
	 * 		零和非零开头的数字 :"^(0|[1-9][0-9]*)$"
	 * 		非零开头的最多带两位小数的数字 :R"(^[1-9]\d*(\.\d{1,2})?$)"
	 * 		带 1~2 位小数的正数或负数 :R"(^-?\d+(\.\d{1,2})?$)"
	 * 		正数,负数,和小数:"^(-|\\+)?[0-9]+(.[0-9]+)?$"
	 * 		有两位小数的正实数 :R"(^\d+(\.\d{2})?$)"
	 * 		有 1~3 位小数的正实数 :R"(^\d+(\.\d{1,3})?$)"
	 * 		非零的正整数 :"^[1-9][0-9]*$" 或 "^([1-9][0-9]*){1,3}$" 或 "^\\+?[1-9][0-9]*$"
	 * 		非零的负整数 :"^-[1-9][0-9]*$"
	 * 		非负整数 :"^[0-9]+$" 或 R"(^\d+$)"
	 * 		非正整数 :R"(^0$|^[1-9]\d*$)" 或 "^((-[0-9]+)|(0+))$"
	 * 		非负浮点数 : "^[0-9]+(.[0-9]+)?$" 或 R"(^(0|[1-9][0-9]*)\.[0-9]*|[1-9][0-9]*|0(\.0+)?$)"
	 * 		非正浮点数 : "^((-[0-9]+(.[0-9]+)?)|(0+(.0+)?))$" 或 R"(^(-\d+(\.\d+)?|0(\.\d+)?)$)"
	 * 		正浮点数 : R"(^\d+(\.\d+)?$)" 或 "^[+]?([0-9]+([.][0-9]*)?|[.][0-9]+)$"
	 * 		负浮点数 : R"(^-\d+(\.\d+)?$)" 或 "^(-(([0-9]+.[0-9]*[1-9][0-9]*)|([0-9]*[1-9][0-9]*.[0-9]+)|([0-9]*[1-9][0-9]*)))$"
	 * 		浮点数 : "^(-?[0-9]+)(.[0-9]+)?$" 或 R"(^-?\d+(\.\d+)?$)"
	 * 		最多m位整数，n位小数的实数:"^[0-9]{0,m}(.[0-9]{0,n})?$"
	 *
	 * 2. 校验字符的表达式
	 * 		汉字 :"^[\u4e00-\u9fa5]{0,}$"
	 * 		英文和数字 : "^[A-Za-z0-9]+$" 或 "^[A-Za-z0-9]+$"
	 * 		长度为 3~20 的所有字符 : "^.{3,20}$"
	 * 		由 26 个英文字母组成的字符串 : "^[a-zA-Z]+$"
	 * 		由 26 个大写英文字母组成的字符串 : "^[A-Z]+$"
	 * 		由 26 个小写英文字母组成的字符串 : "^[a-z]+$"
	 * 		由数字和 26 个英文字母组成的字符串 : "^[A-Za-z0-9]{4,40}$"
	 * 		由数字 , 26 个英文字母或者下划线组成的字符串 : "^[a-zA-Z0-9_]+$"
	 * 		中文 , 英文 , 数字包括下划线 : "^[\u4E00-\u9FA5A-Za-z0-9_]+$"
	 * 		中文 , 英文 , 数字但不包括下划线等符号 : "^[\u4E00-\u9FA5A-Za-z0-9]+$"
	 * 		可以输入含有^%&',;=?$\"等字符 : R"(.*)"
	 * 		禁止输入含有 ~ 的字符 : "^[^~]+$"
	 *
	 * 3. 特殊需求表达式
	 * 		Email 地址 : R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"
	 * 		域名 : R"([a-zA-Z0-9.-]+\.[a-zA-Z]{2,})"
	 * 		InternetURL : R"(^(https?:\/\/)?([^\s.]+\.[^\s]{2,}|localhost)(\/[^\s]*)?$)"
	 * 		国内手机号码 :R"(^1[3-9]\d{9}$)"
	 * 		国内电话号码 (0511-4405222 , 021-87888822) : R"(^0\d{2,3}-\d{7,8}$)"
	 * 		身份证号 (15 位 , 18 位数字) :"^[1-9][0-9]{5}(19|20)[0-9]{2}(0[1-9]|1[0-2])(0[1-9]|[1-2][0-9]|3[0-1])[0-9]{3}[0-9X]$"
	 * 		短身份证号码 (数字,字母x结尾)1 :"^([0-9]){7,18}(x|X)?$" 或 "^[0-9]{8,18}|[0-9x]{8,18}|[0-9X]{8,18}?$"
	 * 		帐号是否合法(字母开头，允许 5~16 字节，允许字母数字下划线) :"^[a-zA-Z][a-zA-Z0-9_]{4,15}$"
	 * 		密码 (以字母开头，长度在 6~18 之间，只能包含字母 , 数字和下划线) :"^[a-zA-Z][a-zA-Z0-9_]{5,17}$"
	 * 		强密码 (必须包含大小写字母和数字的组合，不能使用特殊字符) :"^(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])[a-zA-Z0-9]+$"
	 * 		日期格式-:/分隔 :R"(^[0-9]{4}[-:/][0-9]{2}[-:/][0-9]{2}$)"
	 *
	 * @param reg	正则表达式(*注意：\w不识别)
	 * @param str	比对的字符串
	 * @return
	 */
	static bool match(const string& reg, const string& str);
	/**
	 * @LEVEL0
	 * @brief 判断字符串是否以某字符串开始
	 * @param s		长串
	 * @param sub	子串
	 * @return
	 */
	static bool startWith(const string& s, const string& sub);
	/**
	 * @LEVEL0
	 * @brief 判断字符串是否以某字符串开始(忽略大小写)
	 * @param s		长串
	 * @param sub	子串
	 * @return
	 */
	static bool startWithNoCase(const string& s, const string& sub);
	/**
	 * @LEVEL0
	 * @brief 判断字符串是否以某字符串结尾(忽略大小写)
	 * @param s		长串
	 * @param sub	子串
	 * @return
	 */
	static bool endWith(const string& s, const string& sub);
	/**
	 * @LEVEL0
	 * @brief 判断字符串是否以某字符串结尾
	 * @param s		长串
	 * @param sub	子串
	 * @return
	 */
	static bool endWithNoCase(const string& s, const string& sub);
	/**
	 * @LEVEL1
	 * @brief 数字转格式化字符串
	 * @param fvalue		要转换的值
	 * @param fraclen		小数位数
	 * @param thousandth	是否显示千分位
	 * @param intnum		整数位(<=0无效)
	 * @param intbl			整数位前缀补零
	 * @param prefix		前缀
	 * @param suffix		后缀
	 * @return				输出结果
	 */
	static string formatFloat(double fvalue, int fraclen, bool thousandth, int intnum=0, bool intbl=false, const string& prefix="", const string& suffix="");
	/**
	 * @LEVEL1
	 * @brief 长整型格式化日期字符串
	 * @param lvalue		秒数 1970.1.1开始
	 * @param deli1			年月日之间的分隔符，如"-","/"；像2023年1月1日，则deli1="中文"
	 * @param deli2			时分秒之间的分隔符，如":"；像12时13分04秒，则deli2="中文"
	 * @param showYear		是否显示年份
	 * @param showMonth		是否显示月份
	 * @param showDay		是否显示日期
	 * @param showHour		是否显示小时
	 * @param showMinute	是否显示分钟
	 * @param showSecond	是否显示秒数
	 * @param showWeek		是否显示星期几
	 * @param isLocal		是否本地时间
	 * @return				根据需求组合显示的字符串
	 */
	static string formatDate(unsigned long long lvalue,
			const string& deli1="-", const string& deli2=":", bool showYear=true, bool showMonth=true, bool showDay=true,
			bool showHour=true,bool showMinute=true,bool showSecond=true,bool showWeek=false, bool isLocal=true);
	/**
	 * @LEVEL1
	 * @brief 浮点数转科学计数法显示
	 * @param fvalue	浮点数
	 * @param fraclen	小数位
	 * @return	输出结果
	 */
	static string formatScience(double fvalue, int fraclen);
	/**
	 * @LEVEL1
	 * @brief 16进制字符串转无符号整型
	 * @param str
	 * @return
	 */
	static uint32_t hexToUInt(const string& str);
	/**
	 * @LEVEL0
	 * @brief 字符串分割成字符串列表
	 * @param str		源字符串
	 * @param split		分割字符
	 * @param res		返回结果
	 */
	static void split(const string& str, const string& split, vector<string>& res);
	/**
	 * @LEVEL0
	 * @brief 字符串列表拼接为字符串
	 * @param src	字符串列表
	 * @param sub	分割字符
	 * @return		字符串
	 */
	static string join(const vector<string>& src, const string& sub);
	/**
	 * @LEVEL0
	 * @brief 按长度分割字符串
	 * @param len	每段长度
	 * @param c		补齐字符，长度不足按指定字符补齐
	 * @param src	被分割的字符串
	 * @return
	 */
	static vector<string> splitByLength(int len, char c, string& src);
	/**
	 * @LEVEL0
	 * @brief 格式化字符串输出，类似printf（注意最后有\n）
	 * @param pszFmt	如:StringUtil::snprintf("%s,%s", "hello", "world")
	 * @return	返回格式化字符串
	 */
	static string snprintf(const char* pszFmt, ...);
	/**
	 * @LEVEL0
	 * @brief 转换为小写
	 * @param src 源字符串
	 * @return 返回小写串
	 */
	static string toLowerCase(const string& src);
	/**
	 * @LEVEL0
	 * @brief 转换为大写
	 * @param src 源字符串
	 * @return  返回大写串
	 */
	static string toUpperCase(const string& src);
	/**
	 * @LEVEL0
	 * @brief 字符串替换
	 * @param str		原字符串
	 * @param from		要替换
	 * @param to		替换为
	 * @return			返回替换后的串
	 */
	static string replace(string str, const string& from, const string& to);
	/**
	 * @LEVEL1
	 * @brief 用正则表达式进行字符串替换
	 * 例子：
	 * string str = "Hello, World!";
	 * result = replace(str, "World", "C++")		//替换所有的 "World" 为 "C++"
	 * result = replace(str, "[Ww]orld", "C++")		//替换所有的 "World" 或 "world" 为 "C++"
	 * result = replace(str, "[a-zA-Z]+", "C++")	//替换所有的单词为 "C++"
	 * result = replace(str, "\\b\\w+\\b", "C++")	//替换所有的单词为 "C++"
	 *
	 * @param str		原字符串
	 * @param pattern	表达式
	 * @param to		替换为
	 * @return
	 */
	static string rreplace(string str, const string& pattern, const string& to);
	/**
	 * @LEVEL2
	 * @brief 字符串编码格式转换
	 * 具体编码字符参考https://www.gnu.org/software/libiconv/
	 * 例如“UTF-8”UCS-2, UCS-2BE, UCS-2LE EUC-CN, HZ, GBK, CP936, GB18030
		UCS-4, UCS-4BE, UCS-4LE
		UTF-16, UTF-16BE, UTF-16LE
		UTF-32, UTF-32BE, UTF-32LE
		C99, JAVA UTF-7
	 * @param from	原始编码方式
	 * @param to	目标编码方式
	 * @param str	源字符串
	 * @return	转换编码方式后的字符串
	 */
	static string trancoding(const string& from, const string& to, const string& str);
	/**
	 * @LEVEL2
	 * @brief 字符数组转换HEX字符串	形如：0x5A->5A
	 * @param data	字符数组指针
	 * @param len	长度
	 * @return
	 */
	static string charsToString(unsigned char* data, int len);
	/**
	 * @LEVEL2
	 * @brief HEX字符串转换字符数组(用户需保证data长度足够)	形如：5A->0x5A
	 * @param str	源字符串
	 * @param data	转换出的字符数组
	 * @return	数组有效长度
	 */
	static int StringToChars(const string& str, unsigned char* data);
	/**
	 * @LEVEL2
	 * @brief vector<uint8_t>转换字符串，要注意vector中如有0，字符串会截断
	 * @param arry		源
	 * @param coding	扩展
	 * @return
	 */
	static string charArrayToString(vector<uint8_t>& arry, const string& coding="");
	/**
	 * @LEVEL2
	 * @brief 字符串vector<uint8_t>
	 * @param src		源
	 * @param coding	扩展
	 * @return
	 */
	static vector<uint8_t> stringToCharArray(const string& src, const string& coding="");
	/**
	 * @LEVEL2
	 * @brief 键值对列表转jsonc字符串
	 * @param arry
	 * @return
	 */
	static string vectorToJsonc(vector<KV_Str_Str>& arry);
	/**
	 * @LEVEL2
	 * @brief string转int方法(返回转换结果)
	 * @param str   源字符串
	 * @return
	 */
	static int safeStoi(const string& str);
	/**
	 * @LEVEL2
	 * @brief string转int方法(可判断转换结果)
	 * @param src      源字符串
	 * @param result   转换结果
	 * @return
	 */
	static bool safeStoi(const string& str, int& result) noexcept;
	/**
	 * @LEVEL2
	 * @brief string转float方法(返回转换结果)
	 * @param str   源字符串
	 * @return
	 */
	static float safeStof(const string& str);
	/**
	 * @LEVEL2
	 * @brief string转float方法(可判断转换结果)
	 * @param src      源字符串
	 * @param result   转换结果
	 * @return
	 */
	static bool safeStof(const string& str, float& result) noexcept;
	/**
	 * @LEVEL2
	 * @brief string转long long方法(返回转换结果)
	 * @param str   源字符串
	 * @return
	 */
	static long long safeStoll(const string& str);
	/**
	 * @LEVEL2
	 * @brief string转long long方法(可判断转换结果)
	 * @param src      源字符串
	 * @param result   转换结果
	 * @return
	 */
	static bool safeStoll(const string& str, long long& result) noexcept;
	/**
	 * @LEVEL2
	 * @brief string转double方法(返回转换结果)
	 * @param str   源字符串
	 * @return
	 */
	static double safeStod(const string& str);
	/**
	 * @LEVEL2
	 * @brief string转double方法(可判断转换结果)
	 * @param src      源字符串
	 * @param result   转换结果
	 * @return
	 */
	static bool safeStod(const string& str, double& result) noexcept;
	/**
	 * @LEVEL2
	 * @brief 字符串trim操作(删除两端的空格)
	 * @param s			源字符串
	 * @return
	 */
	static string trim(string s);
	/**
	 * @LEVEL2
	 * @brief 删除两端的指定的字符
	 * @param s			源字符串
	 * @return
	 */
	static string trim(string s, char c);
	/**
	 * @brief map转json字符串
	 * @param jsonMap
	 * @return
	 */
	static string mapToJson(map<string,string>& jsonMap);
	/**
	 * @brief 指名数据是数字类型
	 * @param str
	 * @return
	 */
	static string vertInt(const string& str);

};

class TransPY {
public:
	/**
	 *  GBK编码转拼音
	 * @param code: 输入GBK编码
	 * @param result   转换结果 例如（“ZHENG”）
	 * @return
	 */
	static const char* getPinyinByCode(uint32_t code);
	/**
	 *  获取拼音
	 * @param str: 源字符串
	 * @param mode: 1 全部大写;2 全部小写;3 拼音首字母大写;4 拼音首字母输出
	 * @param result   转换结果
	 * @return
	 */
	static string getPinyin(const std::string& str, int mode=1);
};

}
