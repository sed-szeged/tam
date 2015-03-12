/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include "engine/CKernel.h"
#include "wrapper/CClusterList.h"
#include "helper/CClusterPluginParameterTableModel.h"
#include "helper/CTableViewButtonDelegate.h"

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

/**
 * @brief The CMainWindow class implements the main window of the application and also acts as a controller too.
 */
class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:

    /**
     * @brief Creates an object and initialises the main window.
     * @param parent
     */
    explicit CMainWindow(QWidget *parent = 0);
    ~CMainWindow();

    /**
     * @brief Returns true if a test-suite is loaded.
     * @return
     */
    bool isTestSuiteAvailable();

    /**
     * @brief Returns the current workspace.
     * @return Current workspace.
     */
    CWorkspace* getWorkspace() { return m_workspace; }

    /**
     * @brief Returns the plugin manager.
     * @return Plugin manager.
     */
    CKernel* getKernel() { return m_kernel; }

    /**
     * @brief Returns a container which holds functional groups.
     * @return Container which holds functional groups.
     */
    CClusterList* getClusterList() { return m_clusterList; }

    /**
     * @brief Updates the lists which shows the available functional groups.
     */
    void updateAvailableClusters();

    Ui::CMainWindow* getUi() { return ui; }

    /**
     * @brief Returns the name of the current metric measurement.
     * @return Name of the current metric measurement.
     */
    String getMetricMeasurement();

    /**
     * @brief Returns the name of the current fault localization measurement.
     * @return Name of the current fault localization measurement.
     */
    String getScoreMeasurement();

