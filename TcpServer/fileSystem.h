#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QThread>
#include <QDebugStateSaver>

//文件系统给出的路径必须是 xxx/xxxx/xxxx

// 节点类
class FileSystemNode {
private:
    QString name;
    QString FUT;
    bool isDirectory;
    QVector<FileSystemNode*> children;

public:
    FileSystemNode(const QString &name, bool isDirectory, QString fut = "0@0");

    bool addChild(FileSystemNode *child);

    bool removeChild(const QString &name);

    QString getName() const;

    QString getFUT() const;

    bool isDir() const;

    QVector<FileSystemNode*> getChildren() const;
};

// 文件类
class Directory {
public:
    Directory() {
        root = new FileSystemNode("root", true);
    }

    ~Directory() {
        deleteNode(root); // 递归释放所有的内存
    }
    //1是，-1不是，0不存在
    int isPathDir(const QString &path);
    // path 是要添加的项目路径，isDir代表是否是文件夹，all如果项目之前的路径不存在，是否递归创建
    // 返回1代表创建成功，0代表已存在，-1代表失败
    int addItem(const QString &path, bool isDir, QString fut = "0@0", bool all = false);

    // 返回1是删除成功，-1是删除失败
    int delItem(const QString &path);


    QVector<FileSystemNode> look(const QString &path, bool all=false);

    //文件读取和保存
    bool saveToJson(const QString &filePath) const;
    void loadFromJson(const QString &filePath);

private:
    FileSystemNode *root;

    void collectNode(FileSystemNode *node, QVector<FileSystemNode> &ret, bool all= false);

    void deleteNode(FileSystemNode *node);

    QJsonObject nodeToJson(FileSystemNode *node) const;

    void jsonToNode(const QJsonObject &json, FileSystemNode *node, bool isRoot = false);
};

#endif // FILESYSTEM_H
