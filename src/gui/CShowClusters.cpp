#include "CShowClusters.h"

CShowClusters::CShowClusters()
{
}

CShowClusters::~CShowClusters()
{
}

void CShowClusters::generateCharts(std::map<std::string, CClusterDefinition> &clusterList, QWebView *webView)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<!-- One script tag loads all the required libraries! Do not specify any chart types in the autoload statement. -->"
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">google.load('visualization', '1.1', {packages: ['controls']});</script>"
            "<script type=\"text/javascript\">google.load('visualization', '1', {packages:['table']});</script>"
            "<script type=\"text/javascript\">"
                  "google.setOnLoadCallback(drawTable);"
                  "google.setOnLoadCallback(drawChart);"
                  "function drawTable() {"
                    "var data = new google.visualization.DataTable();"
                    "data.addColumn('string', 'Group');"
                    "data.addColumn('number', 'Number of Tests');"
                    "data.addColumn('number', 'Number of Code Elements');"
                    "data.addRows(["
                      "%1"
                    "]);"

                    "var table = new google.visualization.Table(document.getElementById('table_div'));"
                    "table.draw(data);"
                  "}"
                  "function drawChart() {"
                    "var data = new google.visualization.DataTable();"
                    "data.addColumn('string', 'Group');"
                    "data.addColumn('number', 'Number of Tests');"
                    "data.addRows(["
                      "%2"
                    "]);"

                    "var wrapper = new google.visualization.ChartWrapper({"
                      "'chartType': 'LineChart',"
                      "'containerId': 'chart',"
                      "'dataTable': data,"
                      "'options': {"
                        "title: 'Number of Tests in each group',"
                        "pointSize: 2,"
                        "legend: { position: 'none' }"
                      "}"
                    "});"
                    // Draw chart
                    "wrapper.draw();"
                  "}"
            "</script></head><body>"
            "<div id=\"table_div\" style=\"width:530px;\"></div>"
            "<div id=\"chart\" style=\"width:530px;\"></div>"
              "</body></html>";

    QString clusterTable;
    QString clusterChart;
    for (std::map<std::string, CClusterDefinition>::iterator it = clusterList.begin(); it != clusterList.end(); ++it) {
        QStringList parts = QString(it->first.c_str()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;
        clusterTable.append("['" + QString(parts[0]) + "'," + QString::number(it->second.getTestCases().size()) + "," + QString::number(it->second.getCodeElements().size()) + "],");
        clusterChart.append("['" + QString(parts[0]) + "'," + QString::number(it->second.getTestCases().size()) + "],");
    }
    clusterTable.chop(1);
    clusterChart.chop(1);
    html = html.arg(clusterTable, clusterChart);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}
