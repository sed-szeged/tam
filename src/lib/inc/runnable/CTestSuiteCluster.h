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

#ifndef CTESTSUITECLUSTER_H
#define CTESTSUITECLUSTER_H

#include "data/CSelectionData.h"
#include "engine/CKernel.h"
#include <QThread>

using namespace soda;

/**
 * @brief The CTestSuiteCluster class implements cluster computation on a separate thread.
 */
class CTestSuiteCluster : public QThread
{
    Q_OBJECT
public:

    /**
     * @brief Creates a new object.
     * @param parent
     */
    CTestSuiteCluster(QObject *parent = 0);
    ~CTestSuiteCluster() { }

    /**
     * @brief Initialises the thread with the neccessery parameters.
     * @param clusterPlugin Name of the cluster plugin.
     * @param kernel Plugin manager.
     * @param data Current test-suite.
     * @param clusters Container of clusters.
     * @param params Parameters for the selected cluster plugins.
     */
    void createClusters(String clusterPlugin, CKernel *kernel, CSelectionData *data, ClusterMap *clusters, rapidjson::Document &params);

protected:

    /**
     * @brief Starts the computation.
     */
    void run();

signals:

    /**
     * @brief Signal which updates the status label on the main window.
     * @param label Label to be set on the status label.
     */
    void updateStatusLabel(QString label);

    /**
     * @brief Signal that informs the main window that the computation is finished.
     * @param msg Message to the main window.
     */
    void processFinished(QString msg);

private:

    /**
     * @brief Name of the cluster plugin.
     */
    String m_clusterPluginName;

    /**
     * @brief Plugin kernel.
     */
    CKernel *m_kernel;

    /**
     * @brief Loaded test-suite.
     */
    CSelectionData *m_data;

    /**
     * @brief Container of clusters.
     */
    ClusterMap *m_clusters;

    /**
     * @brief Parameters for the cluster plugin.
     */
    rapidjson::Document m_params;
};

#endif // CTESTSUITECLUSTER_H
