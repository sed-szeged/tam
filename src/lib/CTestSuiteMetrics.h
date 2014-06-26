#ifndef CTESTSUITEMETRICS_H
#define CTESTSUITEMETRICS_H

#include "engine/plugin/ITestSuiteMetricPlugin.h"
#include "data/CClusterDefinition.h"

using namespace soda;

class CMainWindow;

class CTestSuiteMetrics
{
public:
    CTestSuiteMetrics(CMainWindow *window);
    ~CTestSuiteMetrics();

    void calculateMetrics(StringVector &metrics, IndexType revision);
    void createClusters();

    std::map<std::string, CClusterDefinition>& getClusterList() { return m_clusterList; }

private:

    void calculateMetric(const std::string &name, IndexType revision);

    CMainWindow *m_mainWindow;

    std::map<std::string, CClusterDefinition> m_clusterList;

    std::set<std::string> m_metricsCalculated;
};

#endif // CTESTSUITEMETRICS_H
