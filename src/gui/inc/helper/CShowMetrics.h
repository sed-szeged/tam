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

/**
 * @brief The CShowMetrics class visualises the results of metrics on different charts.
 */
class CShowMetrics
{
public:

    /**
     * @brief Creates a new object.
     * @param metrics Metric results.
     * @param clusters Container of clusters.
     */
    CShowMetrics(rapidjson::Document& metrics, ClusterMap& clusters);
    ~CShowMetrics();

    /**
     * @brief Visualises the results in a table.
     * @param webView Selected QWebView instance.
     */
    void generateResults(QWebView *webView);

    /**
     * @brief Visualises the results on different charts.
     * @param webView Selected QWebView instance.
     */
    void generateCharts(QWebView *webView);

    /**
     * @brief NIY: Not implemented yet.
     * @param webView Selected QWebView instance.
     */
    void generateHeatMap(QWebView *webView);

private:

    /**
     * @brief Metric results.
     */
    rapidjson::Document& m_metrics;

    /**
     * @brief Available clusters.
     */
    ClusterMap& m_clusters;
};

#endif // CSHOWMETRICS_H
