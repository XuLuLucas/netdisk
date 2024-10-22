#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT //宏，支持信号与槽
public:
    MyTcpServer();

    static MyTcpServer &getInstance();

    void incomingConnection(qintptr handle);//父类的虚函数，每次有新的连接时就会自动调用

};

#endif // MYTCPSERVER_H
