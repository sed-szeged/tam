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

#include "ui/CMainWindow.h"
#include "ui_CMainWindow.h"

#include "runnable/CTestSuiteCluster.h"
#include "runnable/CTestSuiteMetrics.h"
#include "runnable/CTestSuiteLoader.h"
#include "runnable/CStatisticsThread.h"
#include "runnable/CFLScore.h"
#include "wrapper/CWorkspace.h"

#include "ui/CCoveredListDialog.h"
#include "ui/CClusterEditorAddDialog.h"
#include "ui/CClusterEditorDialog.h"
#include "ui/CResultsExportDialog.h"

#include "helper/CIDManagerTableModel.h"
#include "helper/CShowStatistics.h"
#include "helper/CShowClusters.h"
#include "helper/CShowMetrics.h"
#include "helper/CShowScores.h"

#include <QStringListModel>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QStatusBar>
#include <QCheckBox>
#include <QSortFilterProxyModel>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::CMainWindow),
    m_kernel(new CKernel())
{
    ui->setupUi(this);

    ui->buttonLoad->installEventFilter(this);
    ui->buttonCalculateMetrics->installEventFilter(this);
    ui->buttonScoreCalc->installEventFilter(this);

    m_workspace = new CWorkspace(this);
    m_clusterList = new CClusterList();

    m_clusterTableViewDelegate = new CTableViewButtonDelegate(ui->tableViewClusterParameter);

    createStatusBar();
    fillWidgets();
}

CMainWindow::~CMainWindow()
{
    delete ui;
    delete m_statusProgressBar;
    delete m_testSuiteAvailableLabel;
    delete m_statusLabel;
    delete m_workspace;
    delete m_kernel;
    delete m_clusterList;
    delete m_scorePluginModel;
    delete m_metricsPluginModel;
}

void CMainWindow::fillWidgets()
{
    // fill combobox with cluster algorithms
    StringVector clusterPlugins = m_kernel->getTestSuiteClusterPluginManager().getPluginNames();
    for (int i = 0; i < clusterPlugins.size(); ++i) {
        ui->comboBoxClusterPlugins->addItem(tr(clusterPlugins[i].c_str()));
        String desc = m_kernel->getTestSuiteClusterPluginManager().getPlugin(clusterPlugins[i])->getDescription();
        ui->comboBoxClusterPlugins->setItemData(i, tr(desc.c_str()), Qt::ToolTipRole);
    }

    // fill listview with metric algorithm names
    m_metricsPluginModel = new QStandardItemModel();

    StringVector metricPlugins = m_kernel->getTestSuiteMetricPluginManager().getPluginNames();
    for (int i = 0; i < metricPlugins.size(); ++i) {
        QStandardItem *item = new QStandardItem(tr(metricPlugins[i].c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        String desc = m_kernel->getTestSuiteMetricPluginManager().getPlugin(metricPlugins[i])->getDescription();
        item->setToolTip(tr(desc.c_str()));
        m_metricsPluginModel->appendRow(item);
    }
    connect(m_metricsPluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsPluginStateChanged(QStandardItem*)));
    ui->listViewMetrics->setModel(m_metricsPluginModel);

    // fill listview with falt localization technique names
    m_scorePluginModel = new QStandardItemModel();

    StringVector scorePlugins = m_kernel->getFaultLocalizationTechniquePluginManager().getPluginNames();
    for (int i = 0; i < scorePlugins.size(); ++i) {
        QStandardItem *item = new QStandardItem(tr(scorePlugins[i].c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        String desc = m_kernel->getFaultLocalizationTechniquePluginManager().getPlugin(scorePlugins[i])->getDescription();
        item->setToolTip(tr(desc.c_str()));
        m_scorePluginModel->appendRow(item);
    }

    connect(m_scorePluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scorePluginStateChanged(QStandardItem*)));
    ui->listViewFaultlocalization->setModel(m_scorePluginModel);

    ui->listViewMetricsSelClu->setModel(new QStandardItemModel(this));
    connect(qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsClusterStateChanged(QStandardItem*)));
    ui->listViewScoreSelClu->setModel(new QStandardItemModel(this));
    connect(qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scoreClusterStateChanged(QStandardItem*)));
    ui->listViewClusters->setModel(new QStandardItemModel(this));

    ui->listViewFailedCodeElements->setModel(new QStandardItemModel(this));
    ui->listViewUncovered->setModel(new QStringListModel(ui->listViewUncovered));
    ui->listViewUncovered->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CMainWindow::updateClusterParameters(QString cluster)
{
    std::map<String, String> params = m_kernel->getTestSuiteClusterPluginManager().getPlugin(cluster.toStdString())->getRequiredParameters();
    if (m_clusterPluginParameters.count(cluster.toStdString()) == 0)
        m_clusterPluginParameters[cluster.toStdString()] = new CClusterPluginParameterTableModel(this, params);

    if (cluster == "label-test-codeelements")
        ui->tableViewClusterParameter->setItemDelegateForColumn(2, m_clusterTableViewDelegate);
    else
        ui->tableViewClusterParameter->setItemDelegateForColumn(2, 0);

    ui->tableViewClusterParameter->setModel(m_clusterPluginParameters[cluster.toStdString()]);
    ui->tableViewClusterParameter->resizeColumnsToContents();
}

void CMainWindow::fillRevComboBoxes()
{
    QStringList revList;
    IntVector revs = m_workspace->getTestSuite()->getResults()->getRevisionNumbers();
    for (int i = 0; i < revs.size(); ++i)
        revList << QString::number(revs[i]);

    disconnect(ui->comboBoxRevMetrics, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxRevMetrics_currentIndexChanged(QString)));
    ui->comboBoxRevMetrics->addItems(revList);
    connect(ui->comboBoxRevMetrics, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxRevMetrics_currentIndexChanged(QString)));

    disconnect(ui->comboBoxRevScore, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxRevScore_currentIndexChanged(QString)));
    ui->comboBoxRevScore->addItems(revList);
    connect(ui->comboBoxRevScore, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_comboBoxRevScore_currentIndexChanged(QString)));
}

