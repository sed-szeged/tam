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

class CTestSuiteMetrics : public QThread
{
    Q_OBJECT
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
