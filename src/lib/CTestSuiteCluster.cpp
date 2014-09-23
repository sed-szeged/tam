#include "CTestSuiteCluster.h"

CTestSuiteCluster::CTestSuiteCluster(QObject *parent) : QThread(parent)
{
    m_params.SetObject();
}

CTestSuiteCluster::~CTestSuiteCluster()
{
}

void CTestSuiteCluster::createClusters(String clusterPlugin, CKernel *kernel, CSelectionData *data, ClusterMap *clusters, rapidjson::Document &params)
{
    m_clusterPluginName = clusterPlugin;
    m_kernel = kernel;
    m_data = data;
    m_clusters = clusters;
    m_params.CopyFrom(params, m_params.GetAllocator());

    if (!isRunning())
        start();
}

void CTestSuiteCluster::run()
{
    emit updateStatusLabel("Creating groups...");
    ITestSuiteClusterPlugin *clusterAlgorithm = m_kernel->getTestSuiteClusterPluginManager().getPlugin(m_clusterPluginName);
    clusterAlgorithm->init(m_params);
    clusterAlgorithm->execute(*m_data, *m_clusters);
    emit processFinished("Creation of groups has been finished.");
}
