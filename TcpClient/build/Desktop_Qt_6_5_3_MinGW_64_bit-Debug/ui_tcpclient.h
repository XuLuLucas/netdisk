/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TCPclient
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLineEdit *name_le;
    QLineEdit *pwd_le;
    QLabel *name_label;
    QLabel *pwd_label;
    QPushButton *login_pd;
    QHBoxLayout *horizontalLayout;
    QPushButton *register_pd;
    QSpacerItem *horizontalSpacer;
    QPushButton *logout_pd;

    void setupUi(QWidget *TCPclient)
    {
        if (TCPclient->objectName().isEmpty())
            TCPclient->setObjectName("TCPclient");
        TCPclient->resize(490, 204);
        verticalLayout_2 = new QVBoxLayout(TCPclient);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        name_le = new QLineEdit(TCPclient);
        name_le->setObjectName("name_le");
        QFont font;
        font.setPointSize(20);
        name_le->setFont(font);

        gridLayout->addWidget(name_le, 0, 1, 1, 1);

        pwd_le = new QLineEdit(TCPclient);
        pwd_le->setObjectName("pwd_le");
        pwd_le->setFont(font);
        pwd_le->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(pwd_le, 1, 1, 1, 1);

        name_label = new QLabel(TCPclient);
        name_label->setObjectName("name_label");
        name_label->setFont(font);

        gridLayout->addWidget(name_label, 0, 0, 1, 1);

        pwd_label = new QLabel(TCPclient);
        pwd_label->setObjectName("pwd_label");
        pwd_label->setFont(font);

        gridLayout->addWidget(pwd_label, 1, 0, 1, 1);

        login_pd = new QPushButton(TCPclient);
        login_pd->setObjectName("login_pd");
        QFont font1;
        font1.setPointSize(24);
        login_pd->setFont(font1);

        gridLayout->addWidget(login_pd, 2, 0, 1, 2);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        register_pd = new QPushButton(TCPclient);
        register_pd->setObjectName("register_pd");
        QFont font2;
        font2.setPointSize(16);
        register_pd->setFont(font2);

        horizontalLayout->addWidget(register_pd);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        logout_pd = new QPushButton(TCPclient);
        logout_pd->setObjectName("logout_pd");
        logout_pd->setFont(font2);

        horizontalLayout->addWidget(logout_pd);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(TCPclient);

        QMetaObject::connectSlotsByName(TCPclient);
    } // setupUi

    void retranslateUi(QWidget *TCPclient)
    {
        TCPclient->setWindowTitle(QCoreApplication::translate("TCPclient", "TCPclient", nullptr));
        name_label->setText(QCoreApplication::translate("TCPclient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pwd_label->setText(QCoreApplication::translate("TCPclient", "<html><head/><body><p>\345\257\206\347\240\201\357\274\232</p></body></html>", nullptr));
        login_pd->setText(QCoreApplication::translate("TCPclient", "\347\231\273\351\231\206", nullptr));
        register_pd->setText(QCoreApplication::translate("TCPclient", "\346\263\250\345\206\214", nullptr));
        logout_pd->setText(QCoreApplication::translate("TCPclient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TCPclient: public Ui_TCPclient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
