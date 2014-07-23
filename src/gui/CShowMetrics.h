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
