#ifndef CTESTSUITECLUSTER_H
#define CTESTSUITECLUSTER_H

#include "data/CSelectionData.h"
#include "engine/CKernel.h"
#include <QThread>

using namespace soda;

class CTestSuiteCluster : public QThread
{
    Q_OBJECT
public:
    CTestSuiteCluster(QObject *parent = 0);
    ~CTestSuiteCluster();

    void createClusters(String clusterPlugin, CKernel *kernel, CSelectionData *data, ClusterMap *clusters, rapidjson::Document &params);

protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:
    String m_clusterPluginName;
    CKernel *m_kernel;
    CSelectionData *m_data;
    ClusterMap *m_clusters;
    rapidjson::Document m_params;
};

#endif // CTESTSUITECLUSTER_H
