#include "tcpserver.h"
#include "ui_tcpserver.h"

#include "mytcpserver.h" //自己定义的库

#include <QMessageBox>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QHostAddress>
#include <QFile>

Tcpserver::Tcpserver(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Tcpserver)
{
    ui->setupUi(this);

    loadConfig();

    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

Tcpserver::~Tcpserver()
{
    delete ui;
}

void Tcpserver::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        //qDebug() << strData;//用于调试
        file.close();//读取文件，结束后关闭文件

        QStringList strList = strData.split("\r\n");

        m_strIP = strList.at(0);
        m_usPort = strList.at(1).toUShort();


    }
    else{
        //如果读取失败

    }
}
