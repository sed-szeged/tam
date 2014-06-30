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

private:

    void calculateMetric(const std::string &name, IndexType revision);

    CMainWindow *m_mainWindow;

    std::set<std::string> m_metricsCalculated;
};

#endif // CTESTSUITEMETRICS_H
