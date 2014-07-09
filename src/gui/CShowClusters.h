#ifndef CSHOWCLUSTERS_H
#define CSHOWCLUSTERS_H

#include <QWebView>
#include "data/CClusterDefinition.h"

using namespace soda;

class CShowClusters
{
public:
    CShowClusters();
    ~CShowClusters();
    void generateCharts(std::map<std::string, CClusterDefinition> &clusterList, QWebView *webView);
};

#endif // CSHOWCLUSTERS_H
