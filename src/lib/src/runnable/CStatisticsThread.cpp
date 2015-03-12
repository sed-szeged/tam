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

#include "runnable/CStatisticsThread.h"
#include "wrapper/CWorkspace.h"
#include "util/CSelectionStatistics.h"

void CStatisticsThread::calcStatistics(CWorkspace *workspace)
{
    m_testSuite = workspace->getTestSuite();
    m_covStats = workspace->getData(COV_STATS);
    m_resStats = workspace->getData(RES_STATS);

    if (!isRunning())
        start();
}

void CStatisticsThread::run()
{
    CSelectionStatistics stat(m_testSuite);

    emit updateStatusLabel("Calculating coverage statistics...");
    stat.calcCoverageRelatedStatistics(*m_covStats);

    emit updateStatusLabel("Calculating results statistics...");
    stat.calcCovResultsSummary(*m_resStats);

    emit updateStatusLabel("Calculating fail statistics...");
    stat.calcFailStatistics(*m_resStats);

    emit processFinished("Calculation of statistics has been finished.");
}
