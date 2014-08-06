#include "CClusterEditorAddDialog.h"
#include "CMainWindow.h"
#include "CShowStatistics.h"
#include "CShowClusters.h"
#include "CShowMetrics.h"
#include "CShowScores.h"
#include "CClusterEditorDialog.h"
#include "CIDManagerTableModel.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QLayout>
#include <QStatusBar>
#include <QCheckBox>
#include <QWebFrame>
#include <QInputDialog>

#include <iostream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::CMainWindow),
    m_revCompleter(NULL), m_kernel(new CKernel())
{
    ui->setupUi(this);

    ui->buttonLoad->installEventFilter(this);
    ui->buttonCalculateMetrics->installEventFilter(this);
    ui->buttonScoreCalc->installEventFilter(this);

    m_workspace = new CWorkspace(this);
    m_clusterList = new CClusterList(this);

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
    for (StringVector::const_iterator it = clusterPlugins.begin(); it != clusterPlugins.end(); ++it) {
        m_clusterAlgorithms << QString((*it).c_str());
    }
    ui->comboBoxClusterPlugins->addItems(m_clusterAlgorithms);

    // fill listview with metric algorithm names
    m_metricsPluginModel = new QStandardItemModel();

    StringVector metricPlugins = m_kernel->getTestSuiteMetricPluginManager().getPluginNames();
    for (int i = 0; i < metricPlugins.size(); ++i) {
        QStandardItem *item = new QStandardItem(tr(metricPlugins[i].c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
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
        m_scorePluginModel->appendRow(item);
    }

    connect(m_scorePluginModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scorePluginStateChanged(QStandardItem*)));
    ui->listViewFaultlocalization->setModel(m_scorePluginModel);

    ui->listViewMetricsSelClu->setModel(new QStandardItemModel(this));
    connect(qobject_cast<QStandardItemModel*>(ui->listViewMetricsSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(metricsClusterStateChanged(QStandardItem*)));
    ui->listViewScoreSelClu->setModel(new QStandardItemModel(this));
    connect(qobject_cast<QStandardItemModel*>(ui->listViewScoreSelClu->model()), SIGNAL(itemChanged(QStandardItem*)), this, SLOT(scoreClusterStateChanged(QStandardItem*)));
    ui->listViewClusters->setModel(new QStandardItemModel(this));

    ui->lineEditRevisionMetrics->setValidator(new QIntValidator(ui->lineEditRevisionMetrics));
    ui->lineEditScoreRevision->setValidator(new QIntValidator(ui->lineEditScoreRevision));
    ui->listViewFailedCodeElements->setModel(new QStandardItemModel(this));
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
    rapidjson::Document &res = *m_workspace->getResultsByName(WS);
    QFileInfo fileInfo;
    fileInfo.setFile(res.HasMember("cluster-code-elements-list") ? res["cluster-code-elements-list"].GetString() : "");
    if (fileInfo.exists())
        ui->labelClusterCodeElementList->setText(fileInfo.fileName());
    fileInfo.setFile(res.HasMember("cluster-test-list") ? res["cluster-test-list"].GetString() : "");
    if (fileInfo.exists())
        ui->labelClusterTestList->setText(fileInfo.fileName());

    fileInfo.setFile(m_workspace->getCoveragePath());
    if (fileInfo.exists())
        ui->labelCov->setText(fileInfo.fileName());
    fileInfo.setFile(m_workspace->getResultsPath());
    if (fileInfo.exists())
        ui->labelRes->setText(fileInfo.fileName());
    fileInfo.setFile(m_workspace->getChangesetPath());
    if (fileInfo.exists())
        ui->labelChan->setText(fileInfo.fileName());
}

void CMainWindow::updateConfigurations()
{
    rapidjson::Value &metricsSettings = (*m_workspace->getResultsByName(WS))["metrics"];
    rapidjson::Value::MemberIterator member = metricsSettings.FindMember("revision");
    if (member != metricsSettings.MemberEnd())
        ui->lineEditRevisionMetrics->setText(QString::number(member->value.GetInt()));

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

    rapidjson::Value &scoreSettings = (*m_workspace->getResultsByName(WS))["score"];
    member = scoreSettings.FindMember("revision");
    if (member != scoreSettings.MemberEnd())
        ui->lineEditScoreRevision->setText(QString::number(member->value.GetInt()));

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
    }
}

void CMainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void CMainWindow::on_actionDumpCoverage_triggered()
{
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
        s.Clear();
        m_workspace->getResultsByName(WS)->Accept(writer);
        std::cout << s.GetString() << std::endl;
}

void CMainWindow::on_actionNewWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/
    delete m_workspace;
    m_workspace = new CWorkspace(this);
}

