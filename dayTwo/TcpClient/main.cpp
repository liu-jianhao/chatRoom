#include "tcpclient.h"
#include <QApplication>
#include <QPixmap>
#include <QSplashScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QPixmap pixmap("Linux.jpg"); //启动程序前显示的图片
    QSplashScreen splash(pixmap);
    splash.show();
    a.processEvents();

    TcpClient w;
    w.show();
    splash.finish(&w);

    return a.exec();
}
