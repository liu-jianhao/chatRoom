#ifndef ADDFRIENDS_H
#define ADDFRIENDS_H

#include <QDialog>
#include <QPushButton>
#include <QTcpSocket>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QHostAddress>


class AddFriends : public QDialog
{
    Q_OBJECT
public:
    AddFriends(QWidget *parent = 0);
    ~AddFriends();

private:
    QGridLayout *mainLayout;
    QLabel *nameLabel;
    QLineEdit *nameLineEdit;
    QPushButton *enterBtn;

    int port;
    QHostAddress *serverIP;
    QTcpSocket *tcpSocket;

public slots:
    void slotEnter();
    void dataReceived();
};

#endif // ADDFRIENDS_H
