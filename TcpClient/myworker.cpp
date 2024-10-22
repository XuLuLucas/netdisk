#include "myworker.h"


myWorker::myWorker(const QString &filePath, const QString FUT, bool upload, QObject *parent)
    : QThread(parent),m_bUpload(upload),m_FUT(FUT),m_strPath(filePath),
    m_bytesSent(0),m_chunkSize(4096)//这里直接进行了指定
    {

}



void myWorker::run() {
    //建立连接，10秒后未建立则结束进程
    m_tcpSocket = new QTcpSocket();
    //这里注意，以后应该将其修改，从配置文件中加载而不是直接指定
    m_tcpSocket->connectToHost("127.0.0.1", 8888);
    if (!m_tcpSocket->waitForConnected(10000)) {
        qDebug() << "Connection failed:" << m_tcpSocket->errorString();
        return;
    }

    if(m_bUpload){//上传文件模式
        uploadRequest();

        if (!m_tcpSocket->waitForReadyRead(3000)) {
            qDebug() << "连接失败：服务器无回应";
            workover();
        }
        m_timer = new QTimer();
        connect(m_timer, &QTimer::timeout, this, [=]() {
            sendNextChunk();
        });

        sendFile();
    }
    else{
        //发送请求信息并等待
        downloadRequest();
        if (!m_tcpSocket->waitForReadyRead(3000)) {
            qDebug() << "连接失败：服务器无回应";
            workover();
            emit downloadfailed(m_strPath,"服务器无回应");
        }
        //收取下载需要的信息（实际就是文件大小）
        getDownloadInfo();
        //下载文件之前的准备工作，其中包括了开始传输请求
        preToDownload();
    }


    exec();
}

void myWorker::uploadRequest(){
    QFile file(m_strPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << m_strPath;
        workover();
    }
    qint64 fileSize = file.size();//文件的大小
    PDU *pdu = mkPDU(0);
    pdu->enumMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;//消息类型
    sprintf(pdu->caMsgDsc, "%s %lld", m_FUT.toStdString().c_str(), fileSize);
    //读取方式：sscanf(pdu->caData, "%s %lld", fileName, &fileSize);
    m_tcpSocket->write((char*)pdu, pdu->uiPDULen);//发送消息
    free(pdu);
    pdu = NULL;
}

void myWorker::sendFile() {
    QFile file(m_strPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << m_strPath;
        return;
    }

    m_fileData = file.readAll();
    m_iTotal = m_fileData.size();
    file.close();

    QThread::sleep(1); // 暂停1秒
    // 开始定时器，每10毫秒发送一次数据
    m_timer->start(10);

}

void myWorker::sendNextChunk() {
    if (m_bytesSent < m_fileData.size()) {
        qint64 bytesToSend = qMin(m_chunkSize, m_fileData.size() - m_bytesSent);
        m_tcpSocket->write(m_fileData.mid(m_bytesSent, bytesToSend));
        m_tcpSocket->flush();
        m_bytesSent += bytesToSend;
        qDebug() << "Sent:" << bytesToSend << "bytes, Total sent:" << m_bytesSent << "bytes";

        if (m_bytesSent >= m_iTotal) {
            m_timer->stop();
            // 发送完成，发出信号
            qDebug() << "File transmission completed.";
            m_timer->stop();
            workover();;//我滴任务完成啦
        }
    }
}

void myWorker::downloadRequest()
{
    PDU *pdu = mkPDU(0);
    pdu->enumMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;//消息类型
    sprintf(pdu->caMsgDsc, "%s",m_FUT.toStdString().c_str());
    m_tcpSocket->write((char*)pdu, pdu->uiPDULen);//发送消息
    free(pdu);
    pdu = NULL;

}

void myWorker::getDownloadInfo()
{
    unsigned int uiPDULen = 0;
    m_tcpSocket->read((char*)&uiPDULen, sizeof(unsigned int));
    unsigned int uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *recvPdu = mkPDU(uiMsgLen);
    m_tcpSocket->read((char*)recvPdu+sizeof(unsigned int),uiPDULen - sizeof(unsigned int));
    qint64 fileSize = 0;
    sscanf(recvPdu->caMsgDsc, "%lld", &fileSize);
    m_iTotal = fileSize;
}

void myWorker::preToDownload()
{
    m_file.setFileName(m_strPath);
    // 以只写的方式打开文件，如果文件不存在，则自动创建文件
    if(m_file.open(QIODevice::WriteOnly))
    {
        m_iRecved = 0;
        m_iCount = 1;
        connect(m_tcpSocket, SIGNAL(readyRead()),this, SLOT(recvMsg()));
        m_pWait_timer = new QTimer;
        connect(m_pWait_timer, &QTimer::timeout, this, [=]() {
            waitTimeout();
        });
        m_pWait_timer->start(10000);//十秒的等待时间
        //发送消息请求开始
        PDU *pdu = mkPDU(0);
        pdu->enumMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_START;//消息类型
        m_tcpSocket->write((char*)pdu, pdu->uiPDULen);//发送消息
        free(pdu);
        pdu = NULL;
    }
    else qDebug()<<"failed to creat or open file";
}

void myWorker::waitTimeout()
{
    emit downloadfailed(m_strPath,"传输过程中服务器断开连接");
    workover();
}

void myWorker::workover(){
    // 停止和删除定时器
    if (m_timer) {
        m_timer->stop(); // 停止定时器
        delete m_timer;  // 删除定时器
        m_timer = nullptr; // 避免悬空指针
    }

    if (m_pWait_timer) {
        m_pWait_timer->stop(); // 停止等待定时器
        delete m_pWait_timer;  // 删除等待定时器
        m_pWait_timer = nullptr; // 避免悬空指针
    }

    // 处理套接字
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost(); // 确保套接字断开连接
        delete m_tcpSocket; // 释放套接字资源
    }

    // 确保文件关闭
    if (m_file.isOpen()) {
        m_file.close();
    }
    quit();
}

void myWorker::recvMsg()
{
    while (m_tcpSocket->bytesAvailable() > 0){
        m_pWait_timer->stop();
        QByteArray buff = m_tcpSocket->readAll();
        m_file.write(buff);
        m_iRecved += buff.size();

        qDebug() << QString("第 %1 次传入文件,接受数据大小:%2").arg(m_iCount).arg(buff.size());
        m_iCount++;

        if(m_iTotal == m_iRecved)
        {
            m_file.close();
            emit downloadSuc(m_strPath);
            workover();
        }
        m_pWait_timer->start(100000);
    }
}


