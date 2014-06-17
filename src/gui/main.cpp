#include "CMainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setlocale(LC_ALL, "C");
    setlocale(LC_CTYPE, "C");
    CMainWindow w;
    w.show();

    return a.exec();
}
