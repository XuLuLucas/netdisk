#include "book.h"
#include "tcpclient.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QThread>
#include <QThreadPool>
#include <QSharedPointer>
#include <myworker.h>
#include <QDateTime>
#include "mytools.h"

book::book(QWidget *parent): QWidget{parent}
{
    m_pBookListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("新建文件夹");
    m_pDelDirPB = new QPushButton("删除");
    m_pRenameDirPB = new QPushButton("重命名");
    m_pFlushDirPB = new QPushButton("刷新");
    m_pUploadFilePB = new QPushButton("上传文件");
    m_pDownloadFilePB = new QPushButton("下载文件");


    //————————————布局——————————————————
    QVBoxLayout *dirLayout = new QVBoxLayout;
    dirLayout->addWidget(m_pReturnPB);
    dirLayout->addWidget(m_pCreateDirPB);
    dirLayout->addWidget(m_pDelDirPB);
    dirLayout->addWidget(m_pRenameDirPB);
    dirLayout->addWidget(m_pFlushDirPB);

    QVBoxLayout *fileLayout = new QVBoxLayout;
    fileLayout->addWidget(m_pUploadFilePB);
    fileLayout->addWidget(m_pDownloadFilePB);


    QHBoxLayout *hBoxLayout = new QHBoxLayout;
    hBoxLayout->addWidget(m_pBookListW);
    hBoxLayout->addLayout(dirLayout);
    hBoxLayout->addLayout(fileLayout);

    setLayout(hBoxLayout);

    //——————————————信号与槽————————————————-
    connect(m_pCreateDirPB, SIGNAL(clicked(bool)),
            this, SLOT(CreateDir()));

    connect(m_pFlushDirPB, SIGNAL(clicked(bool)),
            this, SLOT(flushList()));

    connect(m_pBookListW, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(enterDir(QListWidgetItem*)));

    connect(m_pReturnPB, SIGNAL(clicked(bool)),
            this, SLOT(exitDir()));

    connect(m_pDelDirPB, SIGNAL(clicked(bool)),
            this, SLOT(delItem()));

    connect(m_pRenameDirPB, SIGNAL(clicked(bool)),
            this, SLOT(renameDir()));

    connect(m_pUploadFilePB, SIGNAL(clicked(bool)),
            this,SLOT(upLoadFile()));

    connect(m_pDownloadFilePB, SIGNAL(clicked(bool)),
            this,SLOT(downLoadFile()));

}

void book::initFileSystem()
{
    m_pDirectory = new Directory;
    m_strJsonPath = "catalog/"+
                    TCPclient::getInstance().getLoginName()+
                    ".json";
    //获取目录
    getCatalog();
    m_strCurPath = "";
    m_fileUploading = new QVector<QPair<QString,QString>>;
}

void book::sendCatalog()
{
    QByteArray byteArray = mytoolReadFileToByteArray(m_strJsonPath);
    PDU *pdu = mkPDU(byteArray.size()+1);
    pdu->enumMsgType = ENUM_MSG_TYPE_CATALOG_DATA;
    memcpy(pdu->caMsg, byteArray, byteArray.size());
    TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    TCPclient::getInstance().getTcpSocket().flush();
    free(pdu);
    pdu = NULL;
}

void book::getCatalog()
{
    TCPclient::getInstance().sendPduWithoutData(ENUM_MSG_TYPE_CATALOG_REQUEST);
}

void book::handleCatalogPDU(PDU *pdu)
{
    QByteArray byteArray = (char*)pdu->caMsg;
    mytoolSaveByteArrayToFile(m_strJsonPath,byteArray);
}

void book::handleUploadSuc(PDU *pdu)
{
    QString fut = pdu->caMsgDsc;
    for(auto x : *m_fileUploading){
        if(x.first == fut){
            qDebug()<<m_pDirectory->addItem(x.second,false,x.first);
            m_pDirectory->saveToJson(m_strJsonPath);
            sendCatalog();
            flushList();
            QMessageBox::information(this,"文件上传",QString("%1 上传成功").arg(x.second));
            break;
        }
    }
}

void book::CreateDir()
{
    QString strDirName;
    while (true) {
        bool ok;
        strDirName = QInputDialog::getText(this, "新建文件夹", "文件夹名称", QLineEdit::Normal, "", &ok);
        if (!ok) {
            // 用户点击取消或关闭对话框，结束流程
            break;
        }
        if (strDirName.isEmpty()) {
            // 用户输入为空，显示警告
            QMessageBox::warning(this, "新建文件夹", "文件夹名称不能为空");
        }
        else if(strDirName.toStdString().size()>31){
            // 文件名过长，显示警告
            QMessageBox::warning(this, "新建文件夹", "文件夹名称不能超过31字节");
        }
        else{
            //创造文件夹
            QString path = m_strCurPath+'/'+strDirName;
            int ret = m_pDirectory->addItem(path, true);
            if(ret==1){
                if(m_pDirectory->saveToJson(m_strJsonPath)){
                    QMessageBox::information(this,"新建文件夹","创建成功");
                    sendCatalog();
                    flushList();
                }
                else{QMessageBox::warning(this,"新建文件夹","创建失败");}
            }
            else if(ret == -1){
                QMessageBox::warning(this,"新建文件夹","创建失败");
            }
            else{
                QMessageBox::warning(this,"新建文件夹","文件夹已存在");
            }

            break;
        }

    }

}

