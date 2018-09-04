#include "addfriends.h"
#include "command.h"

#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include <iostream>

AddFriends::AddFriends(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Add Friends"));
    resize(200,100);

    nameLabel = new QLabel(tr("Name:"));
    nameLineEdit = new QLineEdit();
    enterBtn = new QPushButton("Enter");

    tcpSocket = new QTcpSocket(this);

    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(nameLabel, 0, 0);
    mainLayout->addWidget(nameLineEdit, 0, 1);
    mainLayout->addWidget(enterBtn, 1, 1);

    //连接服务器
    port = 12345;
    serverIP = new QHostAddress();
    QString ip = "127.0.0.1";
    serverIP->setAddress(ip);
    tcpSocket->connectToHost(*serverIP, port);

    connect(enterBtn, SIGNAL(clicked()), this, SLOT(slotEnter()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}


AddFriends::~AddFriends()
{
    tcpSocket->disconnectFromHost();
}

void AddFriends::slotEnter()
{
    if(nameLineEdit->text() == "")
    {
        QMessageBox::warning(this, tr("Name is empty"), tr("Name is empty"));
        return;
    }
    QString msg = tr(ADDFRIENDS);
    msg += nameLineEdit->text();
    tcpSocket->write(msg.toLatin1(), msg.length());
}

void AddFriends::dataReceived()
{
    QByteArray datagram;
    datagram.resize(tcpSocket->bytesAvailable());
    tcpSocket->read(datagram.data(), datagram.size());
    QString msg = datagram.data();

    //如果登录成功就可以进入主界面
    if(msg == "The name is wrong")
    {
        QMessageBox::warning(this, tr("The name is wrong"), tr("The name is wrong"));
    }
    else if(msg == "message has been sended")
    {
        QMessageBox::information(this, tr("message has been sended"), tr("message has been sended"));
    }
}
