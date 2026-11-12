/*
 * IDataBase.h
 *
 *  Created on: 2023年5月9日
 *      Author: SIMTUTAI
 */
#pragma once

#include "base/Interface.h"
#include "type/TTable.h"

using namespace std;
using namespace stt;
using namespace stt::data;

#define ENSURE_COMMIT(db) \
	if(db->m_transing) { \
		db->commit_transaction();\
		db->begin_transaction();\
	}
/**
 * @namespace stt::db
 * @brief 数据库操作(支持sqlite, mysql, mssql, pgsql, redis)
 */
namespace stt::db{

	/**
	 * @brief 数据库状态
	 */
	class DbStatus{
	public:
		enum {
			CONNECT_FAIL = -1,
			EMPTY_SQL_LIST = -2,
		};
	};


/**
 * @brief 数据库接口定义
 */
interface IDatabase {
public:
	/**
	 * @HideCA
	 */
	virtual ~IDatabase(){}

	/**
	 * @HideCA
	 * @brief 打开数据库
	 * @param url : 	sqlite~文件路径 	| mysql~ip地址 				| postgresql~连接串，如("host=127.0.0.1 port=5432 dbname=msdf user=postgres password=123456")
	 * @param uid : 	sqlite~无效 		| mysql~用户名(标准认证模式) 	| postgresql~无效
	 * @param pwd : 	sqlite~无效 		| mysql~密码 				| postgresql~无效
	 * @param dbname : 	sqlite~无效 		| mysql~数据库 				| postgresql~无效
	 * @param port: 	sqlite~无效 		| mysql~端口  				| postgresql~无效
	 * @return 		成功1 失败0
	 */
	virtual bool connect(string url, string uid, string pwd, string dbname, int port = 0) = 0;
	/**
	 * @HideCA
	 * @brief 插入记录
	 * @param sql	insert语句
	 * @return 		成功"1" 失败"0"
	 */
	virtual bool insert(string sql) = 0;
	/**
	 * @LEVEL0
	 * @brief 更新记录
	 * @param sql	update\delete语句
	 * @return 		成功1 失败0
	 */
	virtual bool execute(string sql) = 0;
	/**
	 * @LEVEL0
	 * @brief 更新批量记录(通过事务)
	 * @param	sqls	update\delete语句数组
	 * @return			成功1 失败0
	 */
	virtual bool execute(list<string>& sqls) = 0;
	/**
	 * @LEVEL0
	 * @brief 查询
	 * @param sql	select语句
	 * @return 		返回TTable表
	 */
	virtual TTable query(string sql) = 0;
	/**
	 * @LEVEL0
	 */
	virtual void query(string sql, TTable* table) = 0;
	/**
	 * @HideCA
	 * @brief 关闭数据库
	 * @return		成功1 失败0
	 */
	virtual bool close() = 0;
	/**
	 * @HideCA
	 * @brief 返回数据库对象
	 * @return 		各子类返回自己的数据库对象，如sqlite3*, redisContext*, PGconn*
	 */
	virtual void * getDb() = 0;
	/**
	 * @HideCA
	 * @brief 创建Statement对象
	 * @return 		各子类返回自己的Statement对象
	 */
	virtual void * getStatement(string sql) = 0;
	/**
	 * @HideCA
	 * @brief 关闭Statement对象
	 */
	virtual void closeStatment() = 0;
	/**
	 * @LEVEL0
	 * @brief 启动事务
	 */
	virtual void begin_transaction() = 0;
	/**
	 * @LEVEL0
	 * @brief 若事务启动，则提交事务
	 */
	virtual void commit_transaction() = 0;
	/**
	 * @LEVEL0
	 * @brief 回滚事务
	 */
	virtual void rollback_transaction() = 0;
	/**
	 * @HideCA
	 * @brief 获取数据库类型
	 * @return
	 */
	virtual uint8_t getDBType() = 0;
	/**
	 * @HideCA
	 * @brief 设置连接、响应超时时间(ms)
	 * @param connect 连接超时时间(sqlite不用设连接超时)
	 * @param responce 响应超时时间
	 */
	virtual void setTimeout(int connect, int response);
	/**
	 * @HideCA
	 * @brief 连接是否alive
	 */
	virtual bool isConnectionAlive();

public:
	/** @brief 上次操作的信息码。见相应数据库的信息码*/
	int infoCode;
	/** @brief 上次操作的信息码意义。见相应数据库的信息码意义 */
	string infoContent;
	/**连接超时，单位ms*/
	int timeoutConnect;
	/**响应超时，单位ms*/
	int timeoutResponse;
	/**是否正在进行事务处理*/
	bool m_transing = false;
	/**
	 * 连接参数
	 */
	string m_dbid;
	uint8_t m_dbtype;
	string m_url;
	int m_port;
	string m_uid;
	string m_pwd;
	string m_dbname;
protected:
	/**
	 * @HideCA
	 * @brief 检查连接
	 */
	virtual void checkConnection() = 0;
};

/**
 * @brief 数据库管理器接口定义
 */
class DBMgr {
public:
	/**
	 * @LEVEL0
	 * @brief 动态加载对应的动态库后，打开并连接数据库。如dbid已经存在，则直接已经存在的数据库类指针
	 * 若dbid已存在，则取相应的数据库连接。若不存在，先根据后面的参数连接，再取该连接
	 * 全部为默认值时，即getdb()，取系统的sqlite数据库(default.db)
	 * @param dbid 		自定义的id。不为空时其他（MYSQL,PG,MSSQL,REDIS）数据库，为空时，取系统的默认sqlite数据库default.db
	 * @param dbtype	数据库类型，见常量DBTYPE(支持sqlite,mysql,mssql,postgre,redis)
	 * @param url		数据库服务器ip。其中：当dbtype==T_SQLITE时，为文件路径。
	 * 		当dbtype==T_POSTGRESQL时为连接串，如("host=127.0.0.1 port=5432 dbname=msdf user=postgres password=123456)
	 * @param port		端口号。当dbtype==T_SQLITE时，无效。(mysql 3306, mssql 1433, postger 5432, redis 6379)
	 * @param uid		用户名。当dbtype==T_SQLITE时，无效。注意MySQL服务器的用户验证方式要设为标准
	 * @param pwd		密码。当dbtype==T_SQLITE时，无效。
	 * @param dbname	数据库系统中的数据库名称。
	 * @return 连接成功，返回数据库指针；连接失败，返回NULL
	 *
	 * @warning getdb会产生一个指针，在close后，需用delete清理
	 * @example
	 * 			1.连接sqlite: getdb("./eventlog.db",0,"./eventlog.db");
	 * 			2.连接mysql: getdb("ididid",1,"192.168.1.99",3306,"username","password","dbname");
	 * 			3.连接POSTGRESQL: getdb("idid",2,"192.168.1.99",5432,"username","password","dbname");
	 * 			4.连接mssql: getdb("id",3,"192.168.1.99",1433,"username","password","dbname");
	 */
	static IDatabase* getdb(string dbid = "", uint8_t dbtype = DBTYPE::SQLITE3
			, string url = "", int port = 3306, string uid = "", string pwd = "", string dbname = "") ;
	/**
	 * @HideCA
	 * @brief 释放各数据库的动态链接库
	 * @param dbname	释放的数据库对象名(nullptr:所有)
	 */
	static void release(string dbname);

	/**
	 * @HideCA
	 * @brief 将自带的sqlite数据库恢复出厂设置
	 * @param drop	清除已有数据
	 */
	static void restore(bool drop = true);

	/**
	 * @HideCA
	 * @brief 获取当前可用种子
	 * @param seed	种子字符串
	 * @return		返回同一个种子调用getSeed的次数(记录保存在默认数据库中)
	 */
	static string getSeed(string seed);

	/**
	 * @HideCA
	 * 退出程序时应调用一下，确保事务提交。
	 */
	static void commitAll();
	/**
	 * @HideCA
	 * @brief 退出程序时，应调用一下，确保连接关闭
	 */
	static void closeAll();
	/**
	 * @HideCA
	 */
	static void rollbackAll();
	/**
	 * @HideCA
	 * @brief 移出map。关闭连接后调用(使用DBMgr::remove(db)，而不是用delete db)
	 */
	static void remove(IDatabase* db);
	//static string getId(IDatabase* db);
	/**
	 * @HideCA
	 * @brief 数据库是否被打开，是则返回指针，否则返回NULL
	 */
	static IDatabase* isDbConnected(const string& path);
};

}
