#include "tcpclient.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    TCPclient::getInstance().show();

    return a.exec();
}
