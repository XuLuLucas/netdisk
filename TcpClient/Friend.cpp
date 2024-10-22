#include "Friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <QDebug>
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <QDateTime>
#include "fileSystem.h"

Friend ::Friend(QWidget *parent): QWidget{parent}
{

    m_pFriendListWidget = new QListWidget;
    m_pShowMsgTE = new QTextEdit;
    m_pInputMsgLE = new QLineEdit;
    m_pMsgSendPB = new QPushButton("发送");
    m_pShareFilePB = new QPushButton("分享文件");

    m_pSearchUsrPB = new QPushButton("查找用户");
    m_pFlushFriendPB = new QPushButton("刷新好友列表");
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pGroupChatPB = new QPushButton("群聊");


    //-----------构建页面--------------
    //左边
    QVBoxLayout *pLeftPBVBL = new QVBoxLayout;
    pLeftPBVBL->addWidget(m_pSearchUsrPB);
    pLeftPBVBL->addWidget(m_pFlushFriendPB);
    pLeftPBVBL->addWidget(m_pDelFriendPB);
    pLeftPBVBL->addWidget(m_pGroupChatPB);

    //右边
    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);
    pMsgHBL->addWidget(m_pShareFilePB);

    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pShowMsgTE);
    pRightPBVBL->addLayout(pMsgHBL);


    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addLayout(pLeftPBVBL);
    pMain->addWidget(m_pFriendListWidget);
    pMain->addLayout(pRightPBVBL);


    QVBoxLayout *pAppVBL = new QVBoxLayout;
    pAppVBL->addLayout(pMain);


    setLayout(pAppVBL);

    //---------信号与槽--------------------

    connect(m_pSearchUsrPB, SIGNAL(clicked(bool)),
            this, SLOT(searchUsr()));

    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),
            this, SLOT(flushFriend()));

    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),
            this, SLOT(delFriend()));

    connect(m_pFriendListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(changChatWindow(QListWidgetItem*)));

    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),
            this, SLOT(privateChat()));

    connect(m_pShareFilePB,SIGNAL(clicked(bool)),
            this, SLOT(shareFile()));

    //emit created();//还没用到，后面想办法
}