void CMainWindow::createNewWorkspace()
{
    delete m_workspace;
    m_testSuiteAvailableLabel->hide();
    m_workspace = new CWorkspace(this);
    m_clusterList->getClusters().clear();

    ui->listViewClusters->model()->removeRows(0, ui->listViewClusters->model()->rowCount());
    ui->listViewMetricsSelClu->model()->removeRows(0, ui->listViewMetricsSelClu->model()->rowCount());
    ui->listViewScoreSelClu->model()->removeRows(0, ui->listViewScoreSelClu->model()->rowCount());

    for (int i = 0; i < ui->tabWidgetStatistics->tabBar()->count(); ++i) {
        QWidget* widget = ui->tabWidgetStatistics->widget(i);
        QList<QWebView*> webView = widget->findChildren<QWebView*>();
        if (webView.count() == 1) {
            webView[0]->settings()->clearMemoryCaches();
            webView[0]->setHtml("");
        }
    }

    clearMetricsConfiguration();
    clearScoreConfiguration();

    ui->comboBoxRevMetrics->clear();
    ui->comboBoxRevScore->clear();

    ui->comboBoxMetricsMeasurement->clear();
    ui->comboBoxScoreMeasurement->clear();
    m_workspace->removeAllMeasurement();
    m_workspace->addMeasurement(METRICS, "default");
    ui->comboBoxMetricsMeasurement->addItem("default");
    m_workspace->addMeasurement(SCORE, "default");
    ui->comboBoxScoreMeasurement->addItem("default");

    if (ui->tableViewCE->model())
        delete ui->tableViewCE->model();
    if (ui->tableViewTests->model())
        delete ui->tableViewTests->model();
    updateLabels();
}

QString CMainWindow::checkMetricsPluginsRequirements()
{
    for (int i = 0; i < m_metricsPluginModel->rowCount(); ++i) {
        if (m_metricsPluginModel->item(i)->checkState() == Qt::Checked) {
            if (isRequiredResultsForPlugin(m_metricsPluginModel->item(i)->text()))
                return m_metricsPluginModel->item(i)->text();

            if (m_metricsPluginModel->item(i)->text() == "fault-localization" ||
                    m_metricsPluginModel->item(i)->text() == "uniqueness" ||
                    m_metricsPluginModel->item(i)->text() == "specialization")
                return m_metricsPluginModel->item(i)->text();
        }
    }

    return QString();
}

bool CMainWindow::isTestSuiteAvailable()
{
    return m_workspace->isTestSuiteAvailable();
}

bool CMainWindow::isRequiredResultsForPlugin(QString plugin)
{
    StringVector dependency = m_kernel->getTestSuiteMetricPluginManager().getPlugin(plugin.toStdString())->getDependency();
    for (StringVector::const_iterator it = dependency.begin(); it != dependency.end(); ++it) {
        if (isRequiredResultsForPlugin(QString((*it).c_str())))
            return true;

        if (*it == "fault-localization" || *it == "uniqueness" ||
            *it == "specialization")
            return true;
    }
    return false;
}

void CMainWindow::createStatusBar()
{
    m_statusProgressBar = new QProgressBar(ui->statusBar);
    m_statusProgressBar->setAlignment(Qt::AlignRight);
    m_statusProgressBar->setMaximumSize(180, 19);
    m_statusProgressBar->setTextVisible(false);

    m_statusLabel = new QLabel(ui->statusBar);
    m_statusLabel->setAlignment(Qt::AlignRight);

    m_testSuiteAvailableLabel = new QLabel(ui->statusBar);
    m_testSuiteAvailableLabel->setAlignment(Qt::AlignCenter);
    m_testSuiteAvailableLabel->setText("TS");
    m_testSuiteAvailableLabel->setToolTip("Test-suite loaded.");
    m_testSuiteAvailableLabel->setVisible(false);

    ui->statusBar->setSizeGripEnabled(false);
    ui->statusBar->addPermanentWidget(m_testSuiteAvailableLabel);
    ui->statusBar->addPermanentWidget(m_statusLabel);
    ui->statusBar->addPermanentWidget(m_statusProgressBar);
}

