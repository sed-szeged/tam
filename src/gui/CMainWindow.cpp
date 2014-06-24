#include "CMainWindow.h"
#include "CLoadTestSuiteDialog.h"
#include "CShowStatisticsDialog.h"
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
    m_workspace(NULL), m_kernel(new CKernel())
{
    ui->setupUi(this);
    setFixedSize(size());
    m_workspace = new CWorkspace(this);

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
    };

    ui->listViewMetrics->setModel(m_model);
}

CMainWindow::~CMainWindow()
{
    delete ui;
    delete m_workspace;
    delete m_kernel;
    delete m_model;
}

void CMainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void CMainWindow::on_actionLoadTestSuite_triggered()
{
    CLoadTestSuiteDialog dia(this);
    dia.exec();
}

void CMainWindow::on_actionDumpCoverage_triggered()
{
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        s.Clear();
        m_workspace->getResultsByName(collections[i])->Accept(writer);
        ui->textBrowserOutput->append(s.GetString());
    }
}

void CMainWindow::on_actionShowStatistics_triggered()
{
    CShowStatisticsDialog dia(this);
    dia.exec();
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
    }
}

void CMainWindow::on_actionSaveWorkspace_triggered()
{
    if (saveWorkspace())
        ui->textBrowserOutput->append("Workspace saved successfully to " + m_workspace->getFileName());
    else
        ui->textBrowserOutput->append("Unable to save workspace to " + m_workspace->getFileName());
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
}

void CMainWindow::on_checkBoxSelectAll_stateChanged(int state)
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        m_model->item(i)->setCheckState(Qt::CheckState(state));
    }
}
