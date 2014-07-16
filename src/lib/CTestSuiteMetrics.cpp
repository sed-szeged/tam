#include "gui/CMainWindow.h"
#include "CWorkspace.h"
#include "CTestSuiteMetrics.h"

CTestSuiteMetrics::CTestSuiteMetrics(QObject *parent) : QThread(parent)
{
}

CTestSuiteMetrics::~CTestSuiteMetrics()
{
}

void CTestSuiteMetrics::calculateMetrics(StringVector metrics, IndexType revision, CMainWindow *mainWindow)
{
    m_metricNames = metrics;
    m_revision = revision;
    m_testSuite = mainWindow->getWorkspace()->getTestSuite();
    m_clusters = &mainWindow->getClusterList()->getClusters();
    m_kernel = mainWindow->getKernel();
    m_results = mainWindow->getWorkspace()->getResultsByName(METRICS);

    if (!isRunning())
        start();
}

void CTestSuiteMetrics::run()
{
    for (StringVector::iterator it = m_metricNames.begin(); it != m_metricNames.end(); it++) {
        emit updateStatusLabel(("Calculating " + *it + "...").c_str());
        calculateMetric(*it);
    }
    emit processFinished("Calculation of metrics finished");
}

void CTestSuiteMetrics::calculateMetric(const std::string &name)
{
    ITestSuiteMetricPlugin *metric = m_kernel->getTestSuiteMetricPluginManager().getPlugin(name);

    StringVector dependencies = metric->getDependency();
    for (StringVector::iterator it = dependencies.begin(); it != dependencies.end(); it++) {
        if (m_metricsCalculated.find(*it) == m_metricsCalculated.end()) {
            calculateMetric(*it);
        }
    }

    metric->init(m_testSuite, m_clusters, m_revision);

    metric->calculate(*m_results);
    m_metricsCalculated.insert(name);
}

