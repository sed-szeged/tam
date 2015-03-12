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

#include "runnable/CTestSuiteLoader.h"
#include "wrapper/CWorkspace.h"

CTestSuiteLoader::CTestSuiteLoader(QObject *parent) : QThread(parent)
{
}

CTestSuiteLoader::~CTestSuiteLoader()
{
}

void CTestSuiteLoader::load(CWorkspace *workspace)
{
    m_covPath = workspace->getCoveragePath().toStdString();
    m_resPath = workspace->getResultsPath().toStdString();
    m_changesPath = workspace->getChangesetPath().toStdString();
    m_testSuite = workspace->getTestSuite();

    if (!isRunning())
        start();
}

void CTestSuiteLoader::run()
{
    emit updateStatusLabel("Loading coverage file...");
    m_testSuite->loadCoverage(m_covPath);

    if (!m_resPath.empty()) {
        emit updateStatusLabel("Loading results file...");
        m_testSuite->loadResults(m_resPath);
    }

    if (!m_changesPath.empty()) {
        emit updateStatusLabel("Loading changeset file...");
        m_testSuite->loadChangeset(m_changesPath);
    }

    emit updateStatusLabel("Globalizing...");
    m_testSuite->globalize();
    emit processFinished("Test-suite loading has been finished");

}
