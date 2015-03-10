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

#ifndef CSHOWMETRICS_H
#define CSHOWMETRICS_H

#include <QWebView>
#include <QString>
#include <rapidjson/document.h>
#include <data/CClusterDefinition.h>

using namespace soda;

class CShowMetrics
{
public:
    CShowMetrics(rapidjson::Document& metrics, ClusterMap& clusters);
    ~CShowMetrics();

    void generateResults(QWebView *webView);
    void generateCharts(QWebView *webView);
    void generateHeatMap(QWebView *webView);

private:

    rapidjson::Document& m_metrics;
    ClusterMap& m_clusters;
};

#endif // CSHOWMETRICS_H
