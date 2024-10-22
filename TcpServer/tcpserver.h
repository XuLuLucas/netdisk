#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Tcpserver;
}
QT_END_NAMESPACE

class Tcpserver : public QWidget
{
    Q_OBJECT
public:
    Tcpserver(QWidget *parent = nullptr);
    ~Tcpserver();

    void loadConfig();

private:
    Ui::Tcpserver *ui;
    QString m_strIP;
    quint16 m_usPort;
};
#endif // TCPSERVER_H
