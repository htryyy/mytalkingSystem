/*
 * mymysql.cpp
 *
 *  Created on: 2015年4月30日
 *      Author: train
 */

#include "mymysql.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
mymysql::mymysql() {
	// TODO Auto-generated constructor stub
	sql_init();
}

mymysql::~mymysql() {
	// TODO Auto-generated destructor stub
}

void mymysql::sql_init()
{
	strcpy(g_dbName,"chat");
	strcpy(g_strIP,"127.0.0.1");
	strcpy(g_strUser,"root");
	strcpy(g_strPass,"123456");
	strcpy(g_tableName,"");

	// 进行数据库连接，如果连接失败，则选择系统退出
	m_pMyData = mysql_init(NULL);
	printf("正在进行数据库连接................");
	if(mysql_real_connect(m_pMyData,g_strIP,g_strUser,g_strPass,g_dbName,g_nPort,NULL,0))
	{
		printf("[OK]\n正在查找指定的数据库................");
		if ( mysql_select_db( m_pMyData, g_dbName) < 0 ) //选择制定的数据库失败
		{
			mysql_close( m_pMyData ) ;//初始化mysql结构失败
			printf("[FAILED]\n数据库查找失败，应用程序退出........\n");
			//DBInfor();
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
		//DBInfor();
		exit(0);
    }
	// 用于
	//InitUserList();
}
void mymysql::checkuseridandpwd(const char *strSQL)
{
	MYSQL_RES * res ;//保存查询结果
	MYSQL_FIELD * fd ;//保存字段结果
	MYSQL_ROW row ;
	int num_row = 0;//得到记录数量
	////int num_col = 0;//得到字段数量
	//const char *strSQL="select id,name,password from user_infor";
	if(!mysql_real_query( m_pMyData, strSQL,strlen(strSQL)))
	{
	//	printf("执行成功，影响%d行\n",mysql_affected_rows(m_pMyData));//得到受影响的行数
		res  = mysql_store_result( m_pMyData ) ;//保存查询结果
		//num_row = (int) mysql_num_rows( res ) ; //得到记录数量
		//num_col = mysql_num_fields( res ) ;//得到字段数量
		printf("查询结果%s"，res);
		/*for (int f1 = 0; f1 < num_row; f1++)
		{
			row = mysql_fetch_row(res);
			UserNode node;
			node.id = atoi(row[0]);
			strcpy(node.strName,row[1]);
			strcpy(node.password,row[2]);
			//g_pDisLineUserMap[node.id] = node;
		}*/
		mysql_free_result(res);
	}
	else
	{
		printf("下载用户信息失败\n");
	}
}
