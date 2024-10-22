#include "mytcpserver.h"
#include <QDebug>
#include <QThread>
#include "mythreadworker.h"
#include <QSharedMemory>  //共享内存
#include <QSystemSemaphore>

MyTcpServer::MyTcpServer()
{
    //创建共享内存，储存fut
    QSharedMemory sharedMemory("FUTlist");
    if (!sharedMemory.create(1000)) {
        qDebug() << "Failed to create shared memory:" << sharedMemory.errorString();
    }
    QSystemSemaphore semaphore("FUTlist", 1);
}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr handle)
{

    qDebug() << "new client connected";

    myThreadWorker *worker = new myThreadWorker(handle);
    connect(worker, &myThreadWorker::finished, worker, &QObject::deleteLater);
    worker->start();

}




