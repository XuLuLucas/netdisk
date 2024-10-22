#include "mythreadworker.h"
#include <QTimer>

myThreadWorker::myThreadWorker(qintptr handle, QObject *parent)
{
    m_handle = handle;
}

void myThreadWorker::run()
{

    //创建套接字并连接
    m_socket = new MyTcpSocket;
    m_socket->setSocketDescriptor(m_handle);

    connect(m_socket, &MyTcpSocket::workOver, this, &myThreadWorker::stop);

    // 需要周期性触发的操作
    QTimer *timer = new QTimer();    
    connect(timer, &QTimer::timeout, m_socket, &MyTcpSocket::test);
    timer->start(1000);

    exec();

}

void myThreadWorker::stop(){
    m_socket->deleteLater(); // 安全地删除
    quit();
}

