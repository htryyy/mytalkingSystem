/*
 * mymysql.h
 *
 *  Created on: 2015年4月30日
 *      Author: train
 */

#ifndef MYMYSQL_H_
#define MYMYSQL_H_
#include <mysql/mysql.h>
#define SOCKET_MAXBUFF 1024
struct UserNode
{
	// 在线用户信息结点
	int  id;
	char strName[20]; // 姓名
	char password[10]; // 所在的密码
	char strIP[16];   // 所在IP地址
	int  port;  // 端口号
	char startdate[30];
	char enddate[30];
};
class mymysql {
public:
	mymysql();
	~mymysql();
	void checkuseridandpwd(const char *strSQL);
private:
	void sql_error();
	void sql_init();
	void sql_free();


	MYSQL  m_pMyData;                  //msyql 连接句柄
	char g_dbName[30];                 // 数据表名
	char g_tableName[30];
	char g_strIP[15];
	char g_strUser[50];
	char g_strPass[20];
	int  g_nPort = 3306;
};

#endif /* MYMYSQL_H_ */
