#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "engine/CKernel.h"
#include "lib/CTestSuiteMetrics.h"
#include "lib/CClusterList.h"
#include "lib/CTestSuiteLoader.h"
#include "lib/CStatisticsThread.h"

#include <QCompleter>
#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
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
    void statusUpdate(QString label);
    void loadFinished(QString msg);
    void calcStatsFinished(QString msg);

    void on_actionExit_triggered();

    void on_actionNewWorkspace_triggered();

    void on_actionDumpCoverage_triggered();

    void on_actionLoadWorkspace_triggered();

    void on_actionSaveWorkspace_triggered();

    void on_buttonClusterTestList_clicked();

    void on_buttonClusterCEList_clicked();

    void on_buttonCalculateMetrics_clicked();

    void on_checkBoxMetricsSelectAll_stateChanged(int arg1);

    void on_buttonCalcCluster_clicked();

    void on_buttonBrowseCov_clicked();

    void on_buttonBrowseRes_clicked();

    void on_buttonBrowseCha_clicked();

    void on_buttonLoad_clicked();

    void on_actionSaveWorkspaceAs_triggered();

    void on_tabWidgetStatistics_currentChanged(int index);

    void on_tabWidgetMain_currentChanged(int index);

    void on_tabWidgetCluster_currentChanged(int index);

    void on_tabWidgetMetrics_currentChanged(int index);

private:

    void createStatusBar();
    void createCompleterForMetrics();
    void calculateStatistics();
    void fillWidgets();
    void updateLabels();
    bool saveWorkspace();
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;

    QLabel *m_statusLabel;
    QLabel *m_testSuiteAvailableLabel;
    QProgressBar *m_statusProgressBar;

    CStatisticsThread *m_statsThread;
    CTestSuiteLoader *m_loadThread;
    CWorkspace *m_workspace;
    CKernel *m_kernel;

    CTestSuiteMetrics *m_metrics;
    QCompleter *m_revCompleter;
    QStandardItemModel *m_model;

    CClusterList *m_clusterList;
    QStringList m_clusterAlgorithms;
};

#endif // CMAINWINDOW_H
