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

#ifndef CSHOWCLUSTERS_H
#define CSHOWCLUSTERS_H

#include <QWebView>
#include "data/CClusterDefinition.h"

using namespace soda;

/**
 * @brief The CShowClusters class visualises the generated clusters on charts and a table.
 */
class CShowClusters
{
public:

    /**
     * @brief Creates a new object.
     */
    CShowClusters();
    ~CShowClusters();

    /**
     * @brief Visualises the given clusters.
     * @param clusters Conainer of clusters.
     * @param webView Selected QWebView instance.
     */
    void generateCharts(ClusterMap &clusters, QWebView *webView);
};

#endif // CSHOWCLUSTERS_H
