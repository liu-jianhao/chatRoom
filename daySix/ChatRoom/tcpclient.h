#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHostAddress>
#include <QTcpSocket>

class TcpClient : public QDialog
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = 0,Qt::WindowFlags f=0);
    ~TcpClient();
    QString username;

private:
    QListWidget *contentListWidget;
    QLineEdit *sendLineEdit;
    QPushButton *sendBtn;
    QPushButton *clearBtn;
    QGridLayout *mainLayout;
    int port;
    QString IP;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;

public slots:
    void slotConnected();
    void dataReceived();
    void slotSend();
    void slotClear();
};

#endif // TCPCLIENT_H
