#include "CMainWindow.h"
#include "CShowStatisticsDialog.h"
#include "CShowClusters.h"
#include "CShowMetrics.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"

#include <QFileDialog>
#include <QLayout>
#include <QStatusBar>
#include <QCheckBox>

#include <iostream>

#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    m_kernel(new CKernel())
{
    ui->setupUi(this);
    setFixedSize(size());

    m_workspace = new CWorkspace(this);
    m_clusterList = new CClusterList(this);
    m_metrics = new CTestSuiteMetrics(this);

    ui->textBrowserOutput->hide();
    createStatusBar();
    fillWidgets();
}

CMainWindow::~CMainWindow()
{
    delete ui;
    delete m_progressBar;
    delete m_workspace;
    delete m_kernel;
    delete m_clusterList;
    delete m_metrics;
    delete m_model;
}

void CMainWindow::fillWidgets()
{
    // fill combobox with cluster algorithms
    StringVector clusterPlugins = m_kernel->getTestSuiteClusterPluginManager().getPluginNames();
    for (StringVector::const_iterator it = clusterPlugins.begin(); it != clusterPlugins.end(); ++it) {
        m_clusterAlgorithms << QString((*it).c_str());
    }
    ui->comboBoxClusterPlugins->addItems(m_clusterAlgorithms);

    // fill combobox with metric algorithm names
    m_model = new QStandardItemModel();

    StringVector metricPlugins = m_kernel->getTestSuiteMetricPluginManager().getPluginNames();
    for (int i = 0; i < metricPlugins.size(); ++i) {
        QStandardItem *item = new QStandardItem(tr(metricPlugins[i].c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        m_model->appendRow(item);
    }

    ui->listViewMetrics->setModel(m_model);
}

void CMainWindow::createStatusBar()
{
    ui->statusBar->setSizeGripEnabled(false);
    m_progressBar = new QProgressBar(ui->statusBar);
    m_progressBar->setAlignment(Qt::AlignRight);
    m_progressBar->setMaximumSize(180, 19);
    m_progressBar->setValue(0);
    ui->statusBar->addPermanentWidget(m_progressBar);
}

void CMainWindow::updateLabels()
{
    rapidjson::Document &res = *m_workspace->getResultsByName(WS);
    QFileInfo fileInfo;
    fileInfo.setFile(res.HasMember("cluster-code-elements-list") ? res["cluster-code-elements-list"].GetString() : "");
    if (fileInfo.exists())
        ui->labelClusterCodeeElementList->setText(fileInfo.fileName());
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
        ui->textBrowserOutput->append("Workspace loaded from file: " + m_workspace->getFileName());
        updateLabels();
    }
}

void CMainWindow::on_actionSaveWorkspace_triggered()
{
    if (saveWorkspace())
        ui->textBrowserOutput->append("Workspace saved successfully to " + m_workspace->getFileName());
    else
        ui->textBrowserOutput->append("Unable to save workspace to " + m_workspace->getFileName());
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
    ui->labelClusterCodeeElementList->setText(fileName);
}

void CMainWindow::on_checkBoxMetricsSelectAll_stateChanged(int state)
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        m_model->item(i)->setCheckState(Qt::CheckState(state));
    }
}

void CMainWindow::on_actionComputeClusters_triggered()
{
    CShowClusters cluster;
    cluster.generateCharts(m_clusterList->getClusters(), ui->clusterWebView);
    ui->textBrowserOutput->append("Clusters done.");
}

void CMainWindow::on_actionCalculateStatistics_triggered()
{
    m_workspace->calcStatistics();
}

void CMainWindow::on_buttonCalculateMetrics_clicked()
{
    StringVector metrics;
    for (int i = 0; i < m_model->rowCount(); ++i) {
        if (m_model->item(i)->checkState() == Qt::Checked)
            metrics.push_back(m_model->item(i)->text().toStdString());
    }

    m_metrics->calculateMetrics(metrics, (IndexType)ui->lineEditRevisionMetrics->text().toLongLong());
    ui->textBrowserOutput->append("metrics done");
}

void CMainWindow::on_actionShowMetrics_triggered()
{
    CShowMetrics metrics(*m_workspace->getResultsByName(METRICS), m_clusterList->getClusters());
    metrics.generateResults(ui->webViewResults);
    metrics.generateCharts(ui->webViewCharts);
}

void CMainWindow::on_buttonCalcCluster_clicked()
{
    m_clusterList->createClusters();
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
    m_workspace->loadTestSuite();
}
