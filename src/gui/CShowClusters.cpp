#include "CShowClusters.h"

CShowClusters::CShowClusters()
{
}

CShowClusters::~CShowClusters()
{
}

void CShowClusters::generateCharts(ClusterMap &clusters, QWebView *webView)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<link rel=\"stylesheet\" type=\"text/css\" href=\"qrc:/resources/DataTables/css/jquery.dataTables.min.css\">"
            "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.js\"></script>"
            "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.dataTables.min.js\"></script>"
            "<script type=\"text/javascript\" src=\"qrc:/resources/CanvasJS/canvasjs.min.js\"></script>"
            "<script type=\"text/javascript\">"
                "function init() {"
                  "var chart = new CanvasJS.Chart(\"group_chart\", {"
                    "exportEnabled:true,"
                    "title:{"
                      "text: 'Number of test cases in each group',"
                      "fontSize: 16"
                    "},"
                    "axisX: {"
                      "labelAngle: -60"
                    "},"
                    "data: ["
                      "{"
                       "type: \"line\","
                       "dataPoints: [%1]"
                      "}"
                    "]"
                  "});"

                  "chart.render();"
                  "var tableData = [%2];"
                  "$('#group_table').DataTable({ data: tableData, paging: false, searching: false });"
                "}"
            "</script></head><body onload=\"init()\">"
            "<table id=\"group_table\" class=\"display\">"
            "<thead><tr><th>Group</th>"
            "<th>Number of test cases</th>"
            "<th>Number of code elements</th></tr></thead>"
            "<tbody></tbody></table></br>"
            "<div id=\"group_chart\"></div>"
            "</body></html>";

    QString chartData;
    QString tableData;
    for (ClusterMap::iterator it = clusters.begin(); it != clusters.end(); ++it) {
        QStringList parts = QString(it->first.c_str()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;

        tableData.append("['" + QString(parts[0]) + "'," + QString::number(it->second.getTestCases().size()) + "," + QString::number(it->second.getCodeElements().size()) + "],");
        chartData.append("{label:'" + QString(parts[0]) + "',y:" + QString::number(it->second.getTestCases().size()) + "},");
    }
    tableData.chop(1);
    chartData.chop(1);
    html = html.arg(chartData, tableData);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}
