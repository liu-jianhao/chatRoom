#include "dialog.h"
#include <QString>
#include <QByteArray>

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("communication"));
    contentListWidget = new QListWidget;
    signINBtn = new QPushButton(tr("sign in"));
    signUpBtn = new QPushButton(tr("sign up"));
    disconnectBtn = new QPushButton(tr("disconect"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget, 0, 0, 1, 3);
    mainLayout->addWidget(signINBtn, 1, 0);
    mainLayout->addWidget(signUpBtn, 1, 1);
    mainLayout->addWidget(disconnectBtn, 1, 2);

    connect(signINBtn, SIGNAL(clicked()), this, SLOT(slotSignin()));
    connect(signUpBtn, SIGNAL(clicked()), this, SLOT(slotSignup()));

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(disconnectBtn, SIGNAL(clicked()), this, SLOT(slotDisconnect()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    port = 12345;
    serverIP = new QHostAddress();
    QString ip = "127.0.0.1";
    serverIP->setAddress(ip);
    tcpSocket->connectToHost(*serverIP, port);
}

Dialog::~Dialog()
{
}

void Dialog::slotSignin()
{
    QString msg = tr("sign in");
    int length = tcpSocket->write(msg.toLatin1(), msg.length());
    if(length != msg.length())
    {
        return;
    }
}


void Dialog::slotDisconnect()
{
    tcpSocket->disconnectFromHost();
}

void Dialog::slotSignup()
{
    QString msg = tr("sign up");
    int length = tcpSocket->write(msg.toLatin1(), msg.length());
    if(length != msg.length())
    {
        return;
    }
}

void Dialog::dataReceived()
{
    QByteArray datagram;
    datagram.resize(tcpSocket->bytesAvailable());
    tcpSocket->read(datagram.data(), datagram.size());
    QString msg = datagram.data();
    contentListWidget->addItem(msg.left(datagram.size()));
    contentListWidget->scrollToBottom(); //自动回滚信息
}
