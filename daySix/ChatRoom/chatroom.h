#ifndef CHATROOM_H
#define CHATROOM_H

#include <QDialog>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHostAddress>
#include <QTcpSocket>

#include "command.h"

class ChatRoom : public QDialog
{
    Q_OBJECT

public:
    ChatRoom(QWidget *parent = 0,Qt::WindowFlags f=0);
    ~ChatRoom();

private:
    QListWidget *contentListWidget;
    QLineEdit *sendLineEdit;
    QPushButton *sendBtn;
    QPushButton *clearBtn;
    QPushButton *enterBtn;
    QGridLayout *mainLayout;
    bool status;
    int port;
    QString IP;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;

public slots:
    void slotEnter();
    void slotConnected();
    void slotDisconnected();
    void dataReceived();
    void slotSend();
    void slotClear();
};


#endif // CHATROOM_H
