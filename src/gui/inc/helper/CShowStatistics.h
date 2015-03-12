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

#ifndef CSHOWSTATISTICS_H
#define CSHOWSTATISTICS_H

#include "rapidjson/document.h"
#include "wrapper/CWorkspace.h"
#include <QWebView>


class CShowStatistics
{
public:
    CShowStatistics(CWorkspace *workspace);
    ~CShowStatistics();

    void generateChartForTab(QWebView *view, int tabindex);
    void fillGeneralTab(QWebView *view);

private:
    void convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str, bool chart = false);

    CWorkspace *m_workspace;
};

#endif // CSHOWSTATISTICS_H
