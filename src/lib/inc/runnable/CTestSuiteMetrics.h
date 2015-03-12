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

#ifndef CTESTSUITEMETRICS_H
#define CTESTSUITEMETRICS_H

#include <QThread>
#include "engine/CKernel.h"
#include "data/CClusterDefinition.h"
#include "rapidjson/document.h"

using namespace soda;

class CMainWindow;

/**
 * @brief The CTestSuiteMetrics class implements the computation of test-suite metrics on a separate thread.
 */
class CTestSuiteMetrics : public QThread
{
    Q_OBJECT
public:

    /**
     * @brief Creates a new object.
     * @param parent
     */
    CTestSuiteMetrics(QObject *parent = 0) : QThread(parent) { }
    ~CTestSuiteMetrics() { }

    /**
     * @brief Passes the required initial data to the current object.
     * @param metrics Names of the selected metrics which will be computed.
     * @param selectedClusters Names of the selected clusters.
     * @param revision Selected revision number.
     * @param mainWindow Main object which provides some methods for the neccessery objects.
     */
    void calculateMetrics(StringVector metrics, StringVector selectedClusters, RevNumType revision, CMainWindow *mainWindow);

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
     * @brief Calculates the given metric.
     * @param name Name of the metric.
     */
    void calculateMetric(const std::string &name);

    /**
     * @brief Stores the results of the computed metrics.
     */
    rapidjson::Document *m_results;

    /**
     * @brief Plugin manager.
     */
    CKernel *m_kernel;

    /**
     * @brief Selected clusters.
     */
    ClusterMap m_clusters;

    /**
     * @brief List of metric names.
     */
    StringVector m_metricNames;

    /**
     * @brief Current test-suite.
     */
    CSelectionData *m_testSuite;

    /**
     * @brief Revision number.
     */
    RevNumType m_revision;

    /**
     * @brief Stores which metrics has been computed.
     */
    std::set<std::string> m_metricsCalculated;
};

#endif // CTESTSUITEMETRICS_H
