#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "command.h"

class QUdpSocket;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent, QString usrname);
    ~Widget();

protected:
    void usrEnter(QString, QString);
    void usrLeft(QString, QString);
    void sndMsg(MsgType type, QString srvaddr = "");
    QString getIP();
    QString getMsg();
    QString getUsr();

private:
    Ui::Widget *ui;

    QUdpSocket *udpSocket;
    qint16 port;
    QString uName;

private slots:
    void processPendingDatagrams();
    void on_sendBtn_clicked();
    void on_exitBtn_clicked();
};

#endif // WIDGET_H
