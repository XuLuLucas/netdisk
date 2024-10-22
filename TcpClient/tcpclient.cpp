#include "tcpclient.h"
#include "ui_tcpclient.h"
#include <QMessageBox>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <QHostAddress>

#include "protocol.h"
#include <QTimer>


//构造函数，在这里执行初始化操作
TCPclient::TCPclient(QWidget *parent): QWidget(parent), ui(new Ui::TCPclient)
{
    ui->setupUi(this);


    loadConfig();//加载配置文件

    connect(&m_tcpSocket,&QTcpSocket::connected,
            this,[this]() {QMessageBox::warning(this, "连接服务器", "连接服务器成功");});

    connect(&m_tcpSocket,&QTcpSocket::disconnected,
            this,[this]() {QMessageBox::warning(this, "连接服务器", "断开连接");});

    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    //连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);

}

TCPclient::~TCPclient()
{
    delete ui;
}

void TCPclient::sendPduWithoutData(ENUM_MSG_TYPE msgType, const char *msgDsc1,const char *msgDsc2)
{
    PDU *pdu = mkPDU(0);
    pdu->enumMsgType = msgType;//消息类型
    if(msgDsc1)strcpy(pdu->caMsgDsc,msgDsc1);
    if(msgDsc2)strcpy(pdu->caMsgDsc+32,msgDsc2);//消息描述
    m_tcpSocket.write((char*)pdu, pdu->uiPDULen);//发送消息
    free(pdu);
    pdu = NULL;
}

void TCPclient::loadConfig()
{
    QFile file(":/client.config");
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

TCPclient &TCPclient::getInstance()
{
    static TCPclient instance;
    return instance;
}

QTcpSocket &TCPclient::getTcpSocket()
{
    return m_tcpSocket;
}

QString &TCPclient::getLoginName()
{
    return m_strLoginName;
}

void TCPclient::on_login_pd_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(strName.isEmpty() || strPwd.isEmpty()){
        QMessageBox::warning(this, "提示","用户名或密码不能为空");
    }
    else{
        PDU *pdu = mkPDU(0);
        pdu->enumMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;//消息类型
        memcpy(pdu->caMsgDsc,strName.toStdString().c_str(),strName.toUtf8().size());
        memcpy(pdu->caMsgDsc+32,strPwd.toStdString().c_str(),strPwd.toUtf8().size());
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);//发送消息
        free(pdu);
        pdu = NULL;
    }
}

void TCPclient::on_register_pd_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(strName.isEmpty() || strPwd.isEmpty()){
        QMessageBox::warning(this, "注册失败","用户名或密码不能为空");
    }
    else{
        PDU *pdu = mkPDU(64);//用户名和密码各32位
        pdu->enumMsgType = ENUM_MSG_TYPE_REGISTER_REQUEST;//消息类型
        memcpy(pdu->caMsg,strName.toStdString().c_str(),strName.toUtf8().size());
        memcpy((char*)pdu->caMsg+32,strPwd.toStdString().c_str(),strPwd.toUtf8().size());
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);//发送消息
        free(pdu);
        pdu = NULL;
    }
}

void TCPclient::on_logout_pd_clicked()
{

}