void Friend::showFriend(PDU *pdu)
{
    if(pdu == NULL)return;
    m_pFriendListWidget->clear();
    unsigned int size = pdu->uiMsgLen/32;
    char caTmp[32];
    for(unsigned int i = 0;i<size;i++){
        memcpy(caTmp, (char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caTmp);
    }
}

void Friend::receivedMessage(PDU *recvPdu)
{
    // 从 PDU 中提取数据
    QString friendName = recvPdu->caMsgDsc;
    QString sendTime = recvPdu->caMsgDsc+32;
    QString message = '['+sendTime+']'+QString::fromLocal8Bit((char*)(recvPdu->caMsg));

    // 提示有新消息
    QMessageBox::information(this, "新消息通知", QString("%1 发来了新消息").arg(friendName));

    // 如果聊天记录中不存在，则需先创建聊天记录
    if (chatHistory.contains(friendName) == false) {
        chatHistory.insert(friendName, "");
    }

    // 添加进聊天记录
    chatHistory[friendName] += friendName + ":\n" + message + "\n"; // 添加消息到聊天记录

    //如果当前聊天的好友是接收到的消息的好友，则更新显示
    QListWidgetItem *currentItem = opeWidget::getInstance().m_pFriend->m_pFriendListWidget->currentItem();
    if (currentItem && currentItem->text() == friendName) {
        m_pShowMsgTE->setPlainText(chatHistory.value(friendName)); // 更新显示
    }

}

void Friend::searchUsr()
{
    QString name = QInputDialog::getText(this, "搜索用户","用户名：");
    //发送消息进行查询查询
    PDU *pdu = mkPDU(0);
    pdu->enumMsgType = ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
    memcpy(pdu->caMsgDsc, name.toStdString().c_str(), name.size());
    TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::flushFriend()
{
    PDU *pdu = mkPDU(0);
    pdu->enumMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::delFriend()
{
    QListWidgetItem *pItem = m_pFriendListWidget->currentItem();
    if(pItem == NULL){
        QMessageBox::information(this, "删除用户","请先选择用户");
    }
    else{
        QString strPerUsrName = pItem->text();
        TCPclient::getInstance().sendPduWithoutData(
            ENUM_MSG_TYPE_DEL_FRIEND_REQUEST,
            strPerUsrName.toStdString().c_str(),
            TCPclient::getInstance().getLoginName().toStdString().c_str());
    }
}

void Friend::changChatWindow(QListWidgetItem *item)
{
    QString friendName = item->text(); // 获取好友名称
    // 初始化为空记录
    if (!chatHistory.contains(friendName))chatHistory.insert(friendName, "");
    // 检查聊天记录是否存在
    if (chatHistory.value(friendName)=="") {
        m_pShowMsgTE->setPlainText("没有与"+ friendName + "的聊天记录 ");
    }
    else m_pShowMsgTE->setPlainText(chatHistory.value(friendName));
}

void Friend::privateChat()
{
    QListWidgetItem *currentItem = m_pFriendListWidget->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, "警告", "请先选择一个朋友进行聊天。");
        return;
    }

    QString friendName = currentItem->text(); // 获取当前选中的朋友的名字
    QString selfName = TCPclient::getInstance().getLoginName();

    // 从输入框获取消息
    QString message = m_pInputMsgLE->text();
    if (message.isEmpty()) {
        QMessageBox::warning(this, "警告", "消息不能为空。");
        return; // 如果消息为空，返回
    }

    //更新聊天记录并显示
    chatHistory[friendName] +=
        selfName + ":\n"+'['+QDateTime::currentDateTime().toString("MM-dd hh:mm:ss")+']'+message + "\n"; // 将消息添加到对应好友的记录中
    m_pInputMsgLE->clear();
    m_pShowMsgTE->setPlainText(chatHistory.value(friendName)); // 显示该朋友的聊天记录

    //将消息发送出去
    QByteArray localMessage = message.toLocal8Bit(); // 转换为本地编码的字节数组
    PDU *pdu = mkPDU(localMessage.size()+1);
    pdu->enumMsgType = ENUM_MSG_TYPE_PRIVATE_DATA;
    memcpy(pdu->caMsgDsc, friendName.toStdString().c_str(), friendName.size());//有待修改
    memcpy(pdu->caMsgDsc+32, selfName.toStdString().c_str(), selfName.size());
    memcpy(pdu->caMsg, localMessage.constData(), localMessage.size() + 1); // +1 用于 '\0'
    TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void Friend::shareFile()
{
    //获取分享者和分享的文件
    QListWidgetItem* Item =  m_pFriendListWidget->currentItem();
    if(!Item){
        QMessageBox::warning(this, "分享文件","请选择分享的朋友");
        return;
    }
    QString perName = Item->text();
    QString sharefileFut;
    QString sharefileName;
    selectFile(sharefileFut, sharefileName);

    //将消息发送出去
    PDU *pdu = mkPDU(64);
    pdu->enumMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    memcpy(pdu->caMsgDsc, perName.toStdString().c_str(), perName.size());//有待修改
    memcpy(pdu->caMsgDsc+32, TCPclient::getInstance().getLoginName().toStdString().c_str(), 32);
    memcpy(pdu->caMsg, sharefileFut.toStdString().c_str(), 32);
    memcpy((char*)pdu->caMsg+32, sharefileName.toStdString().c_str(), 32);
    TCPclient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void Friend::getshareFile(const QString& fut, const QString& fileName, const QString& sender)
{
    // 创建提示信息
    QString message = QString("%1 给你分享了文件 \"%2\"。\n你想接收还是拒绝？")
                          .arg(sender)
                          .arg(fileName);

    // 弹出对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "文件分享", message,
                                  QMessageBox::Yes | QMessageBox::No);

    // 拒绝则直接返回
    if (reply == QMessageBox::No)return;
    //同意
    //选择要保存的位置
    QString place;
    QString null_;
    selectFile(null_,place,true);
    qDebug()<<place;
    //修改目录
    Directory* tmpDirectory  = new Directory;
    tmpDirectory->loadFromJson("catalog/"+TCPclient::getInstance().getLoginName()+".json");
    QString fliePath = place+'/'+fileName;
    tmpDirectory->addItem(fliePath,false,fut);
    tmpDirectory->saveToJson("catalog/"+TCPclient::getInstance().getLoginName()+".json");
    opeWidget::getInstance().m_pBook->sendCatalog();
    //告知服务器已经接收，让服务器把共享人数加1
    TCPclient::getInstance().sendPduWithoutData(ENUM_MSG_TYPE_SHARE_FILE_RESPOND,fut.toStdString().c_str());
}

void Friend::selectFile(QString& shareFut, QString& filename, bool selectDir) {
    //加载目录
    Directory* tmpDirectory  = new Directory;
    tmpDirectory->loadFromJson(
        "catalog/"+TCPclient::getInstance().getLoginName()+".json");
    // 创建一个对话框
    QDialog *dialog = new QDialog();

    if(selectDir)dialog->setWindowTitle("选择保存位置");
    else dialog->setWindowTitle("选择文件");
    dialog->setModal(true);
    dialog->setFixedSize(400, 300); // 设置对话框的固定大小

    QListWidget *listWidget = new QListWidget(dialog);

    QPushButton *selectButton;
    if(selectDir) selectButton = new QPushButton("确定", dialog);
    else selectButton = new QPushButton("分享", dialog);
    QPushButton *exitButton = new QPushButton("返回", dialog);

    // 设置按钮的尺寸策略
    selectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    exitButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(selectButton);
    buttonLayout->addWidget(exitButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(buttonLayout);

    // 设置布局的间距和边距
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QString curPath = "";

    auto updateList = [&]() {
        listWidget->clear();
        QVector<FileSystemNode> nodes = tmpDirectory->look(curPath);
        for (const auto &node : nodes) {
            QListWidgetItem *item = new QListWidgetItem;
            item->setText(node.getName());
            item->setIcon(node.isDir() ? QIcon(":/icon/dir_icon.png") : QIcon(":/icon/file_icon.png"));
            listWidget->addItem(item);
        }
    };

    updateList(); // 初次填充列表

    // 处理双击项目的事件
    connect(listWidget, &QListWidget::itemDoubleClicked, [&](QListWidgetItem *item) {
        QString itemName = item->text();
        QString newPath = curPath + "/" + itemName;

        if (tmpDirectory->isPathDir(newPath) == 1) { // 检查是否为目录
            curPath = newPath;
            updateList(); // 更新列表
        }
        else if(tmpDirectory->isPathDir(newPath) == -1){
            if(selectDir){
                //如果是文件则不处理
            }
            else{// 处理文件分享逻辑
                QString tmp_fut;
                tmp_fut = tmpDirectory->look(newPath)[0].getFUT();
                filename = tmpDirectory->look(newPath)[0].getName();
                shareFut = tmp_fut;
                dialog->accept();
            }
        }
        else{QMessageBox::warning(dialog, "警告", "文件不存在");}
    });
    // 处理返回按钮的点击事件
    connect(exitButton, &QPushButton::clicked, [&]() {
        if (curPath.isEmpty()) {
            QMessageBox::warning(dialog, "警告", "已经是根目录");
        } else {
            // 返回上一级目录
            int lastSlashIndex = curPath.lastIndexOf('/');
            curPath = curPath.left(lastSlashIndex);
            updateList(); // 更新列表
        }
    });
    // 处理确定按钮事见
    connect(selectButton, &QPushButton::clicked, [&]() {
        QListWidgetItem *currentItem = listWidget->currentItem();
        if(selectDir){
            if(currentItem){
                QString newPath = curPath+'/'+currentItem->text();
                if(tmpDirectory->isPathDir(newPath) == 1){
                    curPath = newPath;
                }
            }
            filename = curPath;
            dialog->accept();
            return;//这里不return会继续向下执行乃至系统崩溃，为什么
        }

        if (!currentItem) {
            QMessageBox::warning(dialog, "警告", "请选择要分享的文件");
        }

        QString newPath = curPath + "/" + currentItem->text();
        if (tmpDirectory->isPathDir(newPath) == 1) {
            QMessageBox::warning(dialog, "警告", "不能分享文件夹");
        } else if (tmpDirectory->isPathDir(newPath) == -1) {
            shareFut = tmpDirectory->look(newPath)[0].getFUT();
            filename = tmpDirectory->look(newPath)[0].getName();
            dialog->accept(); // 分享后关闭对话框
        } else {
            QMessageBox::warning(dialog, "警告", "文件不存在");
        }
    });

    // 显示对话框
    dialog->exec();

}

//现在应该改成服务器未连接的时候发送，不过这里先不管了
void Friend::sendMessageFailed(PDU *recvPdu)
{
    // 从 PDU 中提取数据
    QString friendName = recvPdu->caMsgDsc;
    // 如果聊天记录中不存在，则需先创建聊天记录
    if (chatHistory.contains(friendName) == false) {
        chatHistory.insert(friendName, "");
    }

    // 添加进聊天记录进行提醒
    chatHistory[friendName] += "!!! 发送消息失败，请检查对方是否在线\n"; // 添加消息到聊天记录
    //如果当前聊天的好友是接收到的消息的好友，则更新显示
    QListWidgetItem *currentItem = opeWidget::getInstance().m_pFriend->m_pFriendListWidget->currentItem();
    if (currentItem && currentItem->text() == friendName) {
        m_pShowMsgTE->setPlainText(chatHistory.value(friendName)); // 更新显示
    }

}


