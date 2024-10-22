#ifndef MYQDIR_H
#define MYQDIR_H

#include <QObject>

class myQDir : public QObject
{
    Q_OBJECT
public:
    explicit myQDir(QObject *parent = nullptr);

    static myQDir& getInstance();

    //name在path中
    int createDir(QString path, QString name);//2(文件已存在)1(成功)0（path不存在）-1（创建失败）
    int delDir(QString path, QString name);//1(成功)0（path+name不存在）-1（删除失败）
    int renameDir(QString path, QString name,QString newname);//1(成功)0（path+name不存在）-1（重命名失败）
    QStringList viewDir(QString path, bool& suc);//返回文件夹内容,suc代表返回为空的时候，是由于问价夹无内容还是由于查询失败

private:

signals:
};

#endif // MYQDIR_H
