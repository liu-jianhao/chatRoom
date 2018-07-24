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


private:

    TcpClient *chat1;
};

#endif // DRAWER_H
