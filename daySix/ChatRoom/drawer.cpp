#include "drawer.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <iostream>

Drawer::Drawer(QWidget *parent, Qt::WindowFlags f)
    :QToolBox(parent, f)
{
    setWindowTitle(tr("Chat Room"));
    setWindowIcon(QPixmap(":/images/ChatRoom.jpg"));

    addFriendsBtn = new QPushButton(tr("ADD"));
    connect(addFriendsBtn, SIGNAL(clicked()), this, SLOT(slotAddFriends()));

    toolBtn1 = new QToolButton;
    toolBtn1->setIcon(QPixmap(":/images/liu.png"));
    toolBtn1->setIconSize(QPixmap(":/images/liu.png").size());
    toolBtn1->setAutoRaise(true);
    toolBtn1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolBtn1, SIGNAL(clicked()), this, SLOT(showChatWidget1()));

    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    layout->setMargin(20);
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(toolBtn1);
    layout->addWidget(addFriendsBtn);
    layout->addStretch(); //插入一个占位符
    this->addItem((QWidget*)groupBox, tr("Member"));
}

void Drawer::showChatWidget1()
{
    chat1 = new TcpClient();
    chat1->username = toolBtn1->text();
    chat1->setWindowTitle(toolBtn1->text());
    chat1->show();
}

void Drawer::slotAddFriends()
{
    addFriends = new AddFriends();
    addFriends->show();
}
