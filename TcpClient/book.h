#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include"protocol.h"

#include "fileSystem.h"




class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);

    void initFileSystem();//初始化与文件有关的内容，在登陆成功的时候被调用
    void sendCatalog();//向服务器发送json文件
    void getCatalog();//请求json文件

    void handleCatalogPDU(PDU*);//从pdu中取出目录文件
    void handleUploadSuc(PDU*);

signals:

public slots:
    void CreateDir();//新建
    void delItem();//删除
    void flushList();//刷新list界面
    void renameDir();//重命名
    void enterDir(QListWidgetItem*);//打开
    void exitDir();//退出

    void upLoadFile();//上传文件
    void downLoadFile();//下载文件


private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenameDirPB;
    QPushButton *m_pFlushDirPB;
    QPushButton *m_pUploadFilePB;
    QPushButton *m_pDownloadFilePB;

    QString m_strCurPath;//当前路径
    QString m_strHomePath;//根路径
    Directory* m_pDirectory;    //文件系统类
    QString m_strJsonPath;      //文件系统保存路径
    QVector<QPair<QString,QString>>* m_fileUploading;//上传中的文件的fut和名字

};

#endif // BOOK_H
