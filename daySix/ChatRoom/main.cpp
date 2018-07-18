#include "sign.h"
#include "drawer.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Sign s;
    s.show();

    return a.exec();
}