void CMainWindow::updateLabels()
{
    rapidjson::Document &res = *m_workspace->getData(WS);
    QFileInfo fileInfo;
    fileInfo.setFile(m_workspace->getCoveragePath());
    if (fileInfo.exists())
        ui->labelCov->setText(fileInfo.fileName());
    else
        ui->labelCov->setText("No file selected");

    fileInfo.setFile(m_workspace->getResultsPath());
    if (fileInfo.exists())
        ui->labelRes->setText(fileInfo.fileName());
    else
        ui->labelRes->setText("No file selected");

    fileInfo.setFile(m_workspace->getChangesetPath());
    if (fileInfo.exists())
        ui->labelChan->setText(fileInfo.fileName());
    else
        ui->labelChan->setText("No file selected");
}

String CMainWindow::getMetricMeasurement()
{
    return ui->comboBoxMetricsMeasurement->currentText().toStdString();
}

String CMainWindow::getScoreMeasurement()
{
    return ui->comboBoxScoreMeasurement->currentText().toStdString();
}

void CMainWindow::clearMetricsConfiguration()
{
    ui->comboBoxRevMetrics->setCurrentIndex(-1);

    disconnect(m_metricsPluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsPluginStateChanged(QStandardItem*)));
    for (int i = 0; i < m_metricsPluginModel->rowCount(); ++i) {
        m_metricsPluginModel->item(i)->setCheckState(Qt::Unchecked);
    }
    connect(m_metricsPluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsPluginStateChanged(QStandardItem*)));

    disconnect(qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsClusterStateChanged(QStandardItem*)));
    for (int i = 0; i < ui->listViewMetricsSelClu->model()->rowCount(); ++i) {
        qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model())->item(i)->setCheckState(Qt::Unchecked);
    }
    connect(qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsClusterStateChanged(QStandardItem*)));
}

void CMainWindow::updateMetricsConfiguration()
{
    clearMetricsConfiguration();

    rapidjson::Value &metricsSettings = (*m_workspace->getMeasurement(METRICS, getMetricMeasurement()));
    rapidjson::Value::MemberIterator member = metricsSettings.FindMember("revision");
    if (member != metricsSettings.MemberEnd()) {
        int index = ui->comboBoxRevMetrics->findText(QString::number(member->value.GetInt()));
        ui->comboBoxRevMetrics->setCurrentIndex(index);
    }

    member = metricsSettings.FindMember("plugin");
    if (member != metricsSettings.MemberEnd()) {
        for (rapidjson::SizeType i = 0; i < member->value.Size(); ++i) {
            QList<QStandardItem *> list = m_metricsPluginModel->findItems(member->value[i].GetString());
            if (list.size())
                list.first()->setCheckState(Qt::Checked);
        }
    }

    member = metricsSettings.FindMember("cluster");
    if (member != metricsSettings.MemberEnd()) {
        for (rapidjson::SizeType i = 0; i < member->value.Size(); ++i) {
            QList<QStandardItem *> list = qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model())->findItems(member->value[i].GetString());
            if (list.size())
                list.first()->setCheckState(Qt::Checked);
        }
    }
}

void CMainWindow::clearScoreConfiguration()
{
    ui->comboBoxRevScore->setCurrentIndex(-1);

    disconnect(m_scorePluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scorePluginStateChanged(QStandardItem*)));
    for (int i = 0; i < m_scorePluginModel->rowCount(); ++i) {
        m_scorePluginModel->item(i)->setCheckState(Qt::Unchecked);
    }
    connect(m_scorePluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scorePluginStateChanged(QStandardItem*)));

    disconnect(qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scoreClusterStateChanged(QStandardItem*)));
    for (int i = 0; i < ui->listViewScoreSelClu->model()->rowCount(); ++i) {
        qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model())->item(i)->setCheckState(Qt::Unchecked);
    }
    connect(qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scoreClusterStateChanged(QStandardItem*)));

    ui->listViewFailedCodeElements->model()->removeRows(0 ,ui->listViewFailedCodeElements->model()->rowCount());
}

void CMainWindow::updateScoreConfiguration()
{
    clearScoreConfiguration();

    rapidjson::Value &scoreSettings = (*m_workspace->getMeasurement(SCORE, getScoreMeasurement()));
    rapidjson::Value::MemberIterator member = scoreSettings.FindMember("revision");
    if (member != scoreSettings.MemberEnd()) {
        int index = ui->comboBoxRevScore->findText(QString::number(member->value.GetInt()));
        ui->comboBoxRevScore->setCurrentIndex(index);
    }

    member = scoreSettings.FindMember("plugin");
    if (member != scoreSettings.MemberEnd()) {
        for (rapidjson::SizeType i = 0; i < member->value.Size(); ++i) {
            QList<QStandardItem *> list = m_scorePluginModel->findItems(member->value[i].GetString());
            if (list.size())
                list.first()->setCheckState(Qt::Checked);
        }
    }

    member = scoreSettings.FindMember("cluster");
    if (member != scoreSettings.MemberEnd()) {
        for (rapidjson::SizeType i = 0; i < member->value.Size(); ++i) {
            QList<QStandardItem *> list = qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model())->findItems(member->value[i].GetString());
            if (list.size())
                list.first()->setCheckState(Qt::Checked);
        }
    }

    member = scoreSettings.FindMember("failed-code-elements");
    if (member != scoreSettings.MemberEnd()) {
        QStandardItemModel *failedCodeElementModel = qobject_cast<QStandardItemModel*>(ui->listViewFailedCodeElements->model());
        for (rapidjson::SizeType i = 0; i < member->value.Size(); ++i) {
            QStandardItem *item = new QStandardItem(member->value[i].GetString());
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            failedCodeElementModel->appendRow(item);
        }
    }
}

