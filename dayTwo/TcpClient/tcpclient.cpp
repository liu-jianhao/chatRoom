#include "tcpclient.h"
#include <QMessageBox>
#include <QHostInfo>

TcpClient::TcpClient(QWidget *parent,Qt::WindowFlags f)
    : QDialog(parent,f)
{
    setWindowTitle(tr("TCP Client"));
    contentListWidget = new QListWidget;
    sendLineEdit = new QLineEdit;
    sendBtn = new QPushButton(tr("Send"));
    clearBtn = new QPushButton(tr("Clear"));
    userNameLabel = new QLabel(tr("Username:"));
    userNameLineEdit = new QLineEdit;
    serverIPLabel = new QLabel(tr("Server IP Address:"));
    serverIPLineEdit = new QLineEdit;
    portLabel = new QLabel(tr("Port:"));
    portLineEdit = new QLineEdit;
    enterBtn= new QPushButton(tr("Enter Chat Room"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget,0,0,1,3);
    mainLayout->addWidget(sendLineEdit,1,0);
    mainLayout->addWidget(sendBtn,1,1);
    mainLayout->addWidget(clearBtn,1,2);
    mainLayout->addWidget(userNameLabel,2,0);
    mainLayout->addWidget(userNameLineEdit,2,1,1,2);
    mainLayout->addWidget(serverIPLabel,3,0);
    mainLayout->addWidget(serverIPLineEdit,3,1,1,2);
    mainLayout->addWidget(portLabel,4,0);
    mainLayout->addWidget(portLineEdit,4,1,1,2);
    mainLayout->addWidget(enterBtn,5,0,1,3);
    status = false;
    port = 12345;
    portLineEdit->setText(QString::number(port));
    IP = "127.0.0.1";
    serverIPLineEdit->setText(IP);
    serverIP = new QHostAddress();
    connect(enterBtn,SIGNAL(clicked()),this,SLOT(slotEnter()));
    connect(sendBtn,SIGNAL(clicked()),this,SLOT(slotSend()));
    connect(clearBtn,SIGNAL(clicked()),this,SLOT(slotClear()));
    sendBtn->setEnabled(false);
}

void TcpClient::slotEnter()
{
    if(!status)
    {
        /* 完成输入合法性检验 */
        QString ip = serverIPLineEdit->text();
        if(!serverIP->setAddress(ip))
        {
            QMessageBox::information(this,tr("error"),tr("server ip address error!"));
            return;
        }
        if(userNameLineEdit->text()=="")
        {
            QMessageBox::information(this,tr("error"),tr("User name error!"));
            return;
        }
        userName = userNameLineEdit->text();
        /* 创建了一个QTcpSocket类对象，并将信号/槽连接起来 */
        tcpSocket = new QTcpSocket(this);
        connect(tcpSocket,SIGNAL(connected()),this,SLOT (slotConnected()));
        connect(tcpSocket,SIGNAL(disconnected()),this,SLOT (slotDisconnected ()));
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT (dataReceived()));
        tcpSocket->connectToHost(*serverIP,port);
        status=true;
    }
    else
    {
        int length=0;
        QString msg = userName + tr(":Leave Chat Room");
        if((length=tcpSocket->write(msg.toLatin1(),msg. length()))!=msg. length())
        {
            return;
        }
        tcpSocket->disconnectFromHost();
        status=false;                                   //将status状态复位
    }
}

void TcpClient::slotConnected()
{
    sendBtn->setEnabled(true);
    enterBtn->setText(tr("leave"));
    int length=0;
    QString msg=userName+tr(":Enter Chat Room");
    if((length=tcpSocket->write(msg.toLatin1(),msg. length()))!=msg.length())
    {
        return;
    }
}

void TcpClient::slotSend()
{
    if(sendLineEdit->text()=="")
    {
        return ;
    }
    //QString msg=userName+":"+sendLineEdit->text();
    QString msg = sendLineEdit->text();
    tcpSocket->write(msg.toLatin1(), msg.length());
    sendLineEdit->clear();
}

void TcpClient::slotDisconnected()
{
    sendBtn->setEnabled(false);
    enterBtn->setText(tr("Enter Chat Room"));
}

void TcpClient::slotClear()
{
    contentListWidget->clear();
}

void TcpClient::dataReceived()
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

TcpClient::~TcpClient()
{
}