private slots:

    /**
     * @brief Called upon receiving a status update.
     * @param label New text of the label.
     */
    void statusUpdate(QString label);

    /**
     * @brief Called upon temporary status update.
     * @param msg Message to show.
     */
    void tmpStatusUpdate(QString msg);

    /**
     * @brief Called when loading of test-suite is finished.
     * @param msg Message to show on status bar.
     */
    void loadFinished(QString msg);

    /**
     * @brief Called when computation of statistics is finished.
     * @param msg Message to show on status bar.
     */
    void calcStatsFinished(QString msg);

    /**
     * @brief Called when computation of metrics is finished.
     * @param msg Message to show on status bar.
     */
    void calcMetricsFinished(QString msg);

    /**
     * @brief Called when computation of fault localization is finished.
     * @param msg Message to show on status bar.
     */
    void calcScoreFinished(QString msg);

    /**
     * @brief Called when computation of groups is finished.
     * @param msg Message to show on status bar.
     */
    void clusterFinished(QString msg);

    /**
     * @brief Called when Exit menu item is clicked.
     */
    void on_actionExit_triggered();

    /**
     * @brief Called upon clicking new workspace menu item.
     */
    void on_actionNewWorkspace_triggered();

    /**
     * @brief Called upon clicking load workspace menu item.
     */
    void on_actionLoadWorkspace_triggered();

    /**
     * @brief Called upon clicking save workspace menu item.
     */
    void on_actionSaveWorkspace_triggered();

    /**
     * @brief Called upon clicking save workspace as menu item.
     */
    void on_actionSaveWorkspaceAs_triggered();

    /**
     * @brief Called upon clicking on Calculate button of Groups Tab.
     */
    void on_buttonCalcCluster_clicked();

    /**
     * @brief Changes the cluster parameters and the current cluster plugin.
     * @param plugin Name of the selected plugin.
     */
    void on_comboBoxClusterPlugins_currentIndexChanged(const QString &plugin);

    /**
     * @brief Creates a new group.
     */
    void on_buttonNewCluster_clicked();

    /**
     * @brief Opens a dialog for editing existing group.
     */
    void on_buttonEditCluster_clicked();

    /**
     * @brief Removes the selected groups.
     */
    void on_buttonDeleteCluster_clicked();

    /**
     * @brief Opens a file browser for selecting coverage file.
     */
    void on_buttonBrowseCov_clicked();

    /**
     * @brief Opens a file browser for selecting results file.
     */
    void on_buttonBrowseRes_clicked();

    /**
     * @brief Opens a file browser for selecting changeset file.
     */
    void on_buttonBrowseCha_clicked();

    /**
     * @brief Loads the selected files into a test-suite.
     */
    void on_buttonLoad_clicked();

    /**
     * @brief Visualises statistics based on the index.
     * @param index Tab index.
     */
    void on_tabWidgetStatistics_currentChanged(int index);

    /**
     * @brief Lists loaded code elements and test cases based on the index.
     * @param index Tab index.
     */
    void on_tabWidgetMain_currentChanged(int index);

    /**
     * @brief Generates group statistics based on the index.
     * @param index Tab index.
     */
    void on_tabWidgetCluster_currentChanged(int index);

    /**
     * @brief Visualises metrics results if any based on the index.
     * @param index Tab index.
     */
    void on_tabWidgetMetrics_currentChanged(int index);

    /**
     * @brief Visualises fault localization results if any based on the tab index.
     * @param index Tab index.
     */
    void on_tabWidgetScore_currentChanged(int index);

    /**
     * @brief Changes the selected revision number for metrics.
     * @param text Selected revision number as string.
     */
    void on_comboBoxRevMetrics_currentIndexChanged(const QString &text);

    /**
     * @brief Updates the selected metric plugins in the list view.
     * @param item Item of the list view.
     */
    void metricsPluginStateChanged(QStandardItem *item);

    /**
     * @brief Updates the selected groups in the list view.
     * @param item Item of the list view.
     */
    void metricsClusterStateChanged(QStandardItem *item);

    /**
     * @brief Computes the selected metrics on the selected groups.
     */
    void on_buttonCalculateMetrics_clicked();

    /**
     * @brief Selects all metric plugins the list view.
     * @param arg1
     */
    void on_checkBoxMetricsSelectAll_stateChanged(int arg1);

    /**
     * @brief Changes the current measurement.
     * @param index
     */
    void on_comboBoxMetricsMeasurement_currentIndexChanged(int index);

    /**
     * @brief Creates a new metric measurement.
     */
    void on_toolButtonMetricMeasAdd_clicked();

    /**
     * @brief Removes the current metric measurement.
     */
    void on_toolButtonMetricMeasRem_clicked();

    /**
     * @brief Changes the selected revision number for fault localization technique.
     * @param text Selected revision number as string.
     */
    void on_comboBoxRevScore_currentIndexChanged(const QString &text);

    /**
     * @brief Updates the selected fault localization plugins in the list view.
     * @param item Item of the list view.
     */
    void scorePluginStateChanged(QStandardItem *item);

    /**
     * @brief Updates the selected groups in the list view.
     * @param item Item of the list view.
     */
    void scoreClusterStateChanged(QStandardItem *item);

    /**
     * @brief Computes the selected fault localization techniques on the selected groups.
     */
    void on_buttonScoreCalc_clicked();

    /**
     * @brief Opens a dialog for selecting code elements for detailed calculations.
     */
    void on_buttonAddFailedCodeElement_clicked();

    /**
     * @brief Remove the selected code elements from the list view.
     */
    void on_buttonRemoveFailedCodeElement_clicked();

    /**
     * @brief Changes the current fault localization measurement.
     * @param index
     */
    void on_comboBoxScoreMeasurement_currentIndexChanged(int index);

    /**
     * @brief Creates a new fault localization measurement.
     */
    void on_toolButtonScoreMeasAdd_clicked();

    /**
     * @brief Removes the current fault localization measurement.
     */
    void on_toolButtonScoreMeasRem_clicked();

    /**
     * @brief Filters the test cases in the list view.
     * @param text Content of line edit.
     */
    void on_lineEditFilterTests_textEdited(const QString &text);

    /**
     * @brief Opens a context menu.
     * @param pos Position of the cursor.
     */
    void on_tableViewTests_customContextMenuRequested(const QPoint &pos);

    /**
     * @brief Lists the code elements covered by the selected test cases.
     */
    void actionCoveredCodeElements_triggered();

    /**
     * @brief Filters the test cases in the list view.
     * @param text Content of line edit.
     */
    void on_lineEditFilterCE_textEdited(const QString &text);

    /**
     * @brief Opens a context menu.
     * @param pos Position of the cursor.
     */
    void on_tableViewCE_customContextMenuRequested(const QPoint &pos);

    /**
     * @brief Lists the test cases covered by the selected code element.
     */
    void actionCoveredTests_triggered();

    /**
     * @brief Called upon Export metrics menu item is clicked.
     */
    void on_actionMetrics_results_triggered();

    /**
     * @brief Called upon Export fault localization menu item is clicked.
     */
    void on_actionFault_localization_results_triggered();

    /**
     * @brief Called upon comboBoxUncoveredGroup index is changes.
     * @param clusterName Name of the group.
     */
    void on_comboBoxUncoveredGroup_currentIndexChanged(const QString &clusterName);

