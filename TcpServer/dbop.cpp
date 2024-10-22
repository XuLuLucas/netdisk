#include "dbop.h"
#include <QMessageBox>
#include <QDebug>
#include <QSqlError>


DBop::DBop(QString linkname, QObject *parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE",linkname);//数据库链接,需要不同的名称
}

//析构函数
DBop::~DBop(){m_db.close();}

void DBop::init()
{
    m_db.setHostName("localhost");//主机名，如果连接远程的数据库就要写ip地址或者主机名
    m_db.setDatabaseName("D:/qtProject/TcpServer/cloud.db");//路径

    if(m_db.open()){
        qDebug() << "数据库连接成功";
    }
    else QMessageBox::critical(NULL, "错误", "打开数据库失败");
}

void DBop::close()
{
    m_db.close();
}

bool DBop::handleRegister(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL)return false;
    QString command = QString("insert into usrInfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    QSqlQuery query(m_db);
    return query.exec(command);
}

bool DBop::handleLogin(const char *name, const char *pwd, bool& state)
{
    if(name == NULL || pwd == NULL)return false;
    QString command = QString("select * from  usrInfo where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
    QSqlQuery query(m_db);
    query.exec(command);
    if(!query.next()){//如果没有找到记录，就是用户名或密码不正确
        return false;
    }
    else{
        state = query.value("online").toInt();
        //如果没有登陆，则登陆成功，将online改为1
        if(!state){
            command = QString("update usrInfo set online=1 where name = \'%1\' and pwd = \'%2\'").arg(name).arg(pwd);
            query.exec(command);
        }
        return true;
    }

}

void DBop::handleClientOffline(const char *name)
{
    if(name == NULL)return;
    QString command = QString("update usrInfo set online = 0 where name = \'%1\'").arg(name);
    QSqlQuery query(m_db);
    query.exec(command);
}

QStringList DBop::handleSerchOnline()
{
    QString command = QString("select name from usrInfo where online = 1");
    QSqlQuery query(m_db);
    query.exec(command);
    QStringList res;
    res.clear();
    while(query.next()){
        res.append(query.value(0).toString());
    }
    return res;
}

int DBop::handleSerchUsr(const char* name)
{
    if(name == NULL)return -1;
    QString command = QString("select online from usrInfo where name = \'%1\'").arg(name);
    QSqlQuery query(m_db);
    query.exec(command);

    if(query.next()){
        return query.value(0).toInt();
    }
    else return -1;

}


int DBop::handleCheckdFriend(const char *perName, const char *usrName)
{
    if (perName == NULL || usrName == NULL) return -1;

    QString command;
    QSqlQuery query(m_db);

    // 检查是否已经是好友
    command = QString(
                  "SELECT * FROM"
                  "(SELECT id as ind FROM friend "
                  "WHERE friendId = "
                  "(SELECT id FROM usrInfo WHERE name = \'%1\') "
                  "UNION "
                  "SELECT friendId as ind FROM friend "
                  "WHERE id = "
                  "(SELECT id FROM usrInfo WHERE name = \'%1\'))"
                  "WHERE ind = "
                  "(SELECT id FROM usrInfo WHERE name = \'%2\') ")
                  .arg(usrName).arg(perName);
    query.exec(command);
    if (query.next()) return 0;

    //检查 usrName 是否在线，稍微有一点多余，本来就是按照在线名单来添加好友的，之后可以更改一下
    command = QString("SELECT online FROM usrInfo WHERE name = \'%1\'").arg(perName);
    query.exec(command);
    if (query.next() && query.value(0).toInt() == 0) {//使用next函数并不是要迭代，而是必须使用一次
        return -1; // usrName 不在线
    }

    return 1;//非好友且在线

}

bool DBop::handleAddFriend(const char *perName, const char *usrName)
{
    QString command;
    QSqlQuery query(m_db);

    command = QString(
                  "INSERT INTO friend values("
                  "(SELECT id FROM usrInfo WHERE name = \'%1\'),"
                  "(SELECT id FROM usrInfo WHERE name = \'%2\')"
                  ")")
                  .arg(usrName).arg(perName);
    return query.exec(command);
}

QStringList DBop::handleSerchFriend(const char *Name)
{
    QString command;
    QSqlQuery query(m_db);
    command = QString(
                  "SELECT name from usrInfo where id in ("
                  "SELECT id as ind FROM friend "
                  "WHERE friendId = "
                  "(SELECT id FROM usrInfo WHERE name = \'%1\')"

                  "UNION "

                  "SELECT friendId as ind FROM friend "
                  "WHERE id = "
                  "(SELECT id FROM usrInfo WHERE name = \'%1\')"
                  ")"
                  ).arg(Name);
    query.exec(command);
    QStringList res;
    res.clear();
    while(query.next()){
        res.append(query.value(0).toString());
    }
    return res;

}

bool DBop::handleDelFriend(const char *Name1, const char *Name2)
{
    QString command;
    QSqlQuery query(m_db);

    command = QString(
                  "DELETE from friend "
                  "WHERE "
                  "id = (select id from usrInfo where name = \'%1\') "
                  "and friendId = (select id from usrInfo where name = \'%2\') "
                  "or "
                  "friendId = (select id from usrInfo where name = \'%1\') "
                  "and id = (select id from usrInfo where name = \'%2\') "
                  )
                  .arg(Name1).arg(Name2);
    return query.exec(command);
}

bool DBop::flieCreate(QString fut)
{
    QString command = "INSERT INTO fileInfo (fut, shareNum, status, createTime) "
                      "VALUES (:fut, 0, 0, strftime('%s', 'now'))";
    QSqlQuery query(m_db);
    query.prepare(command);
    query.bindValue(":fut", fut);
    return query.exec();
}

bool DBop::flieUpSuc(QString fut) {
    QString command = "UPDATE fileInfo SET status = 1 WHERE fut = :fut";
    QSqlQuery query(m_db);
    query.prepare(command);
    query.bindValue(":fut", fut);
    return query.exec();
}


bool DBop::flieUpFaile(QString fut) {
    QString command = "DELETE FROM fileInfo WHERE fut = :fut";
    QSqlQuery query(m_db);
    query.prepare(command);
    query.bindValue(":fut", fut);
    return query.exec();
}


bool DBop::deShareWithDelete(QString fut,bool& del) {
    QSqlQuery query(m_db);
    del =false;
    // 减少共享数量
    query.prepare("UPDATE fileInfo SET shareNum = shareNum - 1 WHERE fut = :fut");
    query.bindValue(":fut", fut);
    if (!query.exec()) return false;

    // 检查共享数量
    query.prepare("SELECT shareNum FROM fileInfo WHERE fut = :fut");
    query.bindValue(":fut", fut);
    query.exec();

    if (query.next() && query.value(0).toInt() <= 0) {
        query.prepare("DELETE FROM fileInfo WHERE fut = :fut");
        query.bindValue(":fut", fut);
        del = true;
        return query.exec();
    }
    return true;
}

bool DBop::deShare(QString fut) {
    QString command = "UPDATE fileInfo SET shareNum = shareNum - 1 WHERE fut = :fut";
    QSqlQuery query(m_db);
    query.prepare(command);
    query.bindValue(":fut", fut);
    return query.exec();
}

bool DBop::addShare(QString fut) {
    QString command = "UPDATE fileInfo SET shareNum = shareNum + 1 WHERE fut = :fut";
    QSqlQuery query(m_db);
    query.prepare(command);
    query.bindValue(":fut", fut);
    return query.exec();
}

QStringList DBop::fileNoShare() {
    QString command = "SELECT fut FROM fileInfo WHERE shareNum = 0";
    QSqlQuery query(m_db);
    query.exec(command);

    QStringList res;
    while (query.next()) {
        res.append(query.value(0).toString());
    }
    return res;
}




