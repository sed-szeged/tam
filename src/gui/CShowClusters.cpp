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
            "<script type=\"text/javascript\" src=\"qrc:/resources/Chart.js/Chart.min.js\"></script>"
            "<script type=\"text/javascript\">"
                "function init() {"
                    "var data = {"
                        "labels: [%1],"
                        "datasets: ["
                            "{"
                                "label: 'Clusters',"
                                "fillColor: 'rgba(151,187,205,0.2)',"
                                "strokeColor: 'rgba(151,187,205,1)',"
                                "pointColor: 'rgba(151,187,205,1)',"
                                "pointStrokeColor: '#fff',"
                                "pointHighlightFill: '#fff',"
                                "pointHighlightStroke: 'rgba(151,187,205,1)',"
                                "data: [%2]"
                            "}"
                        "]"
                    "};"
                    "var ctx = document.getElementById('myChart').getContext('2d');"
                    "var myLineChart = new Chart(ctx).Line(data, { 'responsive':true });"
                    "var tableData = [%3];"
                    "$('#group_table').DataTable({ data: tableData, paging: false, searching: false });"
                "}"
            "</script></head><body onload=\"init()\">"
            "<table id=\"group_table\" class=\"display\">"
            "<thead><tr><th>Group</th>"
            "<th>Number of test cases</th>"
            "<th>Number of code elements</th></tr></thead>"
            "<tbody></tbody></table>"
            "<h3 style=\"text-align:center;\">Number of test cases in each group</h3>"
            "<canvas id=\"myChart\" width=\"500\"></canvas>"
            "</body></html>";

    QString chartLabels;
    QString chartData;
    QString tableData;
    for (ClusterMap::iterator it = clusters.begin(); it != clusters.end(); ++it) {
        QStringList parts = QString(it->first.c_str()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;
        tableData.append("['" + QString(parts[0]) + "'," + QString::number(it->second.getTestCases().size()) + "," + QString::number(it->second.getCodeElements().size()) + "],");
        chartLabels.append("'" + QString(parts[0]) + "',");
        chartData.append(QString::number(it->second.getTestCases().size()) + ",");
    }
    tableData.chop(1);
    chartLabels.chop(1);
    chartData.chop(1);
    html = html.arg(chartLabels, chartData, tableData);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}
