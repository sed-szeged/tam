#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "engine/CKernel.h"

#include <QMainWindow>
#include <QStandardItemModel>

using namespace soda;

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
    CKernel* getKernel() { return m_kernel; }
    Ui::CMainWindow* getUi() { return ui; }

private slots:
    void on_actionExit_triggered();

    void on_actionLoadTestSuite_triggered();

    void on_actionNewWorkspace_triggered();

    void on_actionDumpCoverage_triggered();

    void on_actionShowStatistics_triggered();

    void on_actionLoadWorkspace_triggered();

    void on_actionSaveWorkspace_triggered();

    void on_buttonClusterTestList_clicked();

    void on_checkBoxSelectAll_stateChanged(int state);

    void on_buttonClusterCEList_clicked();

private:

    bool saveWorkspace();
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;
    CWorkspace *m_workspace;
    CKernel *m_kernel;

    QStandardItemModel* m_model;

    QStringList m_clusterAlgorithms;
};

#endif // CMAINWINDOW_H
