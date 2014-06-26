#include "gui/CMainWindow.h"
#include "CWorkspace.h"
#include "ui_CMainWindow.h"
#include "CTestSuiteMetrics.h"

CTestSuiteMetrics::CTestSuiteMetrics(CMainWindow *window) :
    m_mainWindow(window)
{
}

CTestSuiteMetrics::~CTestSuiteMetrics()
{

}

void CTestSuiteMetrics::createClusters()
{
    ITestSuiteClusterPlugin *clusterAlgorithm = m_mainWindow->getKernel()->getTestSuiteClusterPluginManager().getPlugin(m_mainWindow->getUi()->comboBoxClusterPlugins->currentText().toStdString());
    clusterAlgorithm->init(*m_mainWindow->getWorkspace()->getResultsByName(WS));
    clusterAlgorithm->execute(*m_mainWindow->getWorkspace()->getTestSuite(), m_clusterList);
}

void CTestSuiteMetrics::calculateMetrics(StringVector &metrics, IndexType revision)
{
    for (StringVector::iterator it = metrics.begin(); it != metrics.end(); it++) {
        calculateMetric(*it, revision);
    }
}

void CTestSuiteMetrics::calculateMetric(const std::string &name, IndexType revision)
{
    ITestSuiteMetricPlugin *metric = m_mainWindow->getKernel()->getTestSuiteMetricPluginManager().getPlugin(name);

    StringVector dependencies = metric->getDependency();
    for (StringVector::iterator it = dependencies.begin(); it != dependencies.end(); it++) {
        if (m_metricsCalculated.find(*it) == m_metricsCalculated.end()) {
            calculateMetric(*it, revision);
        }
    }

    (std::cerr << "[INFO] Calculating metrics: " << metric->getName() << " ...").flush();
    metric->init(m_mainWindow->getWorkspace()->getTestSuite(), &m_clusterList, revision);

    metric->calculate(*m_mainWindow->getWorkspace()->getResultsByName(METRICS));
    m_metricsCalculated.insert(name);
    (std::cerr << " done." << std::endl).flush();

}