void CMainWindow::updateAvailableClusters()
{
    QStandardItemModel *modelMetrics = qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model());
    QStandardItemModel *modelScore = qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model());
    QStandardItemModel *modelClusters = qobject_cast<QStandardItemModel*>(ui->listViewClusters->model());
    for (ClusterMap::const_iterator it = m_clusterList->getClusters().begin(); it != m_clusterList->getClusters().end(); ++it) {
        if (modelMetrics->findItems(tr(it->first.c_str())).size())
            continue;

        QStandardItem *item = new QStandardItem(tr(it->first.c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        modelMetrics->appendRow(item);
        modelScore->appendRow(item->clone());
        QStandardItem *clusterItem = new QStandardItem(tr(it->first.c_str()));
        clusterItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        modelClusters->appendRow(clusterItem);
        ui->comboBoxUncoveredGroup->addItem(tr(it->first.c_str()));
    }
}

void CMainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void CMainWindow::on_actionNewWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/
    createNewWorkspace();
}

void CMainWindow::on_actionLoadWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open workspace file"),
                                                    QString(),
                                                    tr("Unqlite database file (*.db);;All Files (*)"));
    if (!fileName.isEmpty()) {
        createNewWorkspace();
        m_workspace->setFileName(fileName);
        m_workspace->load();
        ui->statusBar->showMessage("Workspace loaded from file: " + m_workspace->getFileName(), 5000);
        updateLabels();
        updateAvailableClusters();
        updateMetricsConfiguration();
        updateScoreConfiguration();
    }
}

void CMainWindow::on_actionSaveWorkspace_triggered()
{
    if (saveWorkspace())
        ui->statusBar->showMessage("Workspace saved successfully to " + m_workspace->getFileName());
    else
        ui->statusBar->showMessage("Unable to save workspace to " + m_workspace->getFileName());
}

void CMainWindow::on_actionSaveWorkspaceAs_triggered()
{
    saveWorkspaceAs();
}

bool CMainWindow::saveWorkspace()
{
    if (m_workspace->getFileName().isEmpty())
        return saveWorkspaceAs();

    return m_workspace->save();
}

bool CMainWindow::saveWorkspaceAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("Unqlite database file (*.db);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    if (!fn.endsWith(".db", Qt::CaseInsensitive))
        fn += ".db"; // default
    m_workspace->setFileName(fn);
    return saveWorkspace();
}

void CMainWindow::on_buttonBrowseCov_clicked()
{
    QFileInfo fileInfo;
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Select coverage file"), QFileInfo(m_workspace->getCoveragePath()).filePath()));
    if (!fileInfo.exists())
        return;

    ui->labelCov->setText(fileInfo.fileName());
    m_workspace->setCoveragePath(fileInfo.filePath());
}

void CMainWindow::on_buttonBrowseRes_clicked()
{
    QFileInfo fileInfo;
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Select results file"), QFileInfo(m_workspace->getResultsPath()).filePath()));
    if (!fileInfo.exists())
        return;

    ui->labelRes->setText(fileInfo.fileName());
    m_workspace->setResultsPath(fileInfo.filePath());
}

void CMainWindow::on_buttonBrowseCha_clicked()
{
    QFileInfo fileInfo;
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Select changeset file"), QFileInfo(m_workspace->getChangesetPath()).filePath()));
    if (!fileInfo.exists())
        return;

    ui->labelChan->setText(fileInfo.fileName());
    m_workspace->setChangesetPath(fileInfo.filePath());
}

void CMainWindow::on_buttonLoad_clicked()
{
    CTestSuiteLoader *loadThread = new CTestSuiteLoader(this);
    connect(loadThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(loadThread, SIGNAL(processFinished(QString)), this, SLOT(loadFinished(QString)));
    connect(loadThread, SIGNAL(finished()), loadThread, SLOT(deleteLater()));
    m_statusProgressBar->setMaximum(0);
    loadThread->load(m_workspace);
}

void CMainWindow::loadFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
    m_testSuiteAvailableLabel->setVisible(true);
    m_workspace->setTestSuiteAvailable(true);

    QSortFilterProxyModel *filter = new QSortFilterProxyModel(this);
    filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filter->setSourceModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getCodeElements()));
    filter->setFilterKeyColumn(1);
    ui->tableViewCE->setModel(filter);

    filter = new QSortFilterProxyModel(this);
    filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
    filter->setSourceModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getTestcases()));
    filter->setFilterKeyColumn(1);
    ui->tableViewTests->setModel(filter);

    fillRevComboBoxes();
    updateMetricsConfiguration();
    updateScoreConfiguration();

    calculateStatistics();
}