void CMainWindow::on_actionLoadWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open workspace file"),
                                                    QString(),
                                                    tr("Unqlite database file (*.db);;All Files (*)"));
    if (!fileName.isEmpty()) {
        delete m_workspace;
        m_workspace = new CWorkspace(this);
        m_workspace->setFileName(fileName);
        m_workspace->load();
        ui->statusBar->showMessage("Workspace loaded from file: " + m_workspace->getFileName(), 5000);
        updateLabels();
        updateAvailableClusters();
        updateConfigurations();
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
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Open coverage file")));
    if (!fileInfo.exists())
        return;

    ui->labelCov->setText(fileInfo.fileName());
    m_workspace->setCoveragePath(fileInfo.filePath());
}

void CMainWindow::on_buttonBrowseRes_clicked()
{
    QFileInfo fileInfo;
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Open results file")));
    if (!fileInfo.exists())
        return;

    ui->labelRes->setText(fileInfo.fileName());
    m_workspace->setResultsPath(fileInfo.filePath());
}

void CMainWindow::on_buttonBrowseCha_clicked()
{
    QFileInfo fileInfo;
    fileInfo.setFile(QFileDialog::getOpenFileName(this, tr("Open changeset file")));
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

    ui->tableViewTests->setModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getTestcases()));
    ui->tableViewCE->setModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getCodeElements()));
    createRevisionCompleter();
    calculateStatistics();
}

void CMainWindow::on_buttonClusterTestList_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open cluster test list file"),
                                                    QString(),
                                                    tr("All Files (*)"));
    if (fileName.isEmpty())
        return;

    rapidjson::Document *workspace = m_workspace->getResultsByName(WS);
    rapidjson::Document::MemberIterator memberIt = workspace->FindMember("cluster-test-list");
    if (memberIt == workspace->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
        workspace->AddMember("cluster-test-list", s, workspace->GetAllocator());
    } else
        memberIt->value.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
    ui->labelClusterTestList->setText(QFileInfo(fileName).fileName());
}

void CMainWindow::on_buttonClusterCEList_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open cluster code elements list file"),
                                                    QString(),
                                                    tr("All Files (*)"));
    if (fileName.isEmpty())
        return;

    rapidjson::Document *workspace = m_workspace->getResultsByName(WS);
    rapidjson::Document::MemberIterator memberIt = workspace->FindMember("cluster-code-elements-list");
    if (memberIt == workspace->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
        workspace->AddMember("cluster-code-elements-list", s, workspace->GetAllocator());
    } else
        memberIt->value.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
    ui->labelClusterCodeElementList->setText(QFileInfo(fileName).fileName());
}

void CMainWindow::on_buttonCalcCluster_clicked()
{
    m_clusterList->createClusters();
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
    metricsThread->calculateMetrics(metrics, selectedClusters, (IndexType)ui->lineEditRevisionMetrics->text().toLongLong(), this);
}

void CMainWindow::calcMetricsFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
}

void CMainWindow::statusUpdate(QString label)
{
    m_statusLabel->setText(label);
}

