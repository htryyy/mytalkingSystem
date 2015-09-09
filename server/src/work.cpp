/*
 * work.cpp
 *
 *  Created on: 2015年4月18日
 *      Author: train
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <time.h>
#include "work.h"
#include "pub.h"


work::work(int port)
{
	char strTime[100];
	GetCurTime(strTime,0);
	printf("MyQQ聊天服务器\n");
	printf("%s\n",strTime);
	sql_init();
	memset(socket_client,0,sizeof(socket_client));
	listen_st=socket_create(port);
	if(listen_st == 0)
	{
		exit(-1);
	}

}
work::~work()
{
	if (listen_st)
		close (listen_st);
}

void work::run()//建立epoll模型，监控多客户端接入
{
	struct epoll_event ev,events[CLIENTCOUNT];//声明epoll_event结构体变量，ev用于注册事件，数组用于回传要处理的事件
	setnonblocking(listen_st);//设置服务端socket的listen为非阻塞,用于多线程的连入
	int epfd = epoll_create(CLIENTCOUNT);//建立epoll，包含参数为能够接受多少客户端连入
	ev.data.fd = listen_st; //将listen放入epoll
	ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;//监控epoll进入，错误，关闭消息
	epoll_ctl(epfd, EPOLL_CTL_ADD,listen_st,&ev);

	int st = 0; //设置一个来自客户端的socket变量，并且初始化为0
	while(1)
	{
		//阻塞，知道epoll当中socket有指定的信息产生，产生返回值，执行下面步骤
		int nfds = epoll_wait(epfd, events,CLIENTCOUNT,-1);//阻塞，-1永久等待，0不等待，>0等待对应数值的时间
		if(nfds == -1)
		{
			printf("epoll_wait failed %s\n",strerror(errno));
			break;
		}

		for(int i = 0;i<nfds;i++)//循环处理epoll_wait返回的所有出事的socket
		{
			if(events[i].data.fd <0)//无效socket
			{
				continue;
			}
			if(events[i].data.fd == listen_st)//listen_st收到来自客户端的connect
			{
				st = socket_accept();
				if(st >=0)
				{
					setnonblocking(st);
					ev.data.fd = st;
					ev.events = EPOLLIN | EPOLLERR | EPOLLHUP ;
					epoll_ctl(epfd,EPOLL_CTL_ADD,st,&ev);
					continue;
				}ep
			}
			if(events[i].events & EPOLLIN)//来自客户端的数据
			{
				st = events[i].data.fd;//得到有数据的socket
				if(socket_recv(st)<=0)//出错了
				{
					 user_logout(st);
					 events[i].data.fd = -1;
				}
			}

			if(events[i].events & EPOLLERR)//来自客户端的出错数据
			{
				st = events[i].data.fd;
				user_logout(st);
				events[i].data.fd = -1;
			}

			if(events[i].events & EPOLLHUP)//来自客户端的关闭数据
			{
				st = events[i].data.fd;
				user_logout(st);
				events[i].data.fd = -1;
			}
		}
	}
	close(epfd);
}

int work::setnonblocking(int st) //将socket设置为非阻塞
{
	int opts = fcntl(st, F_GETFL);
	if (opts < 0)
	{
		printf("fcntl failed %s\n", strerror(errno));
		return 0;
	}
	opts = opts | O_NONBLOCK;
	if (fcntl(st, F_SETFL, opts) < 0)
	{
		printf("fcntl failed %s\n", strerror(errno));
		return 0;
	}
	return 1;
}

int work::socket_accept()
{
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	memset(&client_addr, 0, sizeof(client_addr));
	int client_st = accept(listen_st, (struct sockaddr *) &client_addr, &len); //接收到来自client的socket连接

	if (client_st < 0)
		printf("accept failed %s\n", strerror(errno));
	else
		printf("accept by %s\n", inet_ntoa(client_addr.sin_addr));
	return client_st; //返回来自client端的socket描述符
}

int work::socket_recv(int st)
{
	struct msg_t msg;
	memset(&msg, 0, sizeof(msg));
	ssize_t rc = recv(st, (char *)&msg, sizeof(msg), 0); //接收来自client socket发送来的消息
	if (rc <= 0) //接收失败
	{
		if (rc < 0)
			printf("recv failed %s\n", strerror(errno));
	} else //接收成功
	{
		switch (msg.head[0])
		{
		case 0: //send消息
			sendmsg(&msg, rc);
			break;
		case 1: //login消息
			loginmsg(st, msg.head[1], msg.body);
			break;
		default: //无法识别的消息
			printf("login fail, it's not login message,%s\n",
					(const char *) &msg);
			msg.head[0] = 2; //系统消息
			msg.head[1] = 0; //无法识别的消息
			msg.head[2] = 0; //暂时保留，填0
			msg.head[3] = 0; //暂时保留，填0
			send(st, (const char *) &msg, sizeof(msg.head), 0); //给client端socket下发系统消息
			return 0;
		}
	}
	return rc;

}

void work::user_logout(int st) //client socket连接断开
{
	for (int i = 0; i < CLIENTCOUNT; i++)//循环遍历socket_client[]数组
	{
		if (socket_client[i] == st)//找到socket_client[]数组 中与相等的client socket
		{
			printf("userid=%d,socket disconn\n", i);
			close (socket_client[i]);//关闭socket_client[]数组中相应的socket
			socket_client[i] = 0;//将数组socket_client[]中相应的元素初始化为0，以便该userid下次还可以继续登录
			//broadcast_user_status(); //向socket_client数组中所有socket广播用户状态消息
			return;
		}
	}
}

void work::sendmsg(const struct msg_t *msg, ssize_t msglen) //处理send消息
{
	if ((msg->head[2] < 0) || (msg->head[2] >= CLIENTCOUNT)) //没有这个目标用户
	{
		printf("%d:have not this userid\n", msg->head[2]);
	} else
	{
		if (socket_client[msg->head[2]] == 0) //目的user不在线
		{
			printf("%d:userid not online\n", msg->head[2]);
		} else
		{
			//给client端socket下发send消息
			send(socket_client[msg->head[2]], (const char *) msg, msglen, 0);
			printf("send message:%dto%d-%s\n", msg->head[1], msg->head[2],
					msg->body);
		}
	}
}
void work::loginmsg(int st, int o_userid, const char *passwd) //处理login消息
{
	struct msg_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.head[0] = 2; //系统消息
	msg.head[2] = 0; //暂时保留，填0
	msg.head[3] = 0; //暂时保留，填0

	if ((o_userid < 0) || (o_userid >= CLIENTCOUNT)) //无效的userid
	{
		printf("login failed, %d:invalid userid\n", o_userid);
		msg.head[1] = 1; //无效userid
		send(st, (const char *) &msg, sizeof(msg.head), 0);
		close(st);
		return;
	}
	if (!auth_passwd(o_userid, passwd))//验证用户登录UserId和密码
	{
		printf("login failed, userid=%d,passwd=%s:invalid password\n", o_userid,
				passwd);
		msg.head[1] = 2; //无效密码
		//给client端socket下发系统消息
		send(st, (const char *) &msg, sizeof(msg.head), 0);
		close(st);//验证失败，关闭client socket，函数返回
		return;
	}

	printf("%d:login success\n", o_userid);
	fix_socket_client(o_userid, st); //将登录密码验证通过的客户端client安装到socket_client[]的数组中
	broadcast_user_status(); //向socket_client数组中所有socket广播用户状态消息
}

int work::auth_passwd(int userid, const char *passwd)//验证用户密码
{
	char sSQL[200];
	memset(sSQL,0 ,sizeof(sSQL));
	sprintf(sSQL,"select userid,passwd from qquser where userid=%d and passwd='%s'",userid,passwd);
	printf("sSQL:\n%s\n",sSQL);
	if(checkuseridandpwd(sSQL)>0)
	{
		printf("check success\n");
		return 1;
	}
		return 0;
	/*if (strncmp(passwd, "123456", 6) == 0)//如果密码等于"123456"验证通过
		return 1;
	else
		return 0;//密码验证失败*/
}
void work::fix_socket_client(int index, int st)
{
	if (socket_client[index] != 0) //同一个userid没有下线，却又在另一个终端登录，拒绝登录
	{
		printf("%d:userid already login\n", index);
		struct msg_t msg;
		memset(&msg, 0, sizeof(msg));
		msg.head[0] = 2; //系统消息
		msg.head[1] = 3; //userid已经登录
		msg.head[2] = 0; //暂时保留，填0
		msg.head[3] = 0; //暂时保留，填0
		send(st, (char *) &msg, sizeof(msg.head), 0);//给client端socket下发系统消息
		close(st);
	} else
	{
		//如果socket_client[index] 等于0，将client端socket赋予socket_client[index]
		socket_client[index] = st;
	}
}

