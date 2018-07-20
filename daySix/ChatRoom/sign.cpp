#include "sign.h"
#include "drawer.h"
#include <QString>
#include <QMessageBox>
#include <QByteArray>
#include <iostream>

Sign::Sign(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Register or Log in"));
    resize(500, 350);
    contentListWidget = new QListWidget;
    registerBtn = new QPushButton(tr("Register"));
    LogInBtn = new QPushButton(tr("Log In"));
    connectBtn = new QPushButton(tr("Connect"));
    nameLabel = new QLabel(tr("Name:"));
    passwordLabel = new QLabel(tr("Password:"));
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    tcpSocket = new QTcpSocket(this);


    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(contentListWidget, 0, 0, 1, 3);
    mainLayout->addWidget(nameLabel, 1, 0);
    mainLayout->addWidget(nameLineEdit, 1, 1, 1, 2);
    mainLayout->addWidget(passwordLabel, 2, 0);
    mainLayout->addWidget(passwordLineEdit, 2, 1, 1, 2);
    mainLayout->addWidget(registerBtn, 3, 0);
    mainLayout->addWidget(LogInBtn, 3, 1);
    mainLayout->addWidget(connectBtn, 3, 2);

    registerBtn->setEnabled(false);
    LogInBtn->setEnabled(false);

    connect(connectBtn, SIGNAL(clicked()), this, SLOT(slotConnect()));
    connect(registerBtn, SIGNAL(clicked()), this, SLOT(slotRegister()));
    connect(LogInBtn, SIGNAL(clicked()), this, SLOT(slotLogIn()));
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
    connect(tcpSocket, SIGNAL(connected()), this, SLOT(connected()));
}

Sign::~Sign()
{
    tcpSocket->disconnectFromHost();
}

void Sign::slotConnect()
{
    if(nameLineEdit->text() == "" || passwordLineEdit->text() == "")
    {
        QMessageBox::warning(this, tr("error"), tr("Name or Password is empty!"));
        return;
    }

    //连接服务器
    port = 12345;
    serverIP = new QHostAddress();
    QString ip = "127.0.0.1";
    serverIP->setAddress(ip);
    tcpSocket->connectToHost(*serverIP, port);
}

void Sign::connected()
{
    connectBtn->setEnabled(false);
    registerBtn->setEnabled(true);
    LogInBtn->setEnabled(true);
}


void Sign::slotRegister()
{
    QString msg = tr(REGISTER);
    username = nameLineEdit->text();
    QString password = passwordLineEdit->text();
    msg += username + '\t' + password;
    int length = tcpSocket->write(msg.toLatin1(), msg.length());
    if(length != msg.length())
    {
        return;
    }
}

void Sign::slotLogIn()
{
    QString msg = tr(LOGIN);
    username = nameLineEdit->text();
    QString password = passwordLineEdit->text();
    msg += username + '\t' + password;
    int length = tcpSocket->write(msg.toLatin1(), msg.length());
    if(length != msg.length())
    {
        return;
    }
}

void Sign::dataReceived()
{
    QByteArray datagram;
    datagram.resize(tcpSocket->bytesAvailable());
    tcpSocket->read(datagram.data(), datagram.size());
    QString msg = datagram.data();
    contentListWidget->addItem(msg.left(datagram.size()));
    contentListWidget->scrollToBottom(); //自动回滚信息


    //如果登录成功就可以进入主界面
    if(msg == "LOG IN SUCCESS")
    {
        QMessageBox::information(this, tr("log in success"), tr("log in success"));
        this->hide();

        drawer = new Drawer();
        drawer->toolBtn1->setText(username);
        drawer->resize(250, 500);
        drawer->show();
    }
    else if(msg == "REGISTER SUCCESS")
    {
        QMessageBox::information(this, tr("register success, now you can log in"), tr("register success, now you can log in"));
    }
}
