#include "gui/CMainWindow.h"
#include "CWorkspace.h"
#include "CTestSuiteMetrics.h"

CTestSuiteMetrics::CTestSuiteMetrics(QObject *parent) : QThread(parent)
{
}

CTestSuiteMetrics::~CTestSuiteMetrics()
{
}

void CTestSuiteMetrics::calculateMetrics(StringVector metrics, StringVector selectedClusters, RevNumType revision, CMainWindow *mainWindow)
{
    m_metricNames = metrics;
    m_revision = revision;
    m_testSuite = mainWindow->getWorkspace()->getTestSuite();
    m_kernel = mainWindow->getKernel();
    m_results = mainWindow->getWorkspace()->getMeasurementResults(METRICS, mainWindow->getMetricMeasurement());

    for (StringVector::const_iterator it = selectedClusters.begin(); it != selectedClusters.end(); ++it) {
        m_clusters[*it] = mainWindow->getClusterList()->getClusters()[*it];
    }

    if (!isRunning())
        start();
}

void CTestSuiteMetrics::run()
{
    for (StringVector::iterator it = m_metricNames.begin(); it != m_metricNames.end(); it++) {
        if (m_metricsCalculated.find(*it) == m_metricsCalculated.end())
            calculateMetric(*it);
    }
    emit processFinished("Calculation of metrics has been finished");
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

    emit updateStatusLabel(("Calculating metric: " + name + "...").c_str());
    metric->init(m_testSuite, &m_clusters, m_revision);

    metric->calculate(*m_results);
    m_metricsCalculated.insert(name);
}

