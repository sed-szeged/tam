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
    CShowMetrics(rapidjson::Document& metrics, std::map<std::string, CClusterDefinition>& clusterList);
    ~CShowMetrics();

    void generateResults(QWebView *webView);
    void generateCharts(QWebView *webView);
    void generateHeatMap(QWebView *webView);

private:

    rapidjson::Document& m_metrics;
    std::map<std::string, CClusterDefinition>& m_clusterList;
};

#endif // CSHOWMETRICS_H