void CMainWindow::createRevisionCompleter()
{
    if (m_revCompleter) {
        delete m_revCompleter;
        m_revCompleter = NULL;
    }

    QStandardItemModel *revs = new QStandardItemModel(this);
    IntVector revsVec = m_workspace->getTestSuite()->getResults()->getRevisionNumbers();
    for (IntVector::const_iterator it = revsVec.begin(); it != revsVec.end(); ++it) {
        revs->appendRow(new QStandardItem(QString::number(*it)));
    }
    m_revCompleter = new QCompleter(revs, this);
    m_revCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditRevisionMetrics->setCompleter(m_revCompleter);
    ui->lineEditScoreRevision->setCompleter(m_revCompleter);
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
    m_workspace->getResultsByName(WS)->AddMember("statistics-calculated", true, m_workspace->getResultsByName(WS)->GetAllocator());
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
        rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["score"];
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
    scoreThread->calculateScore((IndexType)ui->lineEditScoreRevision->text().toLongLong(), flTechnique, selectedClusters, failedCodeElements, this);
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
    CShowMetrics metrics(*m_workspace->getResultsByName(METRICS), m_clusterList->getClusters());
    if (tabText == "Results")
        metrics.generateResults(ui->webViewResults);
    else if (tabText == "Charts")
        metrics.generateCharts(ui->webViewCharts);
    else if (tabText == "Heat map")
        metrics.generateHeatMap(ui->webViewHeatmap);

}

void CMainWindow::on_tabWidgetScore_currentChanged(int index)
{
    QString tabText = ui->tabWidgetScore->tabBar()->tabText(index);
    if (tabText == "Scores") {
        CShowScores scores;
        scores.generateResults(ui->webViewScores, m_workspace->getResultsByName(SCORE), m_workspace->getTestSuite());
    }
}