void work::broadcast_user_status() //向socket_client[]数组中所有client广播用户状态消息
{
	struct msg_t msg;
	memset(&msg, 0, sizeof(msg));
	msg.head[0] = 1; //设置消息类型为用户状态消息

	for (int i = 0; i < CLIENTCOUNT; i++) //设置用户状态消息体
	{
		if (socket_client[i] != 0)
		{
			msg.body[i] = '1'; //设置相应userid状态为在线
		} else
		{
			msg.body[i] = '0'; //设置相应userid状态为离线
		}
	}

	for (int i = 0; i < CLIENTCOUNT; i++) //向socket_client数组中每个client广播用户状态消息
	{
		if (socket_client[i] != 0)
		{
			send(socket_client[i], &msg, strlen(msg.body) + sizeof(msg.head),
					0);
			printf("%d:broadcast, %s\n", i, msg.body);
		}
	}
}

void work::sql_init()
{
	strcpy(g_dbName,"chat");
	strcpy(g_strIP,"127.0.0.1");
	strcpy(g_strUser,"root");
	strcpy(g_strPass,"123456");
	strcpy(g_tableName,"");

	// 进行数据库连接，如果连接失败，则选择系统退出
	m_pMyData=mysql_init(NULL);
	printf("正在进行数据库连接................");
	if(mysql_real_connect(m_pMyData,g_strIP,g_strUser,g_strPass,g_dbName,g_nPort,NULL,0))
	{
		printf("[OK]\n正在查找指定的数据库................");
		if ( mysql_select_db( m_pMyData, g_dbName) < 0 ) //选择制定的数据库失败
		{
			mysql_close(m_pMyData ) ;//初始化mysql结构失败
			printf("[FAILED]\n数据库查找失败，应用程序退出........\n");
			exit(0);

		}
		else
		{
			printf("[OK]\n已连到上数据库...\n");
		}
	}
	else
    {
		mysql_close( m_pMyData ) ;//初始化mysql结构失败
		printf("[FAILED]\n数据库连接失败，应用程序退出........\n");
		exit(0);
    }

}

