#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "engine/CKernel.h"
#include "lib/CTestSuiteMetrics.h"
#include "lib/CClusterList.h"

#include <QMainWindow>
#include <QProgressBar>
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
    CTestSuiteMetrics* getMetrics() { return m_metrics; }
    CKernel* getKernel() { return m_kernel; }
    CClusterList* getClusterList() { return m_clusterList; }

    Ui::CMainWindow* getUi() { return ui; }

private slots:
    void on_actionExit_triggered();

    void on_actionNewWorkspace_triggered();

    void on_actionDumpCoverage_triggered();

    void on_actionLoadWorkspace_triggered();

    void on_actionSaveWorkspace_triggered();

    void on_buttonClusterTestList_clicked();

    void on_buttonClusterCEList_clicked();

    void on_actionComputeClusters_triggered();

    void on_actionCalculateStatistics_triggered();

    void on_buttonCalculateMetrics_clicked();

    void on_checkBoxMetricsSelectAll_stateChanged(int arg1);

    void on_actionShowMetrics_triggered();

    void on_buttonCalcCluster_clicked();

    void on_buttonBrowseCov_clicked();

    void on_buttonBrowseRes_clicked();

    void on_buttonBrowseCha_clicked();

    void on_buttonLoad_clicked();

    void on_actionSaveWorkspaceAs_triggered();

private:

    void createStatusBar();
    void fillWidgets();
    void updateLabels();
    bool saveWorkspace();
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;
    QStandardItemModel *m_model;
    QProgressBar *m_progressBar;

    CWorkspace *m_workspace;
    CKernel *m_kernel;
    CTestSuiteMetrics *m_metrics;
    CClusterList *m_clusterList;

    QStringList m_clusterAlgorithms;
};

#endif // CMAINWINDOW_H