protected:

    /**
     * @brief Prevent calling event when the dialog is in invalid state.
     * @param object Object which takes the action.
     * @param event Event which is called.
     * @return True if the current event have to be interrupted.
     */
    bool eventFilter(QObject *object, QEvent *event);

private:

    /**
     * @brief Creates a new workspace.
     */
    void createNewWorkspace();

    /**
     * @brief Initialises the status bar.
     */
    void createStatusBar();

    /**
     * @brief Computes the statistics of the current test-suite.
     */
    void calculateStatistics();

    /**
     * @brief Fills the widgets with available plugins
     */
    void fillWidgets();

    /**
     * @brief Fills combo boxes which holds the available revision numbers.
     */
    void fillRevComboBoxes();

    /**
     * @brief Updates the labels on General tab.
     */
    void updateLabels();

    /**
     * @brief Updates the actual ClusterPluginParameterModel based on the selected cluster algorithm.
     * @param Name of the plugin.
     */
    void updateClusterParameters(QString cluster);

    /**
     * @brief Returns the name of the metric plugin which needs additional requirement for computation.
     * @return Name of the plugin which has additional dependencies for computation
     */
    QString checkMetricsPluginsRequirements();

    /**
     * @brief Checks if a results matrix is required for the given metric.
     * @param plugin Name of the metric.
     * @return True if required.
     */
    bool isRequiredResultsForPlugin(QString plugin);

    /**
     * @brief Clears the lists which belongs to the metrics tab.
     */
    void clearMetricsConfiguration();

    /**
     * @brief Updates metrics tab based on the current measurement.
     */
    void updateMetricsConfiguration();

    /**
     * @brief Clears the lists which belongs to the fault localization technique tab.
     */
    void clearScoreConfiguration();

    /**
     * @brief Updates fault localization technique tab based on the current measurement.
     */
    void updateScoreConfiguration();

    /**
     * @brief Saves the current workspace to the same file if it exists otherwise we need to specify the path.
     * @return True if the process was successful.
     */
    bool saveWorkspace();

    /**
     * @brief Saves the current workspace to another file.
     * @return True if the process was successful.
     */
    bool saveWorkspaceAs();

    Ui::CMainWindow *ui;

    /**
     * @brief Label which indicates the status of background processes.
     */
    QLabel *m_statusLabel;

    /**
     * @brief Label which indicates if a test-suite is loaded.
     */
    QLabel *m_testSuiteAvailableLabel;

    /**
     * @brief Progress bar which indicates the background processes.
     */
    QProgressBar *m_statusProgressBar;

    /**
     * @brief Current workspace.
     */
    CWorkspace *m_workspace;

    /**
     * @brief Plugin manager.
     */
    CKernel *m_kernel;

    /**
     * @brief Model which holds the names of the metric plugins.
     */
    QStandardItemModel *m_metricsPluginModel;

    /**
     * @brief Model which holds the names of the fault localization technique plugins.
     */
    QStandardItemModel *m_scorePluginModel;

    /**
     * @brief Stores the models for cluster plugins.
     */
    QMap<std::string, CClusterPluginParameterTableModel*> m_clusterPluginParameters;

    /**
     * @brief Special button for group parameters.
     */
    CTableViewButtonDelegate *m_clusterTableViewDelegate;

    /**
     * @brief Container of groups.
     */
    CClusterList *m_clusterList;
};

#endif // CMAINWINDOW_H
