#ifndef TOOLBOX1_H
#define TOOLBOX1_H

#include <QToolBox>
#include <QToolButton>
#include <QEvent>
#include "widget.h"
#include <QTcpSocket>
#include <QCloseEvent>
#define CLIENTCOUNNT 256    //设置最大客户端数目
#define MSGBODYBUF 1024
//定义消息结构
struct msg_t
{
    unsigned char head[4];
    char body[MSGBODYBUF];
};

class toolbox1 : public QToolBox
{
    Q_OBJECT
public:
    explicit toolbox1(QWidget *parent = 0);  
    void send_Msg(int d_userid, const char *msgBody);
    void closeEvent(QCloseEvent *event);
signals:
    
public slots:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QString username[CLIENTCOUNNT];
    QToolButton *toolBtn[CLIENTCOUNNT]; //创建256个用户窗口
    Widget *child[CLIENTCOUNNT];
    QTcpSocket *sockClient;


    int userid;
    QString passwd;
    QString hostip;
    int hostport;

    void init_toolBtn();
    void init_username();
    void login_Msg();
    void system_Msg(int msgcode);//来自server的系统消息
    void userStatus_Msg(const char *msgBody);//来自server的deliver消息
    void recv_send_Msg(int o_userid, const char *msgBody);//来自server的send消息

private slots:
    void sock_Error(QAbstractSocket::SocketError sockErr);//socket出错触发
    void read_Msg();//收到信息触发
    void socket_connected();//连接成功后触发
};

#endif // TOOLBOX1_H
