#include "CMainWindow.h"
#include <QApplication>
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyleSheet("QStatusBar::item { border: none; }"); // removes borders from statusbar items
    setlocale(LC_ALL, "C");
    setlocale(LC_CTYPE, "C");
    CMainWindow w;
    w.show();

    return a.exec();
}
