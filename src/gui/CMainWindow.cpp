#include "CMainWindow.h"
#include "CLoadTestSuiteDialog.h"
#include "CShowStatisticsDialog.h"
#include "CShowClustersDialog.h"
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
    m_workspace(NULL), m_kernel(new CKernel()),
    m_metrics(NULL)
{
    ui->setupUi(this);
    setFixedSize(size());

    m_workspace = new CWorkspace(this);
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

void CMainWindow::on_checkBoxMetricsSelectAll_stateChanged(int state)
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        m_model->item(i)->setCheckState(Qt::CheckState(state));
    }
}

void CMainWindow::on_actionComputeClusters_triggered()
{
    m_metrics->createClusters();
    CShowClustersDialog cluster;
    cluster.generateCharts(m_metrics->getClusterList(), ui->clusterWebView);
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
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<!-- One script tag loads all the required libraries! Do not specify any chart types in the autoload statement. -->"
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">google.load('visualization', '1.1', {packages: ['controls']});</script>"
            "<script type=\"text/javascript\">"
                  "google.setOnLoadCallback(drawCovChart);"
                  "google.setOnLoadCallback(drawSpecChart);"
                  "function drawCovChart() {"
                    "var data = new google.visualization.DataTable();"
                    "data.addColumn('string', 'Group');"
                    "data.addColumn('number', 'cov');"
                    "data.addColumn('number', 'tpce');"
                    "data.addColumn('number', 'effcov');"
                    //"data.addColumn('number', 'effpart');"
                    "data.addRows(["
                      "%1"
                    "]);"

                    "var wrapper = new google.visualization.ChartWrapper({"
                      "'chartType': 'ColumnChart',"
                      "'containerId': 'covval_chart',"
                      "'dataTable': data,"
                      "'options': {"
                        "title: 'Coverage values and test efficiency',"
                        "legend: {position: 'bottom'},"
                      "}"
                    "});"
                    // Draw chart
                    "wrapper.draw();"
                  "}"
                  "function drawSpecChart() {"
                      "var data = new google.visualization.DataTable();"
                      "data.addColumn('string', 'Group');"
                      "data.addColumn('number', 'Special tests');"
                      "data.addColumn('number', 'Not special tests');"
                      "data.addRows(["
                        "%2"
                      "]);"

                      "var wrapper = new google.visualization.ChartWrapper({"
                        "'chartType': 'ColumnChart',"
                        "'containerId': 'spec_chart',"
                        "'dataTable': data,"
                        "'options': {"
                          "title: 'Special and not special part of all tests in each group',"
                          "legend: {position: 'bottom'},"
                          "isStacked: true,"
                        "}"
                      "});"
                      // Draw chart
                      "wrapper.draw();"
                  "}"
            "</script></head><body>"
            "<div id=\"covval_chart\" style=\"height:500px;\"></div>"
            "<div id=\"spec_chart\" style=\"height:500px;\"></div>"
              "</body></html>";

    rapidjson::Document& metrics = *m_workspace->getResultsByName(METRICS);

    QString covVal;
    QString special;
    for (rapidjson::Value::MemberIterator itr = metrics.MemberBegin(); itr != metrics.MemberEnd(); ++itr) {
        QStringList parts = QString(itr->name.GetString()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;

        rapidjson::Value& val = itr->value;
        covVal.append("['" + QString(parts[0]) + "'," + QString::number(val["fault-detection"].GetDouble()) + "," + QString::number(val["tpce"].GetDouble()) + "," + QString::number(val["coverage-efficiency"].GetDouble()) + /*"," + QString::number(val["partition-efficiency"].GetDouble()) +*/ "],");

        IndexType nrOfTests = m_metrics->getClusterList().at(itr->name.GetString()).getTestCases().size();
        IndexType specTests = nrOfTests * val["specialization"].GetDouble();
        if (specTests == nrOfTests)
            continue;

        special.append("['" + QString(parts[0]) + "'," + QString::number(specTests) + "," + QString::number(nrOfTests - specTests) + "],");
    }

    covVal.chop(1);
    special.chop(1);

    html = html.arg(covVal, special);

std::cout << html.toStdString() << std::endl;

    ui->webViewResults->settings()->clearMemoryCaches();
    ui->webViewResults->setHtml(html);
}
