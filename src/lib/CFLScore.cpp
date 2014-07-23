#include "CFLScore.h"
#include "util/CTestSuiteScore.h"
#include "gui/CMainWindow.h"
#include "CWorkspace.h"

#include <QString>

CFLScore::CFLScore(QObject *parent) : QThread(parent)
{
}

void CFLScore::calculateScore(IndexType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, CMainWindow *mainWindow)
{
    m_kernel = mainWindow->getKernel();
    m_revision = revision;
    m_testSuite = mainWindow->getWorkspace()->getTestSuite();
    m_clusters = &mainWindow->getClusterList()->getClusters();
    m_results = mainWindow->getWorkspace()->getResultsByName(SCORE);
    m_faultLocalizationTechniques = faultLocalizationTechniques;
    m_selectedClusters = selectedClusters;

    if (!isRunning())
        start();
}

void CFLScore::run()
{
    for (StringVector::const_iterator it = m_selectedClusters.begin(); it != m_selectedClusters.end(); ++it) {
        rapidjson::Value clusterVal;
        clusterVal.SetObject();
        for (IndexType i = 0; i < m_faultLocalizationTechniques.size(); i++) {
            std::string flTechniqueName = m_faultLocalizationTechniques[i];
            IFaultLocalizationTechniquePlugin *technique = m_kernel->getFaultLocalizationTechniquePluginManager().getPlugin(flTechniqueName);

            emit updateStatusLabel(("Calculating " + flTechniqueName + " for " + *it).c_str());

            technique->init(m_testSuite, m_revision);
            // Calculate FL score
            technique->calculate((*m_clusters)[(*it)], "");

            rapidjson::Value key;
            key.SetString(flTechniqueName.c_str(), m_results->GetAllocator());
            rapidjson::Value values(rapidjson::kObjectType);
            values.CopyFrom(technique->getValues(), m_results->GetAllocator());
            clusterVal.AddMember(key, values, m_results->GetAllocator());

            /*for (IndexType k = 0; k < m_failedCodeElements.size(); k++) {
                IndexType cid = m_failedCodeElements[k];
                double score = CTestSuiteScore::flScore(clusterIt->second, value, technique->getDistribution());
            }*/
        }
        rapidjson::Value key;
        key.SetString((*it).c_str(), m_results->GetAllocator());
        m_results->RemoveMember(key);
        m_results->AddMember(key, clusterVal, m_results->GetAllocator());
    }
    emit processFinished("Calculation of scores has been finished");
}
