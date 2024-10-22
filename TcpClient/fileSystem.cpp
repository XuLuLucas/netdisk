#include "fileSystem.h"
#include <QDir>

FileSystemNode::FileSystemNode(const QString &name, bool isDirectory, QString fut)
    : name(name), FUT(fut), isDirectory(isDirectory) {}

bool FileSystemNode::addChild(FileSystemNode *child) {
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->getName() == child->name) {
            return false; //重名，添加失败
        }
    }
    children.append(child);
    return true;
}

bool FileSystemNode::removeChild(const QString &name) {
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->getName() == name) {
            delete children[i]; // 释放内存
            children.remove(i); // 从子节点列表中移除
            return true;
        }
    }
    return false;   // 没找到，返回false
}

QString FileSystemNode::getName() const {
    return name;
}

QString FileSystemNode::getFUT() const
{
    return FUT;
}

void FileSystemNode::rename(QString newName)
{
    name = newName;
}

bool FileSystemNode::isDir() const {
    return isDirectory;
}

QVector<FileSystemNode *> FileSystemNode::getChildren() const {
    return children;
}

int Directory::isPathDir(const QString &path)
{
    QStringList parts = path.split('/');
    FileSystemNode *currentNode = root;
    for (int index = 1; index < parts.size(); ++index) {
        QString part = parts[index];
        bool found = false;

        for (FileSystemNode *child : currentNode->getChildren()) {
            if (child->getName() == part) {
                found = true;
                currentNode = child;
                break;
            }
        }
        if (!found) {
            return 0;
        }
    }
    if(currentNode->isDir())return 1;
    else return -1;
}

int Directory::addItem(const QString &path, bool isDir, QString fut, bool all) {
    QStringList parts = path.split('/');
    FileSystemNode *currentNode = root; // 从根节点开始（路径中不包含根节点）
    //从1开始，因为parts[0]是空字符
    for (int index = 1; index < parts.size(); ++index) { // 对路径进行循环查找
        QString part = parts[index]; // 要查找的目标

        if(index == parts.size() -1){//如果是最后一个节点
            // 看是否已存在
            for (FileSystemNode *child : currentNode->getChildren()) {
                if (child->getName() == part) {
                    return 0; // 已存在
                }
            }
            // 不存在，创建
            FileSystemNode *fileNode;
            if(isDir){
                fileNode = new FileSystemNode(part, isDir);
            }
            else{
                if(fut == "0@0")return -1;//如果没有给出fut，创建文件就会失败
                fileNode = new FileSystemNode(part, isDir, fut);
            }

            currentNode->addChild(fileNode);
            return 1; // 创建成功
        }

        bool found = false; // 标记是否找到匹配的节点
        // 在当前节点的子节点中查找匹配的部分并更新当前节点
        for (FileSystemNode *child : currentNode->getChildren()) {
            if (child->getName() == part) {
                found = true; // 找到匹配的节点
                currentNode = child; // 更新当前节点
                break; // 找到后退出循环
            }
        }

        // 如果没有找到节点
        if (!found) {
            if (all) {
                // 递归创建之前的文件夹
                for (int i = index; i < parts.size() - 1; ++i) {
                    FileSystemNode *fileNode = new FileSystemNode(parts[i], true);
                    currentNode->addChild(fileNode);
                    currentNode = fileNode; // 更新当前节点
                }
                // 创建文件/文件夹
                FileSystemNode *fileNode;
                if(isDir){
                    fileNode = new FileSystemNode(part, true);
                }
                else{
                    if(fut == "0@0")return -1;//如果没有给出fut，创建文件就会失败
                    fileNode = new FileSystemNode(part, false, fut);
                }
                currentNode->addChild(fileNode);
                return 1;
            }
            // 没有选择递归创建则创建失败
            else return -1;
        }

    }

    return -1; // 没有在循环最后一次完成就是失败
}

