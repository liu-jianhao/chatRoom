#ifndef DRAWER_H
#define DRAWER_H

#include <QToolButton>
#include <QToolBox>
#include "tcpclient.h"

class Drawer : public QToolBox
{
    Q_OBJECT
public:
    Drawer(QWidget *parent = 0, Qt::WindowFlags f = 0);

    QString username;
    QToolButton *toolBtn1;

private slots:
    void showChatWidget1();
    //void showChatWidget2();


private:
    //QToolButton *toolBtn2;

    TcpClient *chat1;
    //TcpClient *chat2;
};

#endif // DRAWER_H
