#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>

class CWorkspace;

namespace Ui {
class CMainWindow;
}

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    CWorkspace* getWorkspace() { return m_workspace; }
    Ui::CMainWindow* getUi() { return ui; }

private slots:
    void on_actionExit_triggered();

    void on_actionLoadTestSuite_triggered();

    void on_actionNewWorkspace_triggered();

    void on_actionDumpCoverage_triggered();

    void on_actionShowStatistics_triggered();

    void on_actionLoadWorkspace_triggered();

    void on_actionSaveWorkspace_triggered();

private:

    bool saveWorkspace();
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;
    CWorkspace* m_workspace;
};

#endif // CMAINWINDOW_H
