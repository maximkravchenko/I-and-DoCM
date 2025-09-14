#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setFixedSize(1100, 700);
    w.setWindowTitle("Grimm pressent");


    QIcon iconApp(":/logo/resources/img/logo/logo.ico");
    w.setWindowIcon(iconApp);


    w.show();
    return a.exec();
}
