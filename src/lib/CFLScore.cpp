#include "CFLScore.h"
#include "util/CTestSuiteScore.h"
#include "gui/CMainWindow.h"
#include "CWorkspace.h"

#include <QString>

CFLScore::CFLScore(QObject *parent) : QThread(parent)
{
}

void CFLScore::calculateScore(RevNumType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, IntVector failedCodeElements, CMainWindow *mainWindow)
{
    m_kernel = mainWindow->getKernel();
    m_revision = revision;
    m_testSuite = mainWindow->getWorkspace()->getTestSuite();
    m_clusters = &mainWindow->getClusterList()->getClusters();
    m_results = mainWindow->getWorkspace()->getResultsByName(SCORE);
    m_faultLocalizationTechniques = faultLocalizationTechniques;
    m_failedCodeElements = failedCodeElements;
    m_selectedClusters = selectedClusters;

    if (!isRunning())
        start();
}

void CFLScore::run()
{
    for (StringVector::const_iterator it = m_selectedClusters.begin(); it != m_selectedClusters.end(); ++it) {
        rapidjson::Value clusterVal(rapidjson::kObjectType);
        for (IndexType i = 0; i < m_faultLocalizationTechniques.size(); i++) {
            std::string flTechniqueName = m_faultLocalizationTechniques[i];
            IFaultLocalizationTechniquePlugin *technique = m_kernel->getFaultLocalizationTechniquePluginManager().getPlugin(flTechniqueName);

            emit updateStatusLabel(("Calculating " + flTechniqueName + " for " + *it).c_str());

            technique->init(m_testSuite, m_revision);
            // Calculate FL score
            technique->calculate((*m_clusters)[(*it)], "");
            rapidjson::Value key(flTechniqueName.c_str(), m_results->GetAllocator());
            rapidjson::Value values(rapidjson::kObjectType);
            values.CopyFrom(technique->getValues(), m_results->GetAllocator());
            clusterVal.AddMember(key, values, m_results->GetAllocator());

            if (!m_failedCodeElements.size())
                continue;

            rapidjson::Value flValues(rapidjson::kObjectType);
            for (IndexType k = 0; k < m_failedCodeElements.size(); k++) {
                IndexType cid = m_failedCodeElements[k];
                rapidjson::Value::MemberIterator memberIt = clusterVal[flTechniqueName.c_str()].FindMember(static_cast<std::ostringstream*>( &(std::ostringstream() << cid) )->str().c_str());
                if (memberIt == clusterVal[flTechniqueName.c_str()].MemberEnd())
                    continue;
                double value = memberIt->value.GetDouble();
                double score = CTestSuiteScore::flScore((*m_clusters)[(*it)], value, technique->getDistribution());

                rapidjson::Value failedCe(static_cast<std::ostringstream*>( &(std::ostringstream() << cid) )->str().c_str(), m_results->GetAllocator());
                rapidjson::Value flScore(score);
                flValues.AddMember(failedCe, flScore, m_results->GetAllocator());
            }
            rapidjson::Value flKey((flTechniqueName + "-fl").c_str(), m_results->GetAllocator());
            clusterVal.AddMember(flKey, flValues, m_results->GetAllocator());
        }
        rapidjson::Value key((*it).c_str(), m_results->GetAllocator());
        m_results->RemoveMember(key);
        m_results->AddMember(key, clusterVal, m_results->GetAllocator());
    }
    emit processFinished("Calculation of scores has been finished");
}
