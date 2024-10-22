#include "mytools.h"
#include <QDir>

QByteArray mytoolReadFileToByteArray(const QString &filePath) {
    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件:" << file.errorString();
        return QByteArray();
    }

    // 读取文件内容到字节数组
    QByteArray byteArray = file.readAll();

    // 关闭文件
    file.close();

    return byteArray;
}

bool mytoolSaveByteArrayToFile(const QString &filePath, const QByteArray &byteArray) {
    // 获取目录路径
    QString dirPath = QFileInfo(filePath).absolutePath();
    QDir dir(dirPath);

    // 如果目录不存在，则创建它
    if (!dir.exists() && !dir.mkpath(dirPath)) {
        qWarning() << "无法创建目录:" << dirPath;
        return false;
    }

    QFile file(filePath);

    // 尝试打开文件
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "无法打开文件:" << file.errorString();
        return false;
    }

    // 写入字节数组到文件
    qint64 bytesWritten = file.write(byteArray);

    // 关闭文件
    file.close();

    // 检查写入的字节数是否与字节数组的大小相同
    return bytesWritten == byteArray.size();
}
