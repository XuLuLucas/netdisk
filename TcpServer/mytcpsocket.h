#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QTcpSocket>
#include <QDir>
#include "protocol.h"
#include "dbop.h"
#include <QMutex>
#include <QTimer>

struct shareContent {
    /*1聊天消息2好友请求3加好友被同意4加好友被拒绝*/
    /*10分享文件*/
    int type;
    QString receiver;
    QString sender;
    QString message;
    QString sendTime;
    bool operator==(const shareContent& other) const {
        return sender == other.sender &&
               receiver == other.receiver &&
               message == other.message;
    }
};


class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString get_strName()const{return m_strName;}

private:
    QString m_strName;//当前连接的客户的名称
    DBop* m_pDBop;

    //接收或上传文件的时候需要用到参数
    bool m_bUpload = false;         //上传文件模式
    bool m_bDownload = false;       //下载文件模式
    QString FUT;                    //上传或下载文件的fut
    QFile m_file;                   //上传或下载的文件对象
    qint64 m_iTotal = 0;            //上传或下载文件的总长度

    qint64 m_iRecved = 0;           //已经接收到的长度
    qint64 m_iCount = 1;            //需要接收的下一个

    QByteArray m_fileData;          //客户端要下载的文件的数据
    qint64 m_bytesSent = 0;             //已经发送了的字节数
    qint64 m_chunkSize = 4096;      //分包，每次发送的字节数
    QTimer *downloadTimer;        //下载文件的时候用到的定时器

    //——————————工具函数——————————
    void sendPduWithoutData(ENUM_MSG_TYPE msgType, const char* msgDsc1 = NULL, const char* msgDsc2 = NULL);

    void shareFUT();

    QStringList FindAndDelFUT(const QString &futToRemove);

    void sendCatalog();

    void haneleCatalogPDU(PDU*);//从pdu中取出目录文件

    void sendNextChunk();

    //——————检查共享空间——————————————
    void checkShareContent();
    void checkFUT();
    //——————————共享数据—————————————
    static QStringList FUTlist;
    static QMutex mutex;  // 互斥锁

    static QVector<shareContent> shareContentList;
    static QMutex shareContentListLock;  // 互斥锁

public slots:
    void recvMsg();//收到消息的处理
    void clientOffline();//客户端下线的处理
    void test();//定时执行的操作


signals:
    void workOver();//客户端下线后，善后工作也处理完了，就发出这个信号
};

#endif // MYTCPSOCKET_H
