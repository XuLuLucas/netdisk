#include "opeWidget.h"

opeWidget &opeWidget::getInstance()
{
    static opeWidget instance;
    return instance;
}

opeWidget::opeWidget(QWidget *parent): QWidget{parent}
{
    //——————————构建页面——————————————
    m_pListW = new QListWidget(this);
    m_pListW->addItem("好友");
    m_pListW->addItem("文件");

    m_pFriend = new Friend;
    m_pBook = new book;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pListW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    //——————————信号与槽———————————————

    connect(m_pListW, SIGNAL(currentRowChanged(int)),
            m_pSW, SLOT(setCurrentIndex(int)));

}
