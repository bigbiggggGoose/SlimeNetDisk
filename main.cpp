 #include "mainwindow.h"
 #include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    LoginDialog l;
    l.show();
    return a.exec();
}
