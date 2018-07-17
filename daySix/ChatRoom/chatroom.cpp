#include "chatroom.h"
#include <QMessageBox>
#include <QHostInfo>

ChatRoom::ChatRoom(QWidget *parent,Qt::WindowFlags f)
    : QDialog(parent,f)
{
    setWindowTitle(tr("TCP Client"));
    contentListWidget = new QListWidget;
    sendLineEdit = new QLineEdit;
    sendBtn = new QPushButton(tr("Send"));
    clearBtn = new QPushButton(tr("Clear"));
    enterBtn= new QPushButton(tr("Enter Chat Room"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget,0,0,1,3);
    mainLayout->addWidget(sendLineEdit,1,0);
    mainLayout->addWidget(sendBtn,1,1);
    mainLayout->addWidget(clearBtn,1,2);
    mainLayout->addWidget(enterBtn,2,0,1,3);


    status = false;
    //连接服务器
    port = 12345;
    IP = "172.29.1.82";
    serverIP = new QHostAddress();
    connect(enterBtn, SIGNAL(clicked()), this, SLOT(slotEnter()));
    connect(sendBtn, SIGNAL(clicked()), this, SLOT(slotSend()));
    connect(clearBtn, SIGNAL(clicked()), this, SLOT(slotClear()));
    sendBtn->setEnabled(false);
}

ChatRoom::~ChatRoom()
{
}

void ChatRoom::slotEnter()
{
    if(!status)
    {
        /* 完成输入合法性检验 */
        if(!serverIP->setAddress(IP))
        {
            QMessageBox::information(this,tr("error"),tr("server ip address error!"));
            return;
        }

        /* 创建了一个QTcpSocket类对象，并将信号/槽连接起来 */
        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket, SIGNAL(connected()), this, SLOT (slotConnected()));
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT (slotDisconnected ()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
        tcpSocket->connectToHost(*serverIP, port);
        status = true;
        sendBtn->setEnabled(true);
    }
    else
    {
        int length = 0;
        QString msg = tr(" :Leave Chat Room");
        if((length = tcpSocket->write(msg.toLatin1(),msg. length()))!=msg. length())
        {
            return;
        }
        tcpSocket->disconnectFromHost();
        status = false;                                   //将status状态复位
    }
}

void ChatRoom::slotConnected()
{
    enterBtn->setText(tr("leave"));
    int length = 0;
    QString msg = tr(" :Enter Chat Room");
    if((length = tcpSocket->write(msg.toLatin1(), msg.length())) != msg.length())
    {
        return;
    }
}

void ChatRoom::slotSend()
{
    if(sendLineEdit->text() == "")
    {
        return;
    }
    QString msg = sendLineEdit->text();
    tcpSocket->write(msg.toLatin1(), msg.length());
    sendLineEdit->clear();
}

void ChatRoom::slotDisconnected()
{
    sendBtn->setEnabled(false);
    enterBtn->setText(tr("Enter Chat Room"));
}

void ChatRoom::slotClear()
{
    contentListWidget->clear();
}

void ChatRoom::dataReceived()
{
    while(tcpSocket->bytesAvailable()>0)
    {
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());
        tcpSocket->read(datagram.data(),datagram.size());
        QString msg=datagram.data();
        contentListWidget->addItem(msg.left(datagram.size()));
        contentListWidget->scrollToBottom();
    }
}