void CMainWindow::on_buttonCalcCluster_clicked()
{
    String clusterPlugin = ui->comboBoxClusterPlugins->currentText().toStdString();
    rapidjson::Document params;
    params.SetObject();
    m_clusterPluginParameters[clusterPlugin]->getValues(params);

    CTestSuiteCluster *clusterThread = new CTestSuiteCluster(this);
    connect(clusterThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(clusterThread, SIGNAL(processFinished(QString)), this, SLOT(clusterFinished(QString)));
    connect(clusterThread, SIGNAL(finished()), clusterThread, SLOT(deleteLater()));
    m_statusProgressBar->setMaximum(0);
    clusterThread->createClusters(clusterPlugin, m_kernel, m_workspace->getTestSuite(), &m_clusterList->getClusters(), params);
}

void CMainWindow::clusterFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
    updateAvailableClusters();
}

void CMainWindow::on_buttonNewCluster_clicked()
{
    CClusterEditorDialog dia(this, m_workspace->getTestSuite(), &m_clusterList->getClusters());
    dia.exec();
}

void CMainWindow::on_buttonEditCluster_clicked()
{
    QModelIndexList list = ui->listViewClusters->selectionModel()->selectedIndexes();
    if (list.empty())
        return;

    QString cluster = list.first().data().toString();
    CClusterEditorDialog dia(this, m_workspace->getTestSuite(), &m_clusterList->getClusters(), cluster);
    dia.exec();
}

void CMainWindow::on_buttonDeleteCluster_clicked()
{
    QModelIndexList list = ui->listViewClusters->selectionModel()->selectedIndexes();
    if (list.empty())
        return;

    QString cluster = list.first().data().toString();
    QStandardItem *item = qobject_cast<QStandardItemModel*>(ui->listViewClusters->model())->findItems(cluster).first();
    ui->listViewClusters->model()->removeRow(item->index().row());
    item = qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model())->findItems(cluster).first();
    ui->listViewMetricsSelClu->model()->removeRow(item->index().row());
    item = qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model())->findItems(cluster).first();
    ui->listViewScoreSelClu->model()->removeRow(item->index().row());
    if (int id = ui->comboBoxUncoveredGroup->findText(cluster))
        ui->comboBoxUncoveredGroup->removeItem(id);
    m_clusterList->getClusters().erase(cluster.toStdString());
}

void CMainWindow::on_checkBoxMetricsSelectAll_stateChanged(int state)
{
    for (int i = 0; i < m_metricsPluginModel->rowCount(); ++i) {
        m_metricsPluginModel->item(i)->setCheckState(Qt::CheckState(state));
    }
}

void CMainWindow::on_buttonCalculateMetrics_clicked()
{
    StringVector metrics;
    for (int i = 0; i < m_metricsPluginModel->rowCount(); ++i) {
        if (m_metricsPluginModel->item(i)->checkState() == Qt::Checked)
            metrics.push_back(m_metricsPluginModel->item(i)->text().toStdString());
    }

    StringVector selectedClusters;
    QStandardItemModel *modelSelClusters = qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model());
    for (int i = 0; i < modelSelClusters->rowCount(); ++i) {
        if (modelSelClusters->item(i)->checkState() == Qt::Checked)
            selectedClusters.push_back(modelSelClusters->item(i)->text().toStdString());
    }

    CTestSuiteMetrics *metricsThread = new CTestSuiteMetrics(this);
    connect(metricsThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(metricsThread, SIGNAL(processFinished(QString)), this, SLOT(calcMetricsFinished(QString)));
    connect(metricsThread, SIGNAL(finished()), metricsThread, SLOT(deleteLater()));
    m_statusProgressBar->setMaximum(0);
    metricsThread->calculateMetrics(metrics, selectedClusters, ui->comboBoxRevMetrics->currentText().toInt(), this);
}

void CMainWindow::calcMetricsFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
}

void CMainWindow::tmpStatusUpdate(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
}

void CMainWindow::statusUpdate(QString label)
{
    m_statusLabel->setText(label);
}

void CMainWindow::calculateStatistics()
{
    if (m_workspace->isStatisticsCalculated())
        return;

    CStatisticsThread *statsThread = new CStatisticsThread(this);
    connect(statsThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(statsThread, SIGNAL(processFinished(QString)), this, SLOT(calcStatsFinished(QString)));
    connect(statsThread, SIGNAL(finished()), statsThread, SLOT(deleteLater()));
    m_statusProgressBar->setMaximum(0);
    statsThread->calcStatistics(m_workspace);
}

void CMainWindow::calcStatsFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
    m_workspace->getData(WS)->AddMember("statistics-calculated", true, m_workspace->getData(WS)->GetAllocator());
}

void CMainWindow::on_buttonAddFailedCodeElement_clicked()
{
    CClusterEditorAddDialog dia(this, m_workspace->getTestSuite()->getCodeElements(), qobject_cast<QStandardItemModel*>(ui->listViewFailedCodeElements->model()), true, true);
    dia.setWindowTitle("Add failed code elements");
    dia.exec();
}

void CMainWindow::on_buttonRemoveFailedCodeElement_clicked()
{
    QList<QPersistentModelIndex> indexes;
    foreach (const QModelIndex &i, ui->listViewFailedCodeElements->selectionModel()->selectedIndexes())
        indexes << i;

    foreach (const QPersistentModelIndex &i, indexes) {
        rapidjson::Value &settings = (*m_workspace->getMeasurement(SCORE, getScoreMeasurement()));;
        for (rapidjson::Value::ValueIterator it = settings["failed-code-elements"].Begin(); it != settings["failed-code-elements"].End(); ++it) {
            if (it->GetString() != i.data().toString())
                continue;

            rapidjson::Value::ValueIterator last = settings["failed-code-elements"].End() - 1;
            it->Swap(*last);
            settings["failed-code-elements"].PopBack();
            break;
        }
        ui->listViewFailedCodeElements->model()->removeRow(i.row());
    }
}

