#ifndef DBOP_H
#define DBOP_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>

class DBop : public QObject
{
    Q_OBJECT
public:
    explicit DBop(QString linkname,QObject *parent = nullptr);
    ~DBop();

    void init();
    void close();
    //———————————管理用户和用户关系——————————————
    bool handleRegister(const char* name, const char* pwd);
    //登陆，返回的bool变量仅代表用户名与密码是否匹配，是否登陆成功还要看state
    bool handleLogin(const char* name, const char* pwd, bool& state);
    //用户端下线
    void handleClientOffline(const char *name);
    //查询所有在线用户
    QStringList handleSerchOnline();
    //根据名称查找用户
    int handleSerchUsr(const char *name);//int用于表示在线(1)、不在线(0)、不存在(-1)三种状态
    //查找好友关系，0是已经是好友了，-1是对方不在线，1是对方在线
    int handleCheckdFriend(const char *perName, const char *usrName);//已经是好友（0）在线（1）不在线（-1）
    //添加好友
    bool handleAddFriend(const char *perName, const char *usrName);
    //搜索好友
    QStringList handleSerchFriend(const char *Name);
    //删除好友
    bool handleDelFriend(const char *Name1, const char *Name2);


    //————————————管理文件————————————————————

    //文件创建，插入新的记录，状态为0，共享人数为0
    bool flieCreate(QString fut);

    //上传成功，状态改为1
    bool flieUpSuc(QString fut);

    //上传失败，删除记录
    bool flieUpFaile(QString fut);

    //共享数量减1，当共享数量小于等于0时删除该记录
    bool deShareWithDelete(QString fut, bool& del);

    //共享数量减1，不做删除
    bool deShare(QString fut);

    //共享数量加1
    bool addShare(QString fut);

    //返回共享数量为0的所有fut
    QStringList fileNoShare();


signals:

private:
    QSqlDatabase m_db;  //用于连接数据库
};

#endif // DBOP_H
