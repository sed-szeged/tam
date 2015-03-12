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

#ifndef CFLSCORE_H
#define CFLSCORE_H

#include <QThread>
#include "data/CSelectionData.h"
#include "rapidjson/document.h"
#include "engine/CKernel.h"

using namespace soda;

class CMainWindow;

/**
 * @brief The CFLScore class implements the computation of faulty code elements on separate thread.
 */
class CFLScore : public QThread
{
    Q_OBJECT
public:

    /**
     * @brief Creates a new object.
     * @param parent
     */
    CFLScore(QObject *parent = 0) : QThread(parent) { }
    ~CFLScore() { }

    /**
     * @brief Initialises the thread with the neccessery parameters.
     * @param revision Revision number.
     * @param faultLocalizationTechniques Selected fault localization techniques.
     * @param selectedClusters Selected clusters.
     * @param failedCodeElements Selected failed code element ids.
     * @param mainWindow Main window.
     */
    void calculateScore(RevNumType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, IntVector failedCodeElements, CMainWindow *mainWindow);

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
     * @brief Selected revision number.
     */
    RevNumType m_revision;

    /**
     * @brief Selected failed code elements.
     */
    IntVector m_failedCodeElements;

    /**
     * @brief Selected fault localization techniques.
     */
    StringVector m_faultLocalizationTechniques;

    /**
     * @brief List of selected clusters.
     */
    StringVector m_selectedClusters;

    /**
     * @brief Current test-suite.
     */
    CSelectionData *m_testSuite;

    /**
     * @brief Container of the results.
     */
    rapidjson::Document *m_results;

    /**
     * @brief Plugin manager.
     */
    CKernel *m_kernel;

    /**
     * @brief Container of available clusters.
     */
    ClusterMap *m_clusters;
};

#endif // CFLSCORE_H
