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

#ifndef CTESTSUITELOADER_H
#define CTESTSUITELOADER_H

#include <QThread>
#include "data/CSelectionData.h"

using namespace soda;

class CWorkspace;

/**
 * @brief The CTestSuiteLoader class implements the coverage, results, changeset file loading on a separate thread.
 */
class CTestSuiteLoader : public QThread
{
    Q_OBJECT

public:

    /**
     * @brief CTestSuiteLoader
     * @param parent
     */
    CTestSuiteLoader(QObject *parent = 0) : QThread(parent) { }
    ~CTestSuiteLoader() { }

    /**
     * @brief Loads the specified files from the given workspace.
     * @param workspace Current workspace.
     */
    void load(CWorkspace *workspace);

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
     * @brief Path of the coverage file.
     */
    String m_covPath;

    /**
     * @brief Path of the results file.
     */
    String m_resPath;

    /**
     * @brief Path of the changeset file.
     */
    String m_changesPath;

    /**
     * @brief Test-suite where the files will be loaded.
     */
    CSelectionData *m_testSuite;
};

#endif // CTESTSUITELOADER_H
