//好友界面


#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "protocol.h"
#include <QMap>


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    void showFriend(PDU*);
    void receivedMessage(PDU*);//收到私聊信息
    void sendMessageFailed(PDU*);
    void selectFile(QString& shareFut, QString& filename, bool selectDir = false);

    //聊天记录
    QMap<QString, QString> chatHistory;

private:

    //界面组成

    QListWidget *m_pFriendListWidget;
    QTextEdit *m_pShowMsgTE;

    QLineEdit *m_pInputMsgLE;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pShareFilePB;

    QPushButton *m_pSearchUsrPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pDelFriendPB;
    QPushButton *m_pGroupChatPB;



signals:
    void created(); //创建信号

public slots:

    void searchUsr();
    void flushFriend();
    void delFriend();
    void changChatWindow(QListWidgetItem*);
    void privateChat();
    void shareFile();
    void getshareFile(const QString& fut, const QString& fileName, const QString& sender);
};


#endif // FRIEND_H
