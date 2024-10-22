#include "mytcpsocket.h"
#include <QStringList>
#include <QDebug>
#include "mytcpserver.h"
#include "myqdir.h"
#include <QThread>
#include <QSharedMemory>
#include <QTimer>
#include <QSystemSemaphore>
#include "fileSystem.h"
#include "mytools.h"

//定义全局变量
QStringList MyTcpSocket::FUTlist;
QMutex MyTcpSocket::mutex;
QVector<shareContent> MyTcpSocket::shareContentList;
QMutex MyTcpSocket::shareContentListLock;  // 互斥锁

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    //收到信息就处理信息
    connect(this, SIGNAL(readyRead()),
            this, SLOT(recvMsg()));
    //客户端断开时的处理，要对数据库进行修改后才告知thread
    connect(this, SIGNAL(disconnected()),
            this, SLOT(clientOffline()));

    //初始化数据库连接
    Qt::HANDLE threadId = QThread::currentThreadId();
    m_pDBop = new DBop(QString::number(reinterpret_cast<quintptr>(threadId)));
    m_pDBop->init();

}



void MyTcpSocket::recvMsg()
{
while (bytesAvailable() > 0){
    if(m_bUpload){

        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();

        qDebug() << QString("第 %1 次传入文件,接受数据大小:%2").arg(m_iCount).arg(buff.size());
        m_iCount++;

        if(m_iTotal == m_iRecved)
        {
            m_file.close();
            m_pDBop->flieUpSuc(FUT);//更新数据库信息
            //读取文件内容完成，将fut写进共享内存
            shareFUT();
        }

        return;
    }

    //读取数据
    unsigned int uiPDULen = 0;
    this->read((char*)&uiPDULen, sizeof(unsigned int));
    unsigned int uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *recvpdu = mkPDU(uiMsgLen);
    this->read((char*)recvpdu+sizeof(unsigned int),uiPDULen - sizeof(unsigned int));
    qDebug()<<"收到消息:"<<recvpdu->enumMsgType;
    //根据数据类型进行相应操作
    switch (recvpdu->enumMsgType) {

    case ENUM_MSG_TYPE_REGISTER_REQUEST://注册请求
    {

        char caName[32], caPwd[32];
        strncpy(caName,(char*)recvpdu->caMsg,32);
        strncpy(caPwd,(char*)recvpdu->caMsg+32,32);
        bool ret = m_pDBop->handleRegister(caName, caPwd);
        PDU *respdu = mkPDU(0);
        respdu->enumMsgType = ENUM_MSG_TYPE_REGISTER_RESPOND;

        if(ret){
            //返回注册成功的信息
            strcpy(respdu->caMsgDsc,RESPOND_TO_REGISTER_SUCCESSED);
            //创建json文件
            Directory dir;
            dir.saveToJson(QString("catalogs/%1.json").arg(caName));
        }
        else {
            strcpy(respdu->caMsgDsc,RESPOND_TO_REGISTER_FAILED);
        }

        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_LOGIN_REQUEST://登陆请求
    {
        char caName[32], caPwd[32];
        strncpy(caName,(char*)recvpdu->caMsgDsc,32);
        strncpy(caPwd,(char*)recvpdu->caMsgDsc+32,32);
        bool state = false;//记录登陆失败的时候是否是已经登陆过导致的
        bool ret = m_pDBop->handleLogin(caName, caPwd, state);

        PDU *respdu = mkPDU(0);
        respdu->enumMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;

        if(!ret){//发生其它错误
            strcpy(respdu->caMsgDsc,RESPOND_TO_LOGIN_FAILED);
        }
        else if(state){//已经登陆过了
            strcpy(respdu->caMsgDsc,RESPOND_TO_LOGIN_REFUSED);
        }
        else {//登陆成功
            strcpy(respdu->caMsgDsc,RESPOND_TO_LOGIN_SUCCESSED);
            m_strName = caName;
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_LOGOFF_REQUEST://注销请求
    {
        break;
    }

    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:{//查找用户请求
        int ret = m_pDBop->handleSerchUsr(recvpdu->caMsgDsc);
        PDU *respdu = mkPDU(0);
        respdu->enumMsgType = ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(ret == 1)
        {
            strcpy(respdu->caMsgDsc,RESPOND_TO_SEARCH_USR_ONLINE);
            strcpy(respdu->caMsgDsc+32,recvpdu->caMsgDsc);
        }
        else if(ret == 0){
            strcpy(respdu->caMsgDsc,RESPOND_TO_SEARCH_USR_OFFLINE);
            strcpy(respdu->caMsgDsc+32,recvpdu->caMsgDsc);
        }
        else if(ret == -1){
            strcpy(respdu->caMsgDsc,RESPOND_TO_SEARCH_USR_ABSENT);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{//加好友请求
        //取信息
        char perName[32], usrName[32];
        strncpy(perName,(char*)recvpdu->caMsgDsc,32);
        strncpy(usrName,(char*)recvpdu->caMsgDsc+32,32);
        //查表
        int ret = m_pDBop->handleCheckdFriend(perName, usrName);
        //返回信息
        PDU *respdu = mkPDU(0);
        respdu->enumMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        if(ret == 0){//对方已经是好友了
            strcpy(respdu->caMsgDsc,RESPOND_TO_ADDFRIEND_FAILED);
        }
        else{//对方还不是好友
            strcpy(respdu->caMsgDsc,RESPOND_TO_ADDFRIEND_SENDED);
            //将消息加到共享信息里面
            shareContent tmp;
            tmp.type = 2;//加好友
            tmp.receiver = perName;
            tmp.sender = usrName;
            tmp.sendTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
            tmp.message = "";//无消息，为空
            QMutexLocker locker(&shareContentListLock);
            shareContentList.append(tmp);
        }

        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_AGREED:{//同意添加好友
        char requester[32], responder[32];
        strncpy(requester,(char*)recvpdu->caMsgDsc,32);
        strncpy(responder,(char*)recvpdu->caMsgDsc+32,32);
        bool ret = m_pDBop->handleAddFriend(requester, responder);
        if(ret){
            shareContent tmp;
            tmp.type = 3;//添加好友被同意
            tmp.receiver = requester;
            tmp.sender = responder;
            tmp.sendTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
            tmp.message = "";//无消息，为空
            QMutexLocker locker(&shareContentListLock);
            shareContentList.append(tmp);
            break;
        }
        break;
    }

    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSED:{//拒绝添加好友
        char requester[32], responder[32];
        strncpy(requester,(char*)recvpdu->caMsgDsc,32);
        strncpy(responder,(char*)recvpdu->caMsgDsc+32,32);

        shareContent tmp;
        tmp.type = 4;//加好友被拒绝
        tmp.receiver = requester;
        tmp.sender = responder;
        tmp.sendTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
        tmp.message = "";//无消息，为空
        QMutexLocker locker(&shareContentListLock);
        shareContentList.append(tmp);

        break;
    }

    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{//刷新好友列表
        QStringList ret;
        ret = m_pDBop->handleSerchFriend(get_strName().toStdString().c_str());
        unsigned int uiMsgLen = ret.size()*32;
        PDU *respdu = mkPDU(uiMsgLen);
        respdu->enumMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
        for(int i = 0;i<ret.size();i++){
            memcpy((char*)(respdu->caMsg)+i*32,
                   ret[i].toStdString().c_str(),
                   ret[i].size());
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }

    //未完善，应该让对方知道自己被删除了
    case ENUM_MSG_TYPE_DEL_FRIEND_REQUEST:{//删除好友请求
        bool ret =m_pDBop->handleDelFriend(recvpdu->caMsgDsc,m_strName.toStdString().c_str());
        if(ret){
            sendPduWithoutData(ENUM_MSG_TYPE_DEL_FRIEND_RESPOND);
        }
        break;
    }

    case ENUM_MSG_TYPE_PRIVATE_DATA:{//聊天信息

        shareContent tmp;
        tmp.type = 1;
        tmp.receiver = recvpdu->caMsgDsc;
        tmp.sender = recvpdu->caMsgDsc+32;
        tmp.sendTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
        tmp.message = QString::fromLocal8Bit((char*)recvpdu->caMsg);
        QMutexLocker locker(&shareContentListLock);
        shareContentList.append(tmp);
        break;
    }

    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:{//删除文件请求
        int length = recvpdu->uiMsgLen/32;
        QString fut_tmp;
        for(int i = 0;i<length;i++){
            fut_tmp = (char*)recvpdu->caMsg+32*i;
            bool del = false;
            m_pDBop->deShareWithDelete(fut_tmp, del);
            if(del){
                QFile file("usrData/"+fut_tmp);
                file.remove();
            }
        }
        break;
    }

    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:{//上传文件请求
        //获取FUT和文件长度
        char fut_tmp[32] = {'\0'};
        qint64 fileSize = 0;
        sscanf(recvpdu->caMsgDsc, "%s %lld", fut_tmp, &fileSize);
        QString localPath = "usrData/" + QString(fut_tmp);
        FUT = fut_tmp;
        m_file.setFileName(localPath);
        // 以只写的方式打开文件，如果文件不存在，则自动创建文件
        if(m_file.open(QIODevice::WriteOnly))
        {
            m_bUpload = true;
            m_iTotal = fileSize;
            m_iRecved = 0;
            m_iCount = 1;
            //更新文件信息表
            m_pDBop->flieCreate(FUT);
            //发送回应
            sendPduWithoutData(ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND);
        }
        else qDebug()<<"failed to creat or open file";
        break;

    }

    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{

        FUT = recvpdu->caMsgDsc;
        QString localPath = "usrData/" + FUT;
        m_file.setFileName(localPath);
        if (!m_file.open(QIODevice::ReadOnly)) {
            break;//如果无法打开文件，不回复，客户端会自动关闭
        }
        m_iTotal = m_file.size();//文件的大小
        PDU *pdu = mkPDU(0);
        pdu->enumMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;//消息类型
        sprintf(pdu->caMsgDsc, "%lld", m_iTotal);
        //读取方式：sscanf(pdu->caData, "%s %lld", fileName, &fileSize);
        write((char*)pdu, pdu->uiPDULen);//发送消息
        free(pdu);
        pdu = NULL;
        break;
    }

    case ENUM_MSG_TYPE_DOWNLOAD_FILE_START:{
        m_bDownload = true;
        downloadTimer = new QTimer();
        connect(downloadTimer, &QTimer::timeout, this, [=]() {
            sendNextChunk();
        });
        //读取数据
        m_fileData = m_file.readAll();
        m_file.close();
        downloadTimer->start(10);
        break;
    }

    case ENUM_MSG_TYPE_CATALOG_REQUEST:{
        sendCatalog();
        break;
    }

    case ENUM_MSG_TYPE_CATALOG_DATA:{
        haneleCatalogPDU(recvpdu);
        break;
    }

    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
        shareContent tmp;
        QString tmp_fut = (char*)recvpdu->caMsg;
        QString tmp_name = (char*)recvpdu->caMsg+32;
        QString tmp_message = tmp_fut+'?'+tmp_name;
        tmp.type = 10;
        tmp.receiver = recvpdu->caMsgDsc;
        tmp.sender = recvpdu->caMsgDsc+32;
        tmp.sendTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
        tmp.message = tmp_message;
        QMutexLocker locker(&shareContentListLock);
        shareContentList.append(tmp);
        break;
    }

    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
        QString fut2add = recvpdu->caMsgDsc;
        qDebug()<<"here"<<fut2add;
        m_pDBop->addShare(fut2add);
        break;
    }
    default:
        qDebug() << "收到未知类型消息";
        break;

    };

}
}

//客户端下线之后的操作
void MyTcpSocket::clientOffline()
{
    if(m_bUpload){
        //传输文件客户端断连了应该怎么操作，写在这里；
        if(m_iTotal == m_iRecved){
        }
        else{
            //没有传输完成，则将文件从库中删除，也从本地删除
            m_pDBop->flieUpFaile(FUT);
            m_file.remove();
        }
    }
    else if(m_bDownload){
        //暂时不做处理
    }

    else {
        //两步，第一步修改数据库中的在线状态，第二部通过信号函数告知服务器，将socket从列表中删除
        m_pDBop->handleClientOffline(m_strName.toStdString().c_str());
        m_pDBop->close();
    }
    emit workOver();
}

//定时操作
void MyTcpSocket::test()//
{
    //————————上传模式的操作——————————
    if(m_bUpload){
        return;
    }
    //————————服务模式的操作——————————
    checkFUT();
    checkShareContent();
    //——————————————————————————————
}

void MyTcpSocket::checkShareContent()
{
    //找到给当前用户的信息
    QMutexLocker locker(&shareContentListLock);
    QList<shareContent> myMessages;
    for (auto message : shareContentList) {
        if (message.receiver == m_strName) {
            myMessages.append(message);
        }
    }
    for (const auto& message : myMessages) {
        shareContentList.removeOne(message);
        qDebug()<<message.message<<"here";
    }
    locker.unlock();

    // 处理信息
    for (auto mymessage : myMessages) {
        switch (mymessage.type) {
        //聊天消息
        case 1:
        {
            QByteArray localMessage = mymessage.message.toLocal8Bit(); // 转换为本地编码的字节数组
            PDU *pdu = mkPDU(localMessage.size()+1);
            pdu->enumMsgType = ENUM_MSG_TYPE_PRIVATE_DATA;
            memcpy(pdu->caMsgDsc, mymessage.sender.toStdString().c_str(), 32);//有待修改
            memcpy(pdu->caMsgDsc+32, mymessage.sendTime.toStdString().c_str(), 32);
            memcpy(pdu->caMsg, localMessage.constData(), localMessage.size() + 1); // +1 用于 '\0'
            write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
            break;
        }
        //好友请求
        case 2:{
            sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,
                               mymessage.sender.toStdString().c_str());
            break;
        }
        // 加好友被同意
        case 3:{
            sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_AGREED,
                               mymessage.sender.toStdString().c_str());
            break;
        }
        // 加好友被拒绝
        case 4:{
            sendPduWithoutData(ENUM_MSG_TYPE_ADD_FRIEND_REFUSED,
                               mymessage.sender.toStdString().c_str());
            break;
        }
        case 10:{//分享文件
            PDU *pdu = mkPDU(64);
            pdu->enumMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
            memcpy(pdu->caMsgDsc, mymessage.sender.toStdString().c_str(), 32);//有待修改
            memcpy(pdu->caMsgDsc+32, mymessage.sendTime.toStdString().c_str(), 32);
            memcpy(pdu->caMsg, mymessage.message.toStdString().c_str(), mymessage.message.toStdString().size()+1); // +1 用于 '\0'
            write((char*)pdu, pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
            break;
        }
        default:
            break;
        }

    }



}

void MyTcpSocket::checkFUT()
{
    QStringList tmp = FindAndDelFUT(m_strName);
    for(const auto &x : tmp){
        //通知客户端上传成功
        sendPduWithoutData(ENUM_MSG_TYPE_UPLOAD_FILE_SUC,x.toStdString().c_str());
        //修改数据库状态
        m_pDBop->addShare(x);
    }
}

//————————————工具函数——————————————————————————
void MyTcpSocket::sendPduWithoutData(ENUM_MSG_TYPE msgType, const char *msgDsc1, const char *msgDsc2)
{
    PDU *respdu = mkPDU(0);
    respdu->enumMsgType = msgType;
    if(msgDsc1)strcpy(respdu->caMsgDsc, msgDsc1);
    if(msgDsc2)strcpy(respdu->caMsgDsc+32, msgDsc2);
    write((char*)respdu, respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void MyTcpSocket::shareFUT()
{
    QMutexLocker locker(&mutex);
    FUTlist.append(FUT);
}

QStringList MyTcpSocket::FindAndDelFUT(const QString &username) {
    QMutexLocker locker(&mutex);

    QStringList foundItems;
    QStringList itemsToRemove;

    // 查找包含str的元素

    // 构造要查找的匹配字符串
    QString searchPattern = username + "@";

    // 查找包含该用户名的元素
    for (const QString &item : FUTlist) {
        if (item.startsWith(searchPattern)) {
            foundItems.append(item);
            itemsToRemove.append(item);
        }
    }

    // 从FUTlist中删除找到的元素
    for (const QString &item : itemsToRemove) {
        FUTlist.removeAll(item);
    }

    return foundItems;
}

void MyTcpSocket::sendCatalog()
{
    QString path = QString("catalogs/%1.json").arg(m_strName);
    QByteArray byteArray = mytoolReadFileToByteArray(path);

    PDU *pdu = mkPDU(byteArray.size()+1);
    pdu->enumMsgType = ENUM_MSG_TYPE_CATALOG_DATA;
    memcpy(pdu->caMsg, byteArray,byteArray.size());
    write((char*)pdu, pdu->uiPDULen);
    flush();
    free(pdu);
    pdu = NULL;
}

void MyTcpSocket::haneleCatalogPDU(PDU *pdu)
{
    QString path = QString("catalogs/%1.json").arg(m_strName);
    QByteArray byteArray = (char*)pdu->caMsg;
    mytoolSaveByteArrayToFile(path,byteArray);
}

void MyTcpSocket::sendNextChunk()
{
    qDebug()<<m_bytesSent<<'?'<<m_fileData.size();
    if (m_bytesSent < m_fileData.size()) {
        qint64 bytesToSend = qMin(m_chunkSize, m_fileData.size() - m_bytesSent);
        write(m_fileData.mid(m_bytesSent, bytesToSend));
        flush();
        m_bytesSent += bytesToSend;
        qDebug() << "Sent:" << bytesToSend << "bytes, Total sent:" << m_bytesSent << "bytes";

        if (m_bytesSent >= m_fileData.size()) {
            downloadTimer->stop();
            // 发送完成
            qDebug() << "File transmission completed.";
            downloadTimer->stop();
            //有没有必要发送信号
        }
    }
}




