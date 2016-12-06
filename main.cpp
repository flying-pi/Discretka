#include "mainwindow.h"
#include <QApplication>
#include "logic/printutil.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow windows;
    windows.show();

    return a.exec();
}
