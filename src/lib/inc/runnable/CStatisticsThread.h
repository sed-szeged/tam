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

#ifndef CSTATISTICSTHREAD_H
#define CSTATISTICSTHREAD_H

#include <QThread>
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

class CWorkspace;

/**
 * @brief The CStatisticsThread class implements statistics computation on a separate thread.
 */
class CStatisticsThread : public QThread
{
    Q_OBJECT
public:

    /**
     * @brief Creates a new object.
     * @param parent
     */
    CStatisticsThread(QObject *parent = 0) : QThread(parent) { }
    ~CStatisticsThread() { wait(); }

    /**
     * @brief Initialises the thread with the available files in the workspace.
     * @param workspace Current workspace.
     */
    void calcStatistics(CWorkspace *workspace);

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
     * @brief Current test-suite.
     */
    CSelectionData *m_testSuite;

    /**
     * @brief Container of the computed coverage statistics.
     */
    rapidjson::Document *m_covStats;

    /**
     * @brief Container of the computed results statistics.
     */
    rapidjson::Document *m_resStats;
};

#endif // CSTATISTICSTHREAD_H
