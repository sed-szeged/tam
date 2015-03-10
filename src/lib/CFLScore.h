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

class CFLScore : public QThread
{
    Q_OBJECT
public:
    CFLScore(QObject *parent = 0);
    void calculateScore(RevNumType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, IntVector failedCodeElements, CMainWindow *mainWindow);
protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:

    RevNumType m_revision;
    IntVector m_failedCodeElements;
    StringVector m_faultLocalizationTechniques;
    StringVector m_selectedClusters;
    CSelectionData *m_testSuite;
    rapidjson::Document *m_results;
    CKernel *m_kernel;
    ClusterMap *m_clusters;
};

#endif // CFLSCORE_H
