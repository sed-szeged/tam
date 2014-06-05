#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include "data/CSelectionData.h"

using namespace soda;

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    void loadSelectionData(QString coverage, QString results, QString changes);

private slots:
    void on_actionExit_triggered();

    void on_actionLoadTestSuite_triggered();

    void on_actionDumpCoverage_triggered();

private:
    Ui::CMainWindow *ui;
    CSelectionData *selectionData;
};

#endif // CMAINWINDOW_H
