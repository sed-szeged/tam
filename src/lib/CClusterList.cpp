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

#include "CClusterList.h"

CClusterList::CClusterList() :
    m_clusters(new ClusterMap())
{
}

CClusterList::~CClusterList()
{
    delete m_clusters;
}

void CClusterList::toJson(rapidjson::Document &doc)
{
    for (ClusterMap::iterator it = m_clusters->begin(); it != m_clusters->end(); ++it) {
        rapidjson::Value cluster;
        cluster.SetObject();

        rapidjson::Value tcids;
        tcids.SetArray();
        for (IntVector::const_iterator itr = it->second.getTestCases().begin(); itr != it->second.getTestCases().end(); ++itr) {
            tcids.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("tests", tcids, doc.GetAllocator());

        rapidjson::Value ceids;
        ceids.SetArray();
        for (IntVector::const_iterator itr = it->second.getCodeElements().begin(); itr != it->second.getCodeElements().end(); ++itr) {
            ceids.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("code-elements", ceids, doc.GetAllocator());

        rapidjson::Value key;
        key.SetString(it->first.c_str(), doc.GetAllocator());
        doc.AddMember(key, cluster, doc.GetAllocator());
    }
}

void CClusterList::fromJson(rapidjson::Document &doc)
{
    for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
        CClusterDefinition def;
        if (!itr->value.IsObject())
            continue;

        for (rapidjson::Value::ConstValueIterator it = itr->value["tests"].Begin(); it != itr->value["tests"].End(); ++it)
            def.addTestCase(it->GetUint64());
        for (rapidjson::Value::ConstValueIterator it = itr->value["code-elements"].Begin(); it != itr->value["code-elements"].End(); ++it)
            def.addCodeElement(it->GetUint64());

        (*m_clusters)[String(itr->name.GetString())] = def;
    }
}
