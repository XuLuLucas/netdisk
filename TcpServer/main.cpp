#include "tcpserver.h"
#include <QApplication>
#include <QTimer>
#include <QThread>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Tcpserver w;
    w.show();

    return a.exec();
}
