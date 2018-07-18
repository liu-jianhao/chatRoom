#include "widget.h"
#include "ui_widget.h"
#include <QUdpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QNetworkInterface>
#include <QProcess>

Widget::Widget(QWidget *parent, QString usrname) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    uName = usrname;
    port = 12345;
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(ui->sendBtn, SIGNAL(clicked()), this, SLOT(on_sendBtn_clicked()));
    sndMsg(UsrEnter);
}

Widget::~Widget()
{
    delete ui;
}


void Widget::sndMsg(MsgType type, QString srvaddr)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString address = getIP();
    out << type << getUsr();
    switch(type)
    {
    case Msg:
        if(ui->msgTxtEdit->toPlainText() == "")
        {
            return;
        }
        out << address << getMsg();
        ui->msgBrowser->verticalScrollBar()->setValue(ui->msgBrowser->verticalScrollBar()->maximum());
        break;
    case UsrEnter:
        out << address;
        break;
    case UsrLeft:
        break;
    case FileName: {
        break;
    }
    case Refuse:
        break;
    }

    udpSocket->writeDatagram(data, data.length(), QHostAddress::Broadcast, port);
}


void Widget::processPendingDatagrams()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        //获取当前可供读取的UDP报文大小，并据此大小分配接收缓冲区
        datagram.resize(udpSocket->pendingDatagramSize());

        udpSocket->readDatagram(datagram.data(), datagram.size());
        QDataStream in(&datagram, QIODevice::ReadOnly);
        int msgType;
        in >> msgType;
        QString usrName, ipAddr, msg;
        QString time = QDateTime::currentDateTime().toString("yyy-MM-dd hh:mm:ss");

        switch(msgType)
        {
        case Msg:
            in >> usrName >> ipAddr >> msg;
            ui->msgBrowser->setTextColor(Qt::blue);
            ui->msgBrowser->setCurrentFont(QFont("Times New Roman", 12));
            ui->msgBrowser->append(" [ " + usrName + " ] " + time);
            ui->msgBrowser->append(msg);
            break;
        case UsrEnter:
            in >> usrName >> ipAddr;
            usrEnter(usrName, ipAddr);
            break;
        case UsrLeft:
            in >> usrName;
            usrLeft(usrName, time);
            break;
        case FileName: {
            break;
        }
        case Refuse:
            break;
        }
    }
}

void Widget::usrEnter(QString usrname, QString ipaddr)
{
    //判断是否已经在用户列表中
    bool isEmpty = ui->usrTblWidget->findItems(usrname, Qt::MatchExactly).isEmpty();
    if(isEmpty)
    {
        QTableWidgetItem *usr = new QTableWidgetItem(usrname);
        QTableWidgetItem *ip = new QTableWidgetItem(ipaddr);
        ui->usrTblWidget->insertRow(0);
        ui->usrTblWidget->setItem(0, 0, usr);
        ui->usrTblWidget->setItem(0, 1, ip);
        ui->msgBrowser->setTextColor(Qt::gray);
        ui->msgBrowser->setCurrentFont(QFont("Times New Roman", 10));
        ui->msgBrowser->append(tr("%1 online").arg(usrname));
        ui->usrNumLbl->setText(tr("%1 online people").arg(ui->usrTblWidget->rowCount()));
        sndMsg(UsrEnter);
    }
}

void Widget::usrLeft(QString usrname, QString time)
{
    int rowNum = ui->usrTblWidget->findItems(usrname, Qt::MatchExactly).first()->row();
    ui->usrTblWidget->removeRow(rowNum);
    ui->msgBrowser->setTextColor(Qt::gray);
    ui->msgBrowser->setCurrentFont(QFont("Times New Roman", 10));
    ui->msgBrowser->append(tr("%1 left at %2").arg(usrname).arg(time));
    ui->usrNumLbl->setText(tr("%1 online people").arg(ui->usrTblWidget->rowCount()));
}


QString Widget::getIP()
{
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress addr, list) {
        if(addr.protocol() == QAbstractSocket::IPv4Protocol)
            return addr.toString();
    }
    return 0;
}

QString Widget::getUsr()
{
    return uName;
}

QString Widget::getMsg()
{
    QString msg = ui->msgTxtEdit->toHtml();
    ui->msgTxtEdit->clear();
    ui->msgTxtEdit->setFocus();
    return msg;
}

void Widget::on_sendBtn_clicked()
{
    sndMsg(Msg);
}

void Widget::on_exitBtn_clicked()
{
    close();
}
