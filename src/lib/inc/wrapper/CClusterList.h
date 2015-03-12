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

#ifndef CCLUSTERLIST_H
#define CCLUSTERLIST_H

#include "data/CSelectionData.h"
#include "data/CClusterDefinition.h"
#include "engine/CKernel.h"

#include <rapidjson/document.h>
#include <QString>

using namespace soda;

/**
 * @brief The CClusterList class wraps the ClusterMap type with a few methods.
 */
class CClusterList
{
public:

    /**
     * @brief Initializes a CClusterList object.
     */
    CClusterList();
    ~CClusterList();

    /**
     * @brief Returns a reference to the current clusters.
     * @return Reference to the current clusters.
     */
    ClusterMap& getClusters() { return *m_clusters; }

    /**
     * @brief Stores the given json object in m_clusters
     * @param doc Given json object which contains clusters.
     */
    void fromJson(rapidjson::Document &doc);

    /**
     * @brief Converts the current clusters to json format.
     * @param doc Given json object where the current clusters will be stored.
     */
    void toJson(rapidjson::Document &doc);

private:

    /**
     * @brief Stores cluster groups.
     */
    ClusterMap *m_clusters;
};

#endif // CCLUSTERLIST_H