bool CMainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->buttonLoad && event->type() == QEvent::MouseButtonRelease) {
        if (m_workspace->getCoveragePath().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing coverage file path.");
            return true;
        }
        else if (!QFileInfo(m_workspace->getCoveragePath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing coverage file.");
            return true;
        }
        else if (m_workspace->getResultsPath().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing results file path.");
            return true;
        }
        else if (!QFileInfo(m_workspace->getResultsPath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing results file.");
            return true;
        }
        else if (!m_workspace->getChangesetPath().isEmpty() && !QFileInfo(m_workspace->getChangesetPath()).exists()) {
            QMessageBox::critical(this, "Error", "Not existing changes file.");
            return true;
        }
    }
    else if (object == ui->buttonCalculateMetrics && event->type() == QEvent::MouseButtonRelease) {
        if (ui->lineEditRevisionMetrics->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing revision number.");
            return true;
        }
    }
    else if (object == ui->buttonScoreCalc && event->type() == QEvent::MouseButtonRelease) {
        if (ui->lineEditScoreRevision->text().isEmpty()) {
            QMessageBox::critical(this, "Error", "Missing revision number.");
            return true;
        }
    }
    return false;
}

void CMainWindow::on_comboBoxClusterPlugins_currentIndexChanged(const QString &plugin)
{
    if (plugin != "label-test-codeelements") {
        ui->buttonClusterCEList->hide();
        ui->labelClusterCodeElementList->hide();
        ui->labelClusterCodeElementListTitle->hide();
        ui->buttonClusterTestList->hide();
        ui->labelClusterTestList->hide();
        ui->labelClusterTestListTitle->hide();
    } else {
        ui->buttonClusterCEList->show();
        ui->labelClusterCodeElementList->show();
        ui->labelClusterCodeElementListTitle->show();
        ui->buttonClusterTestList->show();
        ui->labelClusterTestList->show();
        ui->labelClusterTestListTitle->show();
    }
}

void CMainWindow::on_lineEditRevisionMetrics_textEdited(const QString &text)
{
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["metrics"];
    RevNumType rev = text.toInt();
    rapidjson::Value::MemberIterator revIt = settings.FindMember("revision");
    if (revIt == settings.MemberEnd())
        settings.AddMember("revision", rev, m_workspace->getResultsByName(WS)->GetAllocator());
    else if (text.isEmpty())
        settings.RemoveMember(revIt);
    else
        revIt->value.SetInt(rev);
}

void CMainWindow::metricsPluginStateChanged(QStandardItem *item)
{
    String metricName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["metrics"];
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("plugin")) {
            settings.AddMember("plugin", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getResultsByName(WS)->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(metricName.c_str(), m_workspace->getResultsByName(WS)->GetAllocator());
        settings["plugin"].PushBack(val, m_workspace->getResultsByName(WS)->GetAllocator());
    } else {
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
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["metrics"];
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("cluster")) {
            settings.AddMember("cluster", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getResultsByName(WS)->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(clusterName.c_str(), m_workspace->getResultsByName(WS)->GetAllocator());
        settings["cluster"].PushBack(val, m_workspace->getResultsByName(WS)->GetAllocator());
    } else {
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

void CMainWindow::on_lineEditScoreRevision_textEdited(const QString &text)
{
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["score"];
    RevNumType rev = text.toInt();
    rapidjson::Value::MemberIterator revIt = settings.FindMember("revision");
    if (revIt == settings.MemberEnd())
        settings.AddMember("revision", rev, m_workspace->getResultsByName(WS)->GetAllocator());
    else if (text.isEmpty())
        settings.RemoveMember(revIt);
    else
        revIt->value.SetInt(rev);
}

void CMainWindow::scorePluginStateChanged(QStandardItem *item)
{
    String metricName = item->text().toStdString();
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["score"];
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("plugin")) {
            settings.AddMember("plugin", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getResultsByName(WS)->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["plugin"].Begin(); it != settings["plugin"].End(); ++it) {
            if (it->GetString() != metricName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(metricName.c_str(), m_workspace->getResultsByName(WS)->GetAllocator());
        settings["plugin"].PushBack(val, m_workspace->getResultsByName(WS)->GetAllocator());
    } else {
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
    rapidjson::Value &settings = (*m_workspace->getResultsByName(WS))["score"];
    if (item->checkState() == Qt::Checked) {
        if (!settings.HasMember("cluster")) {
            settings.AddMember("cluster", rapidjson::Value(rapidjson::kArrayType).Move(), m_workspace->getResultsByName(WS)->GetAllocator());
        }
        for (rapidjson::Value::ValueIterator it = settings["cluster"].Begin(); it != settings["cluster"].End(); ++it) {
            if (it->GetString() != clusterName)
                continue;
            return;
        }
        rapidjson::Value val(rapidjson::kStringType);
        val.SetString(clusterName.c_str(), m_workspace->getResultsByName(WS)->GetAllocator());
        settings["cluster"].PushBack(val, m_workspace->getResultsByName(WS)->GetAllocator());
    } else {
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

void CMainWindow::on_toolButtonMetricMeasAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New measurement"),
                                         tr("Measurement name:"), QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty() && ui->comboBoxMetricsMeasuremement->findText(text) == -1)
        ui->comboBoxMetricsMeasuremement->addItem(text);
}


void CMainWindow::on_toolButtonMetricMeasRem_clicked()
{
    ui->comboBoxMetricsMeasuremement->removeItem(ui->comboBoxMetricsMeasuremement->currentIndex());
}

void CMainWindow::on_toolButtonScoreMeasAdd_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New measurement"),
                                         tr("Measurement name:"), QLineEdit::Normal,
                                         QString(), &ok);
    if (ok && !text.isEmpty() && ui->comboBoxScoreMeasurement->findText(text) == -1)
        ui->comboBoxScoreMeasurement->addItem(text);
}

void CMainWindow::on_toolButtonScoreMeasRem_clicked()
{
    ui->comboBoxScoreMeasurement->removeItem(ui->comboBoxScoreMeasurement->currentIndex());
}
