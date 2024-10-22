#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget> //用户界面对象的基类
#include <QFile>
#include <QTcpSocket> //用于tcp网络通信的类
#include <opeWidget.h>

//用于命名空间，帮助组织代码并防止命名冲突
QT_BEGIN_NAMESPACE
namespace Ui {
class TCPclient;
}
QT_END_NAMESPACE

class TCPclient : public QWidget//继承代表了它是一个窗口部件
{
    Q_OBJECT  //宏，使类能够使用 Qt 的信号和槽机制
private:
    Ui::TCPclient *ui;
    QString m_strIP;
    quint16 m_usPort;
    QTcpSocket m_tcpSocket;
    QString m_strLoginName;//用户名
    QString m_strCurPath;//当前路径

    void loadConfig();//加载配置文件
    TCPclient(QWidget *parent = nullptr);

public:
    ~TCPclient();
    static TCPclient& getInstance();
    QTcpSocket& getTcpSocket();//返回了socket成员，这样其它类可以利用它进行消息传递
    QString& getLoginName();

    void sendPduWithoutData(ENUM_MSG_TYPE msgType, const char* msgDsc1 = NULL, const char* msgDsc2 = NULL);

public slots:
    void on_login_pd_clicked();//登陆键
    void on_register_pd_clicked();//注册键
    void on_logout_pd_clicked();//注销键

    void recvMsg();
};
#endif // TCPCLIENT_H
