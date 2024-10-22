#include "myqdir.h"
#include <QDir>

myQDir::myQDir(QObject *parent): QObject{parent}{}

myQDir &myQDir::getInstance()
{
    static myQDir instance;
    return instance;
}

int myQDir::createDir(QString path, QString name)
{
    QDir dir(path);
    if (!dir.exists()) {
        return 0; // 路径不存在
    }
    if (dir.exists(name)) {
        return 2; // 目录已存在
    }

    if (dir.mkdir(name)) {
        return 1; // 成功创建
    } else {
        return -1; // 创建失败
    }
}

int myQDir::delDir(QString path, QString name)
{
    QDir dir(path);

    // 检查路径 + 名称是否存在
    if (!dir.exists(name)) {
        return 0; // 路径 + 名称不存在
    }

    // 构造完整路径
    QString fullPath = dir.filePath(name);

    // 使用 removeRecursively() 方法递归删除文件夹及其内容
    if (dir.rmdir(name)) {
        return 1; // 成功删除（仅删除空目录）
    } else if (QDir(fullPath).removeRecursively()) {
        return 1; // 成功删除（递归删除）
    } else {
        return -1; // 删除失败
    }
}


int myQDir::renameDir(QString path, QString name, QString newname)
{
    QDir dir(path);

    if (!dir.exists(name)) {
        return 0; // 路径+名称不存在
    }

    if (dir.rename(name, newname)) {
        return 1; // 成功重命名
    } else {
        return -1; // 重命名失败
    }
}

QStringList myQDir::viewDir(QString path, bool& suc)
{
    QDir dir(path);
    QStringList contents;

    if (!dir.exists()) {
        suc = false; // 目录不存在
        return contents; // 返回空列表
    }

    contents = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries); // 获取目录内容
    suc = !contents.isEmpty(); // 设置 suc 为 true 如果内容不为空，false 否则

    return contents;
}
