#ifndef MYTOOLS_H
#define MYTOOLS_H
#include <QFile>
#include <QByteArray>
#include <QDebug>


//将一个文件转换为字节流，传输小文件的时候可以使用
QByteArray mytoolReadFileToByteArray(const QString &filePath);



bool mytoolSaveByteArrayToFile(const QString &filePath, const QByteArray &byteArray);



#endif // MYTOOLS_H
