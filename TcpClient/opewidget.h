#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include<QListWidget>
#include"friend.h"
#include"book.h"
#include<QStackedWidget>//堆栈窗口
class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();
    Friend *getFriend();
    book *getBook();

private:
    QListWidget *m_plistW;
    Friend *m_pFriend;
    book *m_pBook;
    QStackedWidget *m_pSW;
signals:

};

#endif // OPEWIDGET_H