void TCPclient::recvMsg()
{
    //读取数据
    unsigned int uiPDULen = 0;
    m_tcpSocket.read((char*)&uiPDULen, sizeof(unsigned int));
    unsigned int uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *recvPdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char*)recvPdu+sizeof(unsigned int),uiPDULen - sizeof(unsigned int));
    qDebug()<<"收到消息"<<recvPdu->enumMsgType;
    //根据数据类型进行相应操作
    switch (recvPdu->enumMsgType) {
    case ENUM_MSG_TYPE_REGISTER_RESPOND://注册回复
    {
        if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_REGISTER_SUCCESSED)==0){
            QMessageBox::information(this, "注册","注册成功");
        }
        else if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_REGISTER_FAILED)==0){
            QMessageBox::warning(this, "注册","注册失败，用户名已存在");
        }
        free(recvPdu);
        recvPdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_LOGIN_RESPOND://登陆回复
    {
        if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_LOGIN_SUCCESSED)==0){
            QString strName = ui->name_le->text();
            m_strLoginName = strName;
            QMessageBox::information(this, "登陆","登陆成功");
            opeWidget::getInstance().show();
            this->hide();
            // 初始化文件系统并请求目录信息
            opeWidget::getInstance().m_pBook->initFileSystem();
        }
        else if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_LOGIN_FAILED)==0){
            QMessageBox::warning(this, "登陆失败","用户名或密码错误");
        }
        else if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_LOGIN_REFUSED)==0){
            QMessageBox::warning(this, "登陆失败","你已经在其它地方登陆");
        }
        free(recvPdu);
        recvPdu = NULL;
        break;
    }

    // 未开始
    case ENUM_MSG_TYPE_LOGOFF_RESPOND://注销回复
    {
        break;
    }

    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND://查找用户回复
    {
        if (strcmp(RESPOND_TO_SEARCH_USR_ONLINE, recvPdu->caMsgDsc) == 0 ||
            strcmp(RESPOND_TO_SEARCH_USR_OFFLINE, recvPdu->caMsgDsc) == 0) {

            // 创建自定义对话框
            QDialog *dialog = new QDialog(this);
            dialog->setWindowTitle("查询结果");
            dialog->setFixedSize(300, 150);

            QVBoxLayout *layout = new QVBoxLayout(dialog);

            // 显示用户状态
            QString pername = recvPdu->caMsgDsc+32;
            QString status = (strcmp(RESPOND_TO_SEARCH_USR_ONLINE, recvPdu->caMsgDsc) == 0) ? "online" : "offline";
            layout->addWidget(new QLabel(pername+"用户状态: " + status, dialog));

            // 加好友和返回按钮
            QPushButton *addFriendButton = new QPushButton("加好友", dialog);
            QPushButton *returnButton = new QPushButton("返回", dialog);

            layout->addWidget(addFriendButton);
            layout->addWidget(returnButton);

            // 连接信号与槽
            connect(addFriendButton, &QPushButton::clicked, [&]() {
                dialog->accept();  // 关闭对话框
                //发送加好友的请求
                QString strLoginUsrName = TCPclient::getInstance().getLoginName();
                sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,
                                   pername.toStdString().c_str(),
                                   strLoginUsrName.toStdString().c_str());
            });

            connect(returnButton, &QPushButton::clicked, dialog, &QDialog::reject); // 关闭对话框

            dialog->exec(); // 显示对话框
        }
        else if (strcmp(RESPOND_TO_SEARCH_USR_ABSENT, recvPdu->caMsgDsc) == 0) {
            QMessageBox::information(this, "查询结果", "用户不存在");
        }
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{//收到好友请求
        QString pername = recvPdu->caMsgDsc;
        int ret = QMessageBox::information(NULL, "好友请求",pername,QMessageBox::Yes,QMessageBox::No);

        if(ret == QMessageBox::Yes){//只有yes代表同意，no或者直接关闭对话都代表不同意
            sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_AGREED,
                               pername.toStdString().c_str(),
                               m_strLoginName.toStdString().c_str()
                               );
        }
        else{
            sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_REFUSED,
                               pername.toStdString().c_str(),
                               m_strLoginName.toStdString().c_str()
                               );
        }
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{//服务器反馈
        if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_ADDFRIEND_FAILED) == 0){
            QMessageBox::information(this, "添加好友","对方已经是您的好友了");
        }
        else if(strcmp(recvPdu->caMsgDsc, RESPOND_TO_ADDFRIEND_SENDED) == 0){
            QMessageBox::information(this, "添加好友","请求已发送");
        }
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_AGREED:{//得知对方同意添加好友
        QMessageBox::information(this, "对方同意添加你为好友",recvPdu->caMsgDsc);
        opeWidget::getInstance().m_pFriend->flushFriend();//刷新好友列表
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSED:{//得知对方拒绝添加好友
        QMessageBox::information(this, "对方拒绝添加你为好友",recvPdu->caMsgDsc);
        break;
    }

    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{ // 收到刷新好友操作的反馈
        opeWidget::getInstance().m_pFriend->showFriend(recvPdu); break;
    }

    case ENUM_MSG_TYPE_DEL_FRIEND_RESPOND:{
        QMessageBox::information(this, "删除好友","删除成功");
        opeWidget::getInstance().m_pFriend->flushFriend();//刷新好友列表
        break;
    }

    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{
        QMessageBox::information(this, "你已被删除",recvPdu->caMsgDsc+32);
        opeWidget::getInstance().m_pFriend->flushFriend();//刷新好友列表
        break;
    }

    case ENUM_MSG_TYPE_PRIVATE_DATA:{
        opeWidget::getInstance().m_pFriend->receivedMessage(recvPdu);
        break;
    }

    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{//创建文件夹结果反馈
        QMessageBox::information(opeWidget::getInstance().m_pFriend, "提示",recvPdu->caMsgDsc);
        opeWidget::getInstance().m_pBook->flushList();
        break;
    }

    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:{//删除文件结果反馈
        QMessageBox::information(opeWidget::getInstance().m_pFriend, "提示",recvPdu->caMsgDsc);
        opeWidget::getInstance().m_pBook->flushList();
        break;
    }

    case ENUM_MSG_TYPE_RENAME_DIR_RESPOND:{
        QMessageBox::information(opeWidget::getInstance().m_pFriend, "提示",recvPdu->caMsgDsc);
        opeWidget::getInstance().m_pBook->flushList();
        break;
    }

    case ENUM_MSG_TYPE_CATALOG_DATA:{
        opeWidget::getInstance().m_pBook->handleCatalogPDU(recvPdu);
        break;
    }

    case ENUM_MSG_TYPE_UPLOAD_FILE_SUC:{
        opeWidget::getInstance().m_pBook->handleUploadSuc(recvPdu);
        break;
    }

    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        QString specialMessage = (char*)recvPdu->caMsg;
        int index = 0;
        while(specialMessage[index]!='?')index++;
        QString fut = specialMessage.left(index);;
        QString fileName = specialMessage.mid(index + 1);;
        QString sender = (char*)recvPdu->caMsgDsc;
        opeWidget::getInstance().m_pFriend->getshareFile(fut,fileName,sender);
        break;
    }
    default: {
        QMessageBox::critical(this, "警告","收到未经定义的消息");
        break;
    }
    };
}


