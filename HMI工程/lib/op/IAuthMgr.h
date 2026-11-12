/*
 * IAuthMgr.h
 *
 *  Created on: 2023年8月5日
 *      Author: SIMTUTAI
 */
#pragma once

#include <iostream>
#include <vector>
#include "base/Constants.h"
#include "base/Interface.h"

using namespace std;

/**
 * @namespace stt::op
 * @brief 管理操作(如用户、权限、事件、日志、配方、采样、定时器等)
 */
namespace stt::op{

/**
 * @brief 用户结构体
 */
struct User{
public:
	string uid;
	string name;
	string pwd;
};

/**
 * @brief 权限管理
 */
class AuthMgr{
public:
	/**
	 * @brief 注册用户组
	 * @param grpname	用户组名称
	 * @return			新用户组ID，已存在或其它原因则返回空
	 */
	static string addGroup(string grpname);
	/**
	 * @brief 删除用户组
	 * @param gid		用户组id
	 * @return			返回是否成功
	 */
	static bool delGroup(string gid);
	/**
	 * @brief 重命名用户组
	 * @param gid		用户组id
	 * @param grpname	新的用户组名称
	 * @return			返回是否成功
	 */
	static bool renGroup(string gid, string grpname);
	/**
	 * @brief 添加用户
	 * @param uid		指定用户唯一id.
	 * 					uid为空时系统将生成唯一id。若指定的id已经存在，则添加失败，返回空串""
	 * @param name		指定用户名称.
	 * 					name为空时则添加失败，返回空串""
	 * @param pwd		批定用户密码.
	 * @param gid		指定所属组id.
	 * 					gid为空时，则不分组。不为空，且组不存在时，则添加失败，返回空串""
	 * @return			添加成功则返回新增用户id；失败则返回空串""
	 */
	static string addUser(string uid="", string name="", string pwd="", string gid="");
	/**
	 * @brief 修改密码
	 * @param uname		用户名
	 * @param oldpwd	旧密码
	 * @param newpwd1	新密码
	 * @param newpwd2	确认新密码
	 * @return			返回是否成功
	 */
	static bool changePwd(string uname="", string oldpwd = "", string newpwd1="", string newpwd2="");
	/**
	 * @brief 用户验证
	 * @param uid  	用户ID
	 * @param pwd  	验证密码
	 * @param user 	返回用户信息
	 * @return		返回是否成功
	 */
	static bool userAuth(string uid, string pwd, User& user);
	/**
	 * @brief 用户注销
	 * @return		返回是否成功
	 */
	static bool userLogout();
	/**
	 * @brief 重命名用户
	 * @param uid	用户id
	 * @param uname	用户新名称
	 * @return		返回是否成功
	 */
	static bool renUser(string uid, string uname);
	/**
	 * @brief 删除用户
	 * @param uid	用户id
	 * @return		返回是否成功
	 */
	static bool delUser(string uid);
	/**
	 * @brief 添加用户到组
	 * @param uid		用户id
	 * @param gid		组id
	 * @return			返回是否成功
	 */
	static bool addUserToGroup(string uid, string gid);
	/**
	 * @brief 将用户移出组
	 * @param uid		用户id
	 * @param gid		用户组id
	 * @return			返回是否成功
	 */
	static bool delUserFromGroup(string uid, string gid);
	/**
	 * @brief 获取所有组<组id, 组name>
	 * @return
	 */
	static vector<KV_Str_Str> getAllGroupLists();
	/**
	 * @brief 获取所有用户
	 * @return
	 */
	static vector<User> getUserLists();
	/**
	 * @brief 获取用户所属组(用户可能属于多个组)
	 * @param uid 用户id
	 * @return vector<组id,组名称>
	 */
	static vector<KV_Str_Str> getUserGroups(string uid);
	/**
	 * @brief 获取用户组用户列表
	 * @param gid		用户组id
	 * @return
	 */
	static vector<User> getGroupUsers(string gid);

};

/**
 * 分期管理
 */
class LockMgr{
public:
	/**
	 * @LEVEL2
	 * @brief 获取总期数
	 * @return 总期数
	 */
	static int get_total_qishu();
	/**
	 * @LEVEL2
	 * @brief 获取剩余期数
	 * @return 剩余期数
	 */
	static int get_remain_qishu();
	/**
	 * @LEVEL2
	 * @brief 获取分期日期
	 * @param mid 期数
	 * @return 期数日期 YYYY-MM-DD
	 */
	static string get_qishu_date(int mid);
	/**
	 * @LEVEL2
	 * @brief 获取分期密码
	 * @param mid 期数
	 * @return 分期密码
	 */
	static string get_qishu_pwd(int mid);
	/**
	 * @LEVEL2
	 * @brief 获取是否启动分期
	 * @return 0:禁用 1：启用
	 */
	static int get_fenqi_enable();
	/**
	 * @LEVEL2
	 * @brief 启用/禁用分期
	 * @param x 0:禁用 1：启用
	 * @return 是否成功
	 */
	static bool set_fenqi_enable(int x);
	/**
	 * @LEVEL2
	 * @brief 更新期限设置
	 * @param mid 期数
	 * @param time_st 日期时间（%Y-%m-%d）
	 * @param pwd 密码
	 * @return 是否成功
	 */
	static bool update_setting(int mid,string time_st,string pwd);
	/**
	 * @LEVEL2
	 * @brief 更新期限设置
	 * @param mid 期数
	 * @param year 年（>=2020）
	 * @param month 月
	 * @param day 日
	 * @param pwd 密码
	 * @return 是否成功
	 */
	static bool set_qishu_date(int mid, int year, int month, int day, string pwd);
	/**
	 * @LEVEL2
	 * @brief 创建下拉框选项
	 * @param type "Y":年 "M":月 "D":日
	 * @return Option字符串
	 */
	static string build_wMDropdown_options(string type);
};

}
