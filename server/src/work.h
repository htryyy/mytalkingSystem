/*
 * work.h
 *
 *  Created on: 2015年4月18日
 *      Author: train
 */

#ifndef WORK_H_
#define WORK_H_

#define CLIENTCOUNT 256
#define BODYBUF 1024
#include <mysql/mysql.h>

struct msg_t	//信息通讯结构体 头表示信息类型 体表示信息内容
{
	unsigned char head[4];
	char body[BODYBUF];
};

class work
{
public:
	work(int port);
	~work();
	void run();
	void sql_init();

private:
	int listen_st;
	int socket_client[CLIENTCOUNT];//该数组用来管理来自不同client的连接
	int setnonblocking(int st);
	int socket_accept();
	int socket_recv(int st);
	void user_logout(int st);
	void sendmsg(const struct msg_t *msg, ssize_t msglen);
	void loginmsg(int st, int o_userid, const char *passwd);
	int auth_passwd(int userid, const char *passwd);
	void fix_socket_client(int index, int st);
	void broadcast_user_status();
	int checkuseridandpwd(const char *strSQL);
	int GetCurTime(char *strTime,int type);

	MYSQL* m_pMyData;                  //msyql 连接句柄
	char g_dbName[30];                 // 数据表名
	char g_tableName[30];
	char g_strIP[15];
	char g_strUser[50];
	char g_strPass[20];
	static const int  g_nPort = 3306;
};

#endif /* WORK_H_ */