int Directory::delItem(const QString &path) {
    QStringList parts = path.split('/');
    FileSystemNode *currentNode = root;

    // 遍历到倒数第二层节点
    for (int index = 1; index < parts.size() - 1; ++index) {
        QString part = parts[index];
        bool found = false;

        for (FileSystemNode *child : currentNode->getChildren()) {
            if (child->getName() == part) {
                found = true;
                currentNode = child;
                break;
            }
        }

        if (!found) {
            return -1; // 找不到路径，删除失败
        }
    }

    // 删除
    QString targetName = parts.last();
    currentNode->removeChild(targetName);
    return 1; // 删除成功
}

bool Directory::renameItem(const QString &path, QString newName)
{
    QStringList parts = path.split('/');
    FileSystemNode *currentNode = root;
    for (int index = 1; index < parts.size(); ++index) {
        QString part = parts[index];
        bool found = false;

        for (FileSystemNode *child : currentNode->getChildren()) {
            if (child->getName() == part) {
                found = true;
                currentNode = child;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    currentNode->rename(newName);
    return true;

}

QVector<FileSystemNode> Directory::look(const QString &path, bool all) {
    QStringList parts = path.split('/');

    FileSystemNode *currentNode = root;

    // 从1开始，因为0是空的
    for (int i = 1;i<parts.size();i++) {
        bool found = false;

        for (FileSystemNode *child : currentNode->getChildren()) {
            if (child->getName() == parts[i]) {
                found = true;
                currentNode = child;
                break;
            }
        }

        if (!found) {
            return {}; // 找不到路径，返回空列表
        }
    }

    // 收集节点
    QVector<FileSystemNode> ret;
    collectNode(currentNode, ret, all);
    return ret;
}

bool Directory::saveToJson(const QString &filePath) const {
    // 获取目录路径
    QString dirPath = QFileInfo(filePath).absolutePath();
    QDir dir(dirPath);

    // 检查并创建目录
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "Failed to create directory:" << dirPath;
            return false;
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QJsonObject json = nodeToJson(root);
        QJsonDocument doc(json);
        file.write(doc.toJson());
        file.close();
        return true;
    }
    return false;
}

void Directory::loadFromJson(const QString &filePath) {
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"json文件夹打开成功";
        QByteArray jsonData = file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(jsonData));
        QJsonObject json = doc.object();
        deleteNode(root); // 清空现有树
        root = new FileSystemNode("root", true); // 重新初始化根节点
        jsonToNode(json, root, true); // 解析 JSON 并构建树
        file.close();
    }
    else qDebug()<<"json文件夹打开失败";
}

//如果是文件夹，则找文件夹里面的，不然返回node本身
void Directory::collectNode(FileSystemNode *node, QVector<FileSystemNode> &ret, bool all)
{
    if(!node->isDir()){
        ret.append(*node);
        return;
    }

    for (FileSystemNode *child : node->getChildren()) {
        ret.append(*child);
        if (all&&child->isDir()) {
            collectNode(child, ret, true); // 递归收集
        }
    }

}

void Directory::deleteNode(FileSystemNode *node) {
    for (FileSystemNode *child : node->getChildren()) {
        deleteNode(child);
    }
    delete node; // 释放内存
}

QJsonObject Directory::nodeToJson(FileSystemNode *node) const {
    QJsonObject json;
    json["name"] = node->getName();
    json["FUT"] = node->getFUT();
    json["isDirectory"] = node->isDir();
    if (!node->isDir()) return json;

    QJsonArray childrenArray;
    for (FileSystemNode *child : node->getChildren()) {
        childrenArray.append(nodeToJson(child));
    }
    json["children"] = childrenArray;

    return json;
}

void Directory::jsonToNode(const QJsonObject &json, FileSystemNode *node, bool isRoot) {
    QString name = json["name"].toString();
    QString FUT = json["FUT"].toString();
    bool isDirectory = json["isDirectory"].toBool();
    FileSystemNode *newNode;

    if (isRoot) {
        newNode = node;
    } else {
        newNode = new FileSystemNode(name, isDirectory, FUT);
        node->addChild(newNode);
    }

    if (isDirectory && json.contains("children")) {
        QJsonArray childrenArray = json["children"].toArray();
        for (const QJsonValue &value : childrenArray) {
            jsonToNode(value.toObject(), newNode);
        }
    }
}
