#ifndef CTESTSUITEMETRICS_H
#define CTESTSUITEMETRICS_H

#include <QThread>
#include "engine/CKernel.h"
#include "data/CClusterDefinition.h"
#include "rapidjson/document.h"

using namespace soda;

class CMainWindow;

class CTestSuiteMetrics : public QThread
{
    Q_OBJECT;
public:
    CTestSuiteMetrics(QObject *parent = 0);
    ~CTestSuiteMetrics();

    void calculateMetrics(StringVector metrics, StringVector selectedClusters, RevNumType revision, CMainWindow *mainWindow);

protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:

    void calculateMetric(const std::string &name);

    rapidjson::Document *m_results;
    CKernel *m_kernel;
    ClusterMap m_clusters;
    StringVector m_metricNames;
    CSelectionData *m_testSuite;
    RevNumType m_revision;
    std::set<std::string> m_metricsCalculated;
};

#endif // CTESTSUITEMETRICS_H
