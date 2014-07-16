#include "CMainWindow.h"
#include "CShowStatistics.h"
#include "CShowClusters.h"
#include "CShowMetrics.h"
#include "CIDManagerTableModel.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"

#include <QFileDialog>
#include <QLayout>
#include <QStatusBar>
#include <QCheckBox>
#include <QWebFrame>

#include <iostream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::CMainWindow),
    m_revCompleter(NULL), m_kernel(new CKernel())
{
    ui->setupUi(this);
    setFixedSize(size());

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
    delete m_metricsThread;
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

    ui->listViewFaultlocalization->setModel(m_scorePluginModel);
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

void CMainWindow::on_buttonClusterTestList_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open cluster test list file"),
                                                    QString(),
                                                    tr("All Files (*)"));
    if (fileName.isEmpty())
        return;

    rapidjson::Document *workspace = m_workspace->getResultsByName(WS);
    rapidjson::Value s;
    s.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
    workspace->AddMember("cluster-test-list", s, workspace->GetAllocator());
    ui->labelClusterTestList->setText(fileName);
}

void CMainWindow::on_buttonClusterCEList_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open cluster code elements list file"),
                                                    QString(),
                                                    tr("All Files (*)"));
    if (fileName.isEmpty())
        return;

    rapidjson::Document *workspace = m_workspace->getResultsByName(WS);
    rapidjson::Value s;
    s.SetString(fileName.toStdString().c_str(), fileName.length(), workspace->GetAllocator());
    workspace->AddMember("cluster-code-elements-list", s, workspace->GetAllocator());
    ui->labelClusterCodeElementList->setText(fileName);
}

void CMainWindow::on_buttonCalcCluster_clicked()
{
    m_clusterList->createClusters();
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

    m_metricsThread = new CTestSuiteMetrics(this);
    connect(m_metricsThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(m_metricsThread, SIGNAL(processFinished(QString)), this, SLOT(calcMetricsFinished(QString)));
    m_statusProgressBar->setMaximum(0);
    m_metricsThread->calculateMetrics(metrics, (IndexType)ui->lineEditRevisionMetrics->text().toLongLong(), this);
}

void CMainWindow::calcMetricsFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(0);
    m_statusLabel->clear();
    delete m_metricsThread;
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
    m_loadThread = new CTestSuiteLoader(this);
    connect(m_loadThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(m_loadThread, SIGNAL(processFinished(QString)), this, SLOT(loadFinished(QString)));
    m_statusProgressBar->setMaximum(0);
    m_loadThread->load(m_workspace);
}

void CMainWindow::statusUpdate(QString label)
{
    m_statusLabel->setText(label);
}

void CMainWindow::loadFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
    delete m_loadThread;
    m_testSuiteAvailableLabel->setVisible(true);

    ui->tableViewTests->setModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getTestcases()));
    ui->tableViewCE->setModel(new CIDManagerTableModel(this, m_workspace->getTestSuite()->getCodeElements()));
    createCompleterForMetrics();
    calculateStatistics();
}

void CMainWindow::calculateStatistics()
{
    if (m_workspace->isStatisticsCalculated())
        return;

    m_statsThread = new CStatisticsThread(this);
    connect(m_statsThread, SIGNAL(updateStatusLabel(QString)), this, SLOT(statusUpdate(QString)));
    connect(m_statsThread, SIGNAL(processFinished(QString)), this, SLOT(calcStatsFinished(QString)));
    m_statusProgressBar->setMaximum(0);
    m_statsThread->calcStatistics(m_workspace);
}

void CMainWindow::calcStatsFinished(QString msg)
{
    ui->statusBar->showMessage(msg, 5000);
    m_statusProgressBar->setMaximum(1);
    m_statusLabel->clear();
    delete m_statsThread;
    m_workspace->getResultsByName(WS)->AddMember("statistics-calculated", true, m_workspace->getResultsByName(WS)->GetAllocator());
}

void CMainWindow::createCompleterForMetrics()
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
    if (ui->tabWidgetCluster->tabBar()->tabText(index) == "Data") {
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
