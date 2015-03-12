/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "runnable/CFLScore.h"
#include "util/CTestSuiteScore.h"
#include "ui/CMainWindow.h"
#include "wrapper/CWorkspace.h"

#include <QString>

void CFLScore::calculateScore(RevNumType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, IntVector failedCodeElements, CMainWindow *mainWindow)
{
    m_kernel = mainWindow->getKernel();
    m_revision = revision;
    m_testSuite = mainWindow->getWorkspace()->getTestSuite();
    m_clusters = &mainWindow->getClusterList()->getClusters();
    m_results = mainWindow->getWorkspace()->getMeasurementResults(SCORE, mainWindow->getScoreMeasurement());
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
                std::ostringstream cidAsString;
                cidAsString << cid;
                rapidjson::Value::MemberIterator memberIt = clusterVal[flTechniqueName.c_str()].FindMember(static_cast<std::ostringstream*>( &cidAsString )->str().c_str());
                if (memberIt == clusterVal[flTechniqueName.c_str()].MemberEnd())
                    continue;
                double value = memberIt->value.GetDouble();
                double score = CTestSuiteScore::flScore((*m_clusters)[(*it)], value, technique->getDistribution());

                rapidjson::Value failedCe(static_cast<std::ostringstream*>( &cidAsString )->str().c_str(), m_results->GetAllocator());
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
