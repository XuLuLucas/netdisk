/********************************************************************************
** Form generated from reading UI file 'tcpserver.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPSERVER_H
#define UI_TCPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Tcpserver
{
public:

    void setupUi(QWidget *Tcpserver)
    {
        if (Tcpserver->objectName().isEmpty())
            Tcpserver->setObjectName("Tcpserver");
        Tcpserver->resize(239, 132);

        retranslateUi(Tcpserver);

        QMetaObject::connectSlotsByName(Tcpserver);
    } // setupUi

    void retranslateUi(QWidget *Tcpserver)
    {
        Tcpserver->setWindowTitle(QCoreApplication::translate("Tcpserver", "Tcpserver", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Tcpserver: public Ui_Tcpserver {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPSERVER_H