void CMainWindow::on_buttonScoreCalc_clicked()
{
    StringVector flTechnique;
    for (int i = 0; i < m_scorePluginModel->rowCount(); ++i) {
        if (m_scorePluginModel->item(i)->checkState() == Qt::Checked)
            flTechnique.push_back(m_scorePluginModel->item(i)->text().toStdString());
    }

    StringVector selectedClusters;
    QStandardItemModel *modelSelClusters = qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model());
    for (int i = 0; i < modelSelClusters->rowCount(); ++i) {
        if (modelSelClusters->item(i)->checkState() == Qt::Checked)
            selectedClusters.push_back(modelSelClusters->item(i)->text().toStdString());
    }

    IntVector failedCodeElements;
    QStandardItemModel *modelFailedCodeElements = qobject_cast<QStandardItemModel*>(ui->listViewFailedCodeElements->model());
    for (int i = 0; i < modelFailedCodeElements->rowCount(); ++i) {
        IndexType ceid = m_workspace->getTestSuite()->getCodeElements()->getID(modelFailedCodeElements->item(i)->text().toStdString());
        failedCodeElements.push_back(ceid);
    }

    CFLScore *scoreThread = new CFLScore(this);
    connect(scoreThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(scoreThread, SIGNAL(processFinished(QString)), this, SLOT(calcScoreFinished(QString)));
    connect(scoreThread, SIGNAL(finished()), scoreThread, SLOT(deleteLater()));
    m_statusProgressBar->setMaximum(0);
    scoreThread->calculateScore(ui->comboBoxRevScore->currentText().toInt(), flTechnique, selectedClusters, failedCodeElements, this);
}

void CMainWindow::calcScoreFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
}

void CMainWindow::on_tabWidgetStatistics_currentChanged(int index)
{
    if (!index || !m_workspace->isStatisticsCalculated())
        return;

    QWidget* widget = ui->tabWidgetStatistics->widget(index);
    QList<QWebView*> webView = widget->findChildren<QWebView*>();
    if (webView.count() == 1 && webView[0]->title().isEmpty()) {
        CShowStatistics stats(m_workspace);
        stats.generateChartForTab(webView[0], index - 1);
    }
}

void CMainWindow::on_tabWidgetMain_currentChanged(int index)
{
    if (ui->tabWidgetMain->tabBar()->tabText(index) == "Statistics" && ui->webViewGen->title().isEmpty()
            && m_workspace->isStatisticsCalculated()) {
        CShowStatistics stats(m_workspace);
        stats.fillGeneralTab(ui->webViewGen);
    }
}

void CMainWindow::on_tabWidgetCluster_currentChanged(int index)
{
    if (ui->tabWidgetCluster->tabBar()->tabText(index) == "Statistics") {
        CShowClusters clusters;
        clusters.generateCharts(m_clusterList->getClusters(), ui->webViewCluster);
    }
}

void CMainWindow::on_tabWidgetMetrics_currentChanged(int index)
{
    QString tabText = ui->tabWidgetMetrics->tabBar()->tabText(index);
    CShowMetrics metrics(*m_workspace->getMeasurementResults(METRICS, getMetricMeasurement()), m_clusterList->getClusters());
    if (tabText == "Results")
        metrics.generateResults(ui->webViewResults);
    else if (tabText == "Charts")
        metrics.generateCharts(ui->webViewCharts);
}

void CMainWindow::on_tabWidgetScore_currentChanged(int index)
{
    QString tabText = ui->tabWidgetScore->tabBar()->tabText(index);
    if (tabText == "Scores") {
        CShowScores scores;
        scores.generateResults(ui->webViewScores, m_workspace->getMeasurementResults(SCORE, getScoreMeasurement()), m_workspace->getTestSuite());
    }
}

