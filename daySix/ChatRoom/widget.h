#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QUdpSocket;

namespace Ui {
class Widget;
}

//Msg:聊天信息，UsrEnter:新用户进入，UsrLeft:用户退出，FileName:文件名，Refuse:拒绝接收文件
enum MsgType{Msg, UsrEnter, UsrLeft, FileName, Refuse};

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
};

#endif // WIDGET_H
