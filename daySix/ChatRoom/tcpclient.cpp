#include "tcpclient.h"
#include <QMessageBox>
#include <QHostInfo>

TcpClient::TcpClient(QWidget *parent,Qt::WindowFlags f)
    : QDialog(parent,f)
{
    setWindowTitle(tr("TCP Client"));
    resize(500, 300);
    contentListWidget = new QListWidget;
    sendLineEdit = new QLineEdit;
    sendBtn = new QPushButton(tr("Send"));
    clearBtn = new QPushButton(tr("Clear"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget,0,0,1,3);
    mainLayout->addWidget(sendLineEdit,1,0);
    mainLayout->addWidget(sendBtn,1,1);
    mainLayout->addWidget(clearBtn,1,2);
    port = 12345;
    IP = "127.0.0.1";
    serverIP = new QHostAddress();
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(slotSend()));
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(slotClear()));

    if(!serverIP->setAddress(IP))
    {
        QMessageBox::information(this,tr("error"),tr("server ip address error!"));
        return;
    }
    /* 创建了一个QTcpSocket类对象，并将信号/槽连接起来 */
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(connected()),this,SLOT (slotConnected()));
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT (dataReceived()));
    tcpSocket->connectToHost(*serverIP ,port);
}

TcpClient::~TcpClient()
{

}


void TcpClient::slotConnected()
{
    sendBtn->setEnabled(true);
    int length=0;
    QString msg = username + ": Enter Chat Room";
    if((length = tcpSocket->write(msg.toLatin1(),msg. length())) != msg.length())
    {
        return;
    }
}

void TcpClient::slotSend()
{
    if(sendLineEdit->text() == "")
    {
        return;
    }
    QString msg = username + ": " + sendLineEdit->text();
    tcpSocket->write(msg.toLatin1(), msg.length());
    sendLineEdit->clear();
}

void TcpClient::slotClear()
{
    contentListWidget->clear();
}

void TcpClient::dataReceived()
{
    while(tcpSocket->bytesAvailable() > 0)
    {
        QByteArray datagram;
        datagram.resize(tcpSocket->bytesAvailable());
        tcpSocket->read(datagram.data(),datagram.size());
        QString msg = datagram.data();
        contentListWidget->addItem(msg.left(datagram.size()));
        contentListWidget->scrollToBottom();
    }
}
