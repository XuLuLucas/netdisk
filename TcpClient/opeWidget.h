#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>

#include "friend.h"
#include "book.h"
#include <QStackedWidget>



class opeWidget : public QWidget
{   Q_OBJECT

public:
    static opeWidget &getInstance();

    Friend *m_pFriend;
    book *m_pBook;

private:
    explicit opeWidget(QWidget *parent = nullptr);

    //———————页面组件————————
    QListWidget  *m_pListW;
    QStackedWidget *m_pSW;
    //——————————————————————

signals:

public slots:


};

#endif // OPEWIDGET_H
