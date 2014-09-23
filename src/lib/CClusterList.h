#ifndef CCLUSTERLIST_H
#define CCLUSTERLIST_H

#include "data/CSelectionData.h"
#include "data/CClusterDefinition.h"
#include "engine/CKernel.h"

#include <rapidjson/document.h>
#include <QString>

using namespace soda;

class CClusterList
{
public:
    CClusterList();
    ~CClusterList();

    ClusterMap& getClusters() { return *m_clusters; }
    void fromJson(rapidjson::Document &doc);
    void toJson(rapidjson::Document &doc);

private:
    ClusterMap *m_clusters;
};

#endif // CCLUSTERLIST_H
