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