bool CMainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->buttonLoad && event->type() == QEvent::MouseButtonRelease) {
        if (m_workspace->getCoveragePath().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing coverage file path.");
            return true;
        } else if (!QFileInfo(m_workspace->getCoveragePath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing coverage file.");
            return true;
        } else if (!m_workspace->getResultsPath().isEmpty() && !QFileInfo(m_workspace->getResultsPath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing results file.");
            return true;
        } else if (!m_workspace->getChangesetPath().isEmpty() && !QFileInfo(m_workspace->getChangesetPath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing changes file.");
            return true;
        }
    } else if (object == ui->buttonCalculateMetrics && event->type() == QEvent::MouseButtonRelease) {
        QString resultsMetrics = checkMetricsPluginsRequirements();
        if (!isTestSuiteAvailable()) {
            QMessageBox::critical(this, "Error", "Test suite not available.");
            return true;
        } else if (!(m_workspace->getFileMask() & FILE_RESULTS) && !resultsMetrics.isEmpty()) {
            QMessageBox::critical(this, "Error", "Results file required for metric: " + resultsMetrics);
            return true;
        } else if (!resultsMetrics.isEmpty() && ui->comboBoxRevMetrics->currentIndex() == -1) {
            QMessageBox::critical(this, "Error", "Missing revision number.");
            return true;
        }
    } else if (object == ui->buttonScoreCalc && event->type() == QEvent::MouseButtonRelease) {
        if (!isTestSuiteAvailable()) {
            QMessageBox::critical(this, "Error", "Test suite not available.");
            return true;
        } else if (!(m_workspace->getFileMask() & FILE_RESULTS)) {
            QMessageBox::critical(this, "Error", "Missing results file.");
            return true;
        } else if (ui->comboBoxRevScore->currentIndex() == -1) {
            QMessageBox::critical(this, "Error", "Missing revision number.");
            return true;
        }
    }
    return false;
}

void CMainWindow::on_comboBoxClusterPlugins_currentIndexChanged(const QString &plugin)
{
    updateClusterParameters(plugin);
}

void CMainWindow::on_comboBoxRevMetrics_currentIndexChanged(const QString &text)
{
    if (text.isEmpty())
        return;

    rapidjson::Value &settings = (*m_workspace->getMeasurement(METRICS, getMetricMeasurement()));
    RevNumType rev = text.toInt();
    rapidjson::Value::MemberIterator revIt = settings.FindMember("revision");
    if (revIt == settings.MemberEnd())
        settings.AddMember("revision", rev, m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
    else
        revIt->value.SetInt(rev);
}

void CMainWindow::metricsPluginStateChanged(QStandardItem *item)
{
    String metricName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getMeasurement(METRICS, getMetricMeasurement()));
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("plugin")) {
            settings.AddMember("plugin", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(metricName.c_str(), m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
        settings["plugin"].PushBack(val, m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
    } else {
        if (!settings.HasMember("plugin"))
            return;
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;

            rapidjson::Value::ValueIterator end = settings["plugin"].End() - 1;
            it->Swap(*end);
            settings["plugin"].PopBack();
            break;
        }
    }
}

void CMainWindow::metricsClusterStateChanged(QStandardItem *item)
{
    String clusterName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getMeasurement(METRICS, getMetricMeasurement()));
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("cluster")) {
            settings.AddMember("cluster", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(clusterName.c_str(), m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
        settings["cluster"].PushBack(val, m_workspace->getMeasurement(METRICS, getMetricMeasurement())->GetAllocator());
    } else {
        if (!settings.HasMember("cluster"))
            return;

        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;

            rapidjson::Value::ValueIterator last = settings["cluster"].End() - 1;
            it->Swap(*last);
            settings["cluster"].PopBack();
            break;
        }
    }
}

void CMainWindow::on_comboBoxRevScore_currentIndexChanged(const QString &text)
{
    if (text.isEmpty())
        return;

    rapidjson::Value &settings = (*m_workspace->getMeasurement(SCORE, getScoreMeasurement()));
    RevNumType rev = text.toInt();
    rapidjson::Value::MemberIterator revIt = settings.FindMember("revision");
    if (revIt == settings.MemberEnd())
        settings.AddMember("revision", rev, m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
    else
        revIt->value.SetInt(rev);
}

void CMainWindow::scorePluginStateChanged(QStandardItem *item)
{
    String metricName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getMeasurement(SCORE, getScoreMeasurement()));
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("plugin")) {
            settings.AddMember("plugin", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(metricName.c_str(), m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
        settings["plugin"].PushBack(val, m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
    } else {
        if (!settings.HasMember("plugin"))
            return;
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;

            rapidjson::Value::ValueIterator end = settings["plugin"].End() - 1;
            it->Swap(*end);
            settings["plugin"].PopBack();
            break;
        }
    }
}

void CMainWindow::scoreClusterStateChanged(QStandardItem *item)
{
    String clusterName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getMeasurement(SCORE, getScoreMeasurement()));
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("cluster")) {
            settings.AddMember("cluster", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(clusterName.c_str(), m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
        settings["cluster"].PushBack(val, m_workspace->getMeasurement(SCORE, getScoreMeasurement())->GetAllocator());
    } else {
        if (!settings.HasMember("cluster"))
            return;
        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;

            rapidjson::Value::ValueIterator last = settings["cluster"].End() - 1;
            it->Swap(*last);
            settings["cluster"].PopBack();
            break;
        }
    }
}

void CMainWindow::on_comboBoxMetricsMeasurement_currentIndexChanged(int index)
{
    if (index == -1)
        return;

    updateMetricsConfiguration();
}

void CMainWindow::on_toolButtonMetricMeasAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New measurement"),
                                         tr("Measurement name:"), QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty() && ui->comboBoxMetricsMeasurement->findText(text) == -1) {
        m_workspace->addMeasurement(METRICS, text.toStdString());
        ui->comboBoxMetricsMeasurement->addItem(text);
    }
}

void CMainWindow::on_toolButtonMetricMeasRem_clicked()
{
    m_workspace->removeMeasurement(METRICS, ui->comboBoxMetricsMeasurement->currentText().toStdString());
    ui->comboBoxMetricsMeasurement->removeItem(ui->comboBoxMetricsMeasurement->currentIndex());
}

void CMainWindow::on_comboBoxScoreMeasurement_currentIndexChanged(int index)
{
    if (index == -1)
        return;

    updateScoreConfiguration();
}

