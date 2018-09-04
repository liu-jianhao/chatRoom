#ifndef DRAWER_H
#define DRAWER_H

#include <QToolButton>
#include <QPushButton>
#include <QToolBox>
#include "tcpclient.h"
#include "addfriends.h"

class Drawer : public QToolBox
{
    Q_OBJECT
public:
    Drawer(QWidget *parent = 0, Qt::WindowFlags f = 0);

    QString username;
    QToolButton *toolBtn1;
    QPushButton *addFriendsBtn;

private slots:
    void showChatWidget1();
    void slotAddFriends();

private:
    TcpClient *chat1;
    AddFriends *addFriends;
};

#endif // DRAWER_H
