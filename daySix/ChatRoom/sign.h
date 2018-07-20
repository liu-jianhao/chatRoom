#ifndef SIGN_H
#define SIGN_H


#include <QDialog>
#include <QPushButton>
#include <QTcpSocket>
#include <QGridLayout>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QHostAddress>

#include "command.h"
#include "drawer.h"

class Sign : public QDialog
{
    Q_OBJECT

public:
    Sign(QWidget *parent = 0);
    ~Sign();

private:
    QListWidget *contentListWidget;
    QGridLayout *mainLayout;
    QLabel *nameLabel;
    QLabel *passwordLabel;
    QPushButton *connectBtn;
    QLineEdit *nameLineEdit;
    QLineEdit *passwordLineEdit;
    QPushButton *registerBtn;
    QPushButton *LogInBtn;

    int port;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;

    QString username;

    Drawer *drawer;

public slots:
    void slotRegister();
    void slotLogIn();
    void slotConnect();
    void dataReceived();
    void connected();
};

#endif // SIGN_H
