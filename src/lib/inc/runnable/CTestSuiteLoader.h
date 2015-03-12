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

class CTestSuiteLoader : public QThread
{
    Q_OBJECT

public:
    CTestSuiteLoader(QObject *parent = 0);
    ~CTestSuiteLoader();

    void load(CWorkspace *workspace);

protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:
    String m_covPath;
    String m_resPath;
    String m_changesPath;
    CSelectionData *m_testSuite;
};

#endif // CTESTSUITELOADER_H
