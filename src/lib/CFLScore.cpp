#include "CFLScore.h"
#include "util/CTestSuiteScore.h"

#include <QString>

CFLScore::CFLScore()
{
}

void CFLScore::calculateScore()
{

}

void CFLScore::run()
{
    /*for (rapidjson::Value::ConstValueIterator failIt = (*itr)["failed-code-elements"].Begin(); failIt != (*itr)["failed-code-elements"].End(); ++failIt) {
        if (!selectionData.getCoverage()->getCodeElements().containsValue(failIt->GetString())) {
            continue;
        }
        IndexType cid = selectionData.getCoverage()->getCodeElements().getID(failIt->GetString());
        failedCodeElements.push_back(cid);
    }*/

    std::map<std::string, CClusterDefinition>::iterator clusterIt;
    for (clusterIt = m_clusters->begin(); clusterIt != m_clusters->end(); clusterIt++) {
        rapidjson::Value clusterVal;
        clusterVal.SetObject();
        for (IndexType i = 0; i < m_faultLocalizationTechniques.size(); i++) {
            std::string flTechniqueName = m_faultLocalizationTechniques[i];
            IFaultLocalizationTechniquePlugin *technique = m_kernel->getFaultLocalizationTechniquePluginManager().getPlugin(flTechniqueName);

            technique->init(m_selectionData, m_revision);
            // Calculate FL score
            technique->calculate(clusterIt->second, "");

            IFaultLocalizationTechniquePlugin::FLValues values = technique->getValues();

            rapidjson::Value faultLocVal;
            faultLocVal.SetArray();
            IFaultLocalizationTechniquePlugin::FLValues::iterator flValuesIt;
            for (flValuesIt = values.begin(); flValuesIt != values.end(); ++flValuesIt) {
                rapidjson::Value flVal;
                flVal.SetObject();
                flVal.AddMember(QString::number(flValuesIt->first).toStdString().c_str(), flValuesIt->second, m_results->GetAllocator());
                faultLocVal.PushBack(flVal, m_results->GetAllocator());
            }
            clusterVal.AddMember(flTechniqueName.c_str(), faultLocVal, m_results->GetAllocator());

            /*for (IndexType k = 0; k < m_failedCodeElements.size(); k++) {
                IndexType cid = m_failedCodeElements[k];
                double score = CTestSuiteScore::flScore(clusterIt->second, values[cid], technique->getDistribution());
                scoresByCluster[clusterIt->first][cid][flTechniqueName] = score;
            }*/
        }
        m_results->AddMember(clusterIt->first.c_str(), clusterVal, m_results->GetAllocator());
    }
}