int work::checkuseridandpwd(const char *strSQL)
{
	MYSQL_RES * res ;//保存查询结果
	int num_row = 0;//得到记录数量
	if(!mysql_real_query( m_pMyData, strSQL,strlen(strSQL)))
	{
		//printf("执行成功，影响%d行\n",mysql_affected_rows(m_pMyData));//得到受影响的行数
		res  = mysql_store_result( m_pMyData ) ;//保存查询结果
		num_row = (int) mysql_num_rows( res ) ; //得到记录数量
		printf("查询结果%d",num_row);
		if(num_row>0)
		{
			mysql_free_result(res);
			return 1;
		}
		else
		{
			mysql_free_result(res);
			return 0;
		}
	}
	else
	{
		printf("下载用户信息失败\n");
		return 0;
	}
}
int work::GetCurTime(char *strTime,int type)
{
    time_t t;
	struct tm *tm = NULL;
    t = time(NULL);
    if(t == -1)
	{
        return -1;
    }
    tm = localtime(&t);
    if(tm == NULL)
	{
        return -1;
    }
	if(type == 0)
		sprintf(strTime,"系统运行时间为: %d-%d-%d %d:%d:%d\n",tm->tm_year + 1900,tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	else if(type == 1)
		sprintf(strTime,"%d-%d-%d %d:%d:%d",tm->tm_year + 1900,tm->tm_mon + 1,tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	else if(type == 2)
		sprintf(strTime,"%d_%d_%d",tm->tm_year + 1900,tm->tm_mon + 1,tm->tm_mday);

    return 0;
}
