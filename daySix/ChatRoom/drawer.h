#ifndef DRAWER_H
#define DRAWER_H

#include <QToolButton>
#include <QToolBox>
#include "widget.h"

class Drawer : public QToolBox
{
    Q_OBJECT
public:
    Drawer(QWidget *parent = 0, Qt::WindowFlags f = 0);

private slots:
    void showChatWidget1();
    void showChatWidget2();


private:
    QToolButton *toolBtn1;
    QToolButton *toolBtn2;

    Widget *chatWidget1;
    Widget *chatWidget2;
};

#endif // DRAWER_H
