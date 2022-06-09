#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("srh36DataAcquire");
    a.setOrganizationName("ISTP");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    MainWindow w;
    w.show();

    return a.exec();
}
