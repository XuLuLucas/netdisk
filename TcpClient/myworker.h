#ifndef MYWORKER_H
#define MYWORKER_H

#include <QObject>
#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <protocol.h>

class myWorker : public QThread
{
    Q_OBJECT
public:
    explicit myWorker(const QString &filePath, const QString FUT, bool upload = true,QObject *parent = nullptr);
    void run() override;

private:
    bool m_bUpload;             //上传模式/下载模式
    QTcpSocket *m_tcpSocket;    //套接字

    QString m_FUT;              //文件唯一标识符
    QString m_strPath;          //文件的本地路径
    QFile m_file;               //文件对象
    qint64 m_iTotal = 0;        //文件的总长度

    //发送文件需要的变量
    qint64 m_bytesSent;         //已经发送了的字节数
    const qint64 m_chunkSize;   //分包，每次发送的字节数
    QByteArray m_fileData;      //读取的文件数据
    QTimer *m_timer;            //定时器

    //接收文件需要的变量
    qint64 m_iRecved = 0; //已经接收到的长度
    qint64 m_iCount = 1;  //需要接收的下一个
    QTimer *m_pWait_timer;   //等待一段时间服务器无响应则传输失败

    inline void uploadRequest();
    void sendFile();
    void sendNextChunk();

    inline void downloadRequest();
    inline void getDownloadInfo();
    inline void preToDownload();
    void waitTimeout();


    void workover();

public slots:
    void recvMsg();     //下载模式中接收文件信息

signals:
    //下载成功信号，上传成功信号没有，因为是否上传成功是由服务器判断
    void downloadSuc(QString m_strPath);
    //下载失败信号
    void downloadfailed(QString m_strPath,QString reason);
};

#endif // MYWORKER_H