void CMainWindow::on_toolButtonScoreMeasAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New measurement"),
                                         tr("Measurement name:"), QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty() && ui->comboBoxScoreMeasurement->findText(text) == -1) {
        m_workspace->addMeasurement(SCORE, text.toStdString());
        ui->comboBoxScoreMeasurement->addItem(text);
    }
}

void CMainWindow::on_toolButtonScoreMeasRem_clicked()
{
    m_workspace->removeMeasurement(SCORE, ui->comboBoxScoreMeasurement->currentText().toStdString());
    ui->comboBoxScoreMeasurement->removeItem(ui->comboBoxScoreMeasurement->currentIndex());
}

void CMainWindow::on_lineEditFilterTests_textEdited(const QString &text)
{
    if (ui->tableViewTests->model() == NULL)
        return;
    qobject_cast<QSortFilterProxyModel*>(ui->tableViewTests->model())->setFilterFixedString(text);
}

void CMainWindow::on_lineEditFilterCE_textEdited(const QString &text)
{
    if (ui->tableViewCE->model() == NULL)
        return;
    qobject_cast<QSortFilterProxyModel*>(ui->tableViewCE->model())->setFilterFixedString(text);
}

void CMainWindow::on_tableViewTests_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tableViewTests->selectionModel() == NULL || !ui->tableViewTests->selectionModel()->currentIndex().isValid())
        return;

    QPoint globalPos = ui->tableViewTests->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Covered code elements", this, SLOT(actionCoveredCodeElements_triggered()));
    myMenu.exec(globalPos);
}

void CMainWindow::actionCoveredCodeElements_triggered()
{
    IndexType id = m_workspace->getTestSuite()->getCoverage()->getTestcases().getID(ui->tableViewTests->selectionModel()->currentIndex().data().toString().toStdString());
    CCoveredListDialog dia(this, m_workspace->getTestSuite(), id, true);
    dia.setWindowTitle("List of code elements covered by " + ui->tableViewTests->selectionModel()->currentIndex().data().toString() + " test");
    dia.exec();
}

void CMainWindow::on_tableViewCE_customContextMenuRequested(const QPoint &pos)
{
    if (ui->tableViewCE->selectionModel() == NULL || !ui->tableViewCE->selectionModel()->currentIndex().isValid())
        return;

    QPoint globalPos = ui->tableViewCE->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction("Covered tests", this, SLOT(actionCoveredTests_triggered()));
    myMenu.exec(globalPos);
}

void CMainWindow::actionCoveredTests_triggered()
{
    IndexType id = m_workspace->getTestSuite()->getCoverage()->getCodeElements().getID(ui->tableViewCE->selectionModel()->currentIndex().data().toString().toStdString());
    CCoveredListDialog dia(this, m_workspace->getTestSuite(), id, false);
    dia.setWindowTitle("List of test cases covered by " + ui->tableViewCE->selectionModel()->currentIndex().data().toString() + " code element");
    dia.exec();
}

void CMainWindow::on_actionMetrics_results_triggered()
{
    QStringList measurements;
    for (int i = 0; i < ui->comboBoxMetricsMeasurement->count(); ++i) {
        measurements << ui->comboBoxMetricsMeasurement->itemText(i);
    }

    CResultsExportDialog dia(this, measurements, m_workspace, true);
    dia.setWindowTitle("Export metrics results");
    dia.exec();
}

void CMainWindow::on_actionFault_localization_results_triggered()
{
    QStringList measurements;
    for (int i = 0; i < ui->comboBoxScoreMeasurement->count(); ++i) {
        measurements << ui->comboBoxScoreMeasurement->itemText(i);
    }

    CResultsExportDialog dia(this, measurements, m_workspace, false);
    dia.setWindowTitle("Export fault-localization results");
    dia.exec();
}

void CMainWindow::on_comboBoxUncoveredGroup_currentIndexChanged(const QString &clusterName)
{
    if (clusterName.isEmpty())
        return;

    CClusterDefinition cluster = m_clusterList->getClusters()[clusterName.toStdString()];
    std::set<IndexType> coveredElementIds;
    IndexType nrOfCodeElements = cluster.getCodeElements().size();
    IndexType nrOfTestcases = cluster.getTestCases().size();
    coveredElementIds.clear();
    for (IndexType j = 0; j < nrOfCodeElements; j++) {
        coveredElementIds.insert(cluster.getCodeElements().at(j));
    }
    for (IndexType j = 0; j < nrOfTestcases; j++) {
        IndexType tcid = cluster.getTestCases().at(j);
        std::set<IndexType>::iterator it = coveredElementIds.begin();
        while (it != coveredElementIds.end()) {
            IndexType cid = *it;
            if (m_workspace->getTestSuite()->getCoverage()->getBitMatrix().get(tcid, cid)) {
                coveredElementIds.erase(it++);
            } else {
                it++;
            }
        }
    }
    QStringList list;
    for (std::set<IndexType>::const_iterator it = coveredElementIds.begin(); it != coveredElementIds.end(); ++it) {
        list.append(m_workspace->getTestSuite()->getCoverage()->getCodeElements().getValue(*it).c_str());
    }

    QStringListModel *model = qobject_cast<QStringListModel*>(ui->listViewUncovered->model());
    model->setStringList(list);
}
