#ifndef CCLUSTERLIST_H
#define CCLUSTERLIST_H

#include "data/CClusterDefinition.h"

using namespace soda;

class CMainWindow;

class CClusterList
{
public:
    CClusterList(CMainWindow *mainWindow);
    ~CClusterList();

    void createClusters();
    std::map<std::string, CClusterDefinition>& getClusters() { return *m_clusters; }
    void fromJson(rapidjson::Document &doc);
    void toJson(rapidjson::Document &doc);

private:

    CMainWindow *m_mainWindow;
    std::map<std::string, CClusterDefinition> *m_clusters;
};

#endif // CCLUSTERLIST_H
