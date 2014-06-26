#ifndef CSHOWCLUSTERSDIALOG_H
#define CSHOWCLUSTERSDIALOG_H

#include <QWebView>
#include "data/CClusterDefinition.h"

using namespace soda;

class CShowClustersDialog
{
public:
    CShowClustersDialog();
    ~CShowClustersDialog();
    void generateCharts(std::map<std::string, CClusterDefinition> &clusterList, QWebView *webView);
};

#endif // CSHOWCLUSTERSDIALOG_H
