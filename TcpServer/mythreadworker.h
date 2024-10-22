#ifndef MYTHREADWORKER_H
#define MYTHREADWORKER_H

#include <QObject>
#include <QThread>
#include "mytcpsocket.h"


class myThreadWorker : public QThread
{
    Q_OBJECT
public:
    explicit myThreadWorker(qintptr handle, QObject *parent = nullptr);

    MyTcpSocket* getMySocket(){
        return m_socket;
    }

    void run();


private:
    //可以试一下是否可以在构造函数中创建，如果可以的话就没有必要保存
    qintptr m_handle;
    MyTcpSocket *m_socket;

public slots:
    void stop();


signals:




};

#endif // MYTHREADWORKER_H