// 这里进行了修改，可以用于文件和文件夹
void book::delItem()
{
    QListWidgetItem* currentItem = m_pBookListW->currentItem();
    if(currentItem){
        QString dirPath;
        dirPath = m_strCurPath +'/' + currentItem->text();
        //告知服务器：
        QVector<FileSystemNode> nodes = m_pDirectory->look(dirPath,true);
        QStringList fut_to_del;//需要减少共享人数的fut
        for(auto node :nodes){
            if(!node.isDir())fut_to_del.append(node.getFUT());
        }
        //发送给服务器要删除的fut
        PDU *pdu = mkPDU(32*fut_to_del.size());
        pdu->enumMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        for(int i = 0;i<fut_to_del.size();i++){
            memcpy((char*)pdu->caMsg+32*i,fut_to_del[i].toStdString().c_str(),32);
        }
        TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        //改变目录并发送给服务器
        m_pDirectory->delItem(dirPath);
        m_pDirectory->saveToJson(m_strJsonPath);
        QTimer::singleShot(100, this, &book::sendCatalog);//防止沾包

        flushList();
    }
    else{
        QMessageBox::warning(this, "提示","请选择要删除的文件夹");
    }

}

//根据当前路径来进行刷新
void book::flushList()
{
    m_pDirectory->loadFromJson(m_strJsonPath);
    //如果当前路径是文件，无法刷新，退回上一级
    if(m_pDirectory->isPathDir(m_strCurPath) == -1){
        exitDir();
        return;
    }
    //qDebug()<<m_pDirectory->look("").size();
    QVector<FileSystemNode> nodes = m_pDirectory->look(m_strCurPath);

    m_pBookListW->clear();

    for (auto node : nodes) {
        QListWidgetItem* item_tmp = new QListWidgetItem;
        if(node.isDir())item_tmp->setIcon(QIcon(QPixmap(":/icon/dir_icon.png")));
        else item_tmp->setIcon(QIcon(QPixmap(":/icon/file_icon.png")));
        item_tmp->setText(node.getName());
        m_pBookListW->addItem(item_tmp);
    }
}

void book::renameDir()
{
    QListWidgetItem* currentItem = m_pBookListW->currentItem();
    // 检查是否选中了项目
    if (!currentItem) {
        QMessageBox::warning(this, "警告", "请选择一个文件或文件夹！");
        return;
    }
    QString path = m_strCurPath+'/'+currentItem->text();

    // 弹出输入对话框以获取新名称
    bool ok;
    QString newName = QInputDialog::getText(this, "重命名", "请输入新的名称:", QLineEdit::Normal, currentItem->text(), &ok);

    // 检查用户是否点击了确认
    if (ok && !newName.isEmpty()) {
        // 修改JSON文件
        m_pDirectory->renameItem(path, newName);
        m_pDirectory->saveToJson(m_strJsonPath);
        sendCatalog();
        flushList();
    } else if (ok) {
        QMessageBox::warning(this, "警告", "新名称不能为空！");
    }
}

void book::enterDir(QListWidgetItem* item)
{

    if(m_strCurPath.isEmpty())m_strCurPath += item->text();
    else m_strCurPath += '/' + item->text();
    flushList();
}

void book::exitDir()
{

    // 如果当前目录就是用户根目录，代表没有上级目录了
    if(m_strCurPath == "")
    {
        QMessageBox::warning(this, "返回", "返回上一级失败：当前已经在用户根目录了");
        return ;
    }

    int idx = m_strCurPath.lastIndexOf('/');
    if(idx==-1){m_strCurPath = "";}
    else m_strCurPath.remove(idx, m_strCurPath.size()-idx);
    // 刷新目录文件为当前目录位置的文件
    flushList();

}


void book::upLoadFile()
{
    //获取本机路径
    QString strUploadFilePath =QFileDialog::getOpenFileName();
    if(strUploadFilePath.isEmpty())return;
    //获取网盘路径与fut
    QFileInfo fileInfo(strUploadFilePath);
    QString path = m_strCurPath+'/'+fileInfo.fileName();
    QString FUT = QString("%1@%2")
                        .arg(TCPclient::getInstance().getLoginName())
                        .arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

    m_fileUploading->append({FUT,path});
    myWorker *task = new myWorker(strUploadFilePath,FUT);
    // 连接信号和槽
    connect(task, &myWorker::finished, task, &myWorker::deleteLater);
    // 启动线程
    task->start();
}

void book::downLoadFile()
{
    QListWidgetItem* currentItem = m_pBookListW->currentItem();
    // 检查是否选中了项目
    if (!currentItem) {
        QMessageBox::warning(this, "警告", "请选择一个文件！");
        return;
    }
    QString path = m_strCurPath+'/'+currentItem->text();
    if(m_pDirectory->isPathDir(path)==1){
        QMessageBox::information(this, "下载文件","当前不支持文件夹下载");
        return;
    }
    else if(m_pDirectory->isPathDir(path)==0){
        QMessageBox::information(this, "下载文件","当前文件不存在");
        flushList();
        return;
    }
    QVector<FileSystemNode> tmp =  m_pDirectory->look(path);
    //获取本地路径
    QString localpath = "netdiskDownload/" + currentItem->text();
    QString baseName = localpath;
    int count = 1;
    while (QFile::exists(localpath)) {
        localpath = QString("%1(%2)").arg(baseName).arg(count);
        count++;
    }
    myWorker *task = new myWorker(localpath,tmp[0].getFUT(),false);
    connect(task, &myWorker::finished, task, &myWorker::deleteLater);
    connect(task, &myWorker::downloadSuc, this, [this](const QString &fileName) {
        QMessageBox::information(this, "下载完毕", QString("文件 \"%1\" 下载完毕。").arg(fileName));
    });
    connect(task, &myWorker::downloadfailed, this, [this](const QString &fileName,const QString reason) {
        QMessageBox::critical(this, "下载失败", QString("文件 \"%1\" 下载失败\n: %2").arg(fileName).arg(reason));
    });
    task->start();
}

