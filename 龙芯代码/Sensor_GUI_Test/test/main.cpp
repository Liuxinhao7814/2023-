#include "mainwindow.h"
#include "loginactivity.h"
#include "../ZigBee/zigbee.h"
#include "../node/node.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginActivity loginWindows;
    loginWindows.show();
//    MainWindow w;
//    w.show();
    return a.exec();
}
