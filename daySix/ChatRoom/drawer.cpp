#include "drawer.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <iostream>

Drawer::Drawer(QWidget *parent, Qt::WindowFlags f)
    :QToolBox(parent, f)
{
    setWindowTitle(tr("Chat Room"));
    setWindowIcon(QPixmap(":/images/ChatRoom.jpg"));

    toolBtn1 = new QToolButton;
    //toolBtn1->setText(username);
    toolBtn1->setIcon(QPixmap(":/images/liu.png"));
    toolBtn1->setIconSize(QPixmap(":/images/liu.png").size());
    toolBtn1->setAutoRaise(true);
    toolBtn1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolBtn1, SIGNAL(clicked()), this, SLOT(showChatWidget1()));
/*
    toolBtn2 = new QToolButton;
    toolBtn2->setText(tr("Linux"));
    toolBtn2->setIcon(QPixmap(":/images/Linux.jpg"));
    toolBtn2->setIconSize(QPixmap(":/images/liu.png").size());
    toolBtn2->setAutoRaise(true);
    toolBtn2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(toolBtn2, SIGNAL(clicked()), this, SLOT(showChatWidget2()));
*/
    QGroupBox *groupBox = new QGroupBox;
    QVBoxLayout *layout = new QVBoxLayout(groupBox);

    layout->setMargin(20);
    layout->setAlignment(Qt::AlignLeft);
    layout->addWidget(toolBtn1);
    //layout->addWidget(toolBtn2);
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
/*
void Drawer::showChatWidget2()
{
    chat2 = new TcpClient();
    chat2->username = toolBtn2->text();
    chat2->setWindowTitle(toolBtn2->text());
    chat2->show();
}
*/
