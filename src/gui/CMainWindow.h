#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "engine/CKernel.h"
#include "lib/CClusterList.h"
#include "CClusterPluginParameterTableModel.h"
#include "CTableViewButtonDelegate.h"

#include <QMainWindow>
#include <QProgressBar>
#include <QLabel>
#include <QStandardItemModel>
#include <QMap>

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

    bool isTestSuiteAvailable();
    CWorkspace* getWorkspace() { return m_workspace; }
    CKernel* getKernel() { return m_kernel; }
    CClusterList* getClusterList() { return m_clusterList; }
    void updateAvailableClusters();
    Ui::CMainWindow* getUi() { return ui; }
    String getMetricMeasurement();
    String getScoreMeasurement();

private slots:
    void statusUpdate(QString label);
    void tmpStatusUpdate(QString msg);
    void loadFinished(QString msg);
    void calcStatsFinished(QString msg);
    void calcMetricsFinished(QString msg);
    void calcScoreFinished(QString msg);
    void clusterFinished(QString msg);

    void on_actionExit_triggered();
    void on_actionNewWorkspace_triggered();
    void on_actionLoadWorkspace_triggered();
    void on_actionSaveWorkspace_triggered();
    void on_actionSaveWorkspaceAs_triggered();

    void on_buttonCalcCluster_clicked();
    void on_comboBoxClusterPlugins_currentIndexChanged(const QString &plugin);
    void on_buttonNewCluster_clicked();
    void on_buttonEditCluster_clicked();
    void on_buttonDeleteCluster_clicked();

    void on_buttonBrowseCov_clicked();
    void on_buttonBrowseRes_clicked();
    void on_buttonBrowseCha_clicked();
    void on_buttonLoad_clicked();

    void on_tabWidgetStatistics_currentChanged(int index);
    void on_tabWidgetMain_currentChanged(int index);
    void on_tabWidgetCluster_currentChanged(int index);
    void on_tabWidgetMetrics_currentChanged(int index);
    void on_tabWidgetScore_currentChanged(int index);

    void on_comboBoxRevMetrics_currentIndexChanged(const QString &text);
    void metricsPluginStateChanged(QStandardItem *item);
    void metricsClusterStateChanged(QStandardItem *item);
    void on_buttonCalculateMetrics_clicked();
    void on_checkBoxMetricsSelectAll_stateChanged(int arg1);
    void on_comboBoxMetricsMeasurement_currentIndexChanged(int index);
    void on_toolButtonMetricMeasAdd_clicked();
    void on_toolButtonMetricMeasRem_clicked();

    void on_comboBoxRevScore_currentIndexChanged(const QString &text);
    void scorePluginStateChanged(QStandardItem *item);
    void scoreClusterStateChanged(QStandardItem *item);
    void on_buttonScoreCalc_clicked();
    void on_buttonAddFailedCodeElement_clicked();
    void on_buttonRemoveFailedCodeElement_clicked();
    void on_comboBoxScoreMeasurement_currentIndexChanged(int index);
    void on_toolButtonScoreMeasAdd_clicked();
    void on_toolButtonScoreMeasRem_clicked();

    void on_lineEditFilterTests_textEdited(const QString &text);
    void on_tableViewTests_customContextMenuRequested(const QPoint &pos);
    void actionCoveredCodeElements_triggered();

    void on_lineEditFilterCE_textEdited(const QString &text);
    void on_tableViewCE_customContextMenuRequested(const QPoint &pos);
    void actionCoveredTests_triggered();

    void on_actionMetrics_results_triggered();
    void on_actionFault_localization_results_triggered();

    void on_comboBoxUncoveredGroup_currentIndexChanged(const QString &clusterName);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    void createNewWorkspace();
    void createStatusBar();
    void calculateStatistics();
    void fillWidgets();
    void fillRevComboBoxes();
    void updateLabels();
    void updateClusterParameters(QString cluster);
    QString checkMetricsPluginsRequirements();
    bool isRequiredResultsForPlugin(QString plugin);

    void clearMetricsConfiguration();
    void updateMetricsConfiguration();
    void clearScoreConfiguration();
    void updateScoreConfiguration();
    bool saveWorkspace();
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;

    QLabel *m_statusLabel;
    QLabel *m_testSuiteAvailableLabel;
    QProgressBar *m_statusProgressBar;

    CWorkspace *m_workspace;
    CKernel *m_kernel;

    QStandardItemModel *m_metricsPluginModel;
    QStandardItemModel *m_scorePluginModel;
    QMap<std::string, CClusterPluginParameterTableModel*> m_clusterPluginParameters;
    CTableViewButtonDelegate *m_clusterTableViewDelegate;
    CClusterList *m_clusterList;
};

#endif // CMAINWINDOW_H
