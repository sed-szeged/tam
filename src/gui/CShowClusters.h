#ifndef CSHOWCLUSTERSDIALOG_H
#define CSHOWCLUSTERSDIALOG_H

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

#endif // CSHOWCLUSTERSDIALOG_H
