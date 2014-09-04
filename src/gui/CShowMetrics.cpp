#include "CShowMetrics.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

CShowMetrics::CShowMetrics(rapidjson::Document& metrics, ClusterMap& clusters) :
    m_metrics(metrics), m_clusters(clusters)
{
}

CShowMetrics::~CShowMetrics()
{
}

void CShowMetrics::generateResults(QWebView *webView)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
              "<head>"
                "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
                "<link rel=\"stylesheet\" type=\"text/css\" href=\"qrc:/resources/DataTables/css/jquery.dataTables.min.css\">"
                "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.js\"></script>"
                "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.dataTables.min.js\"></script>"
                "<script type=\"text/javascript\">"
                      "function init() {"
                        "var tableData = [%1];"
                        "$('#summary_table').DataTable({ data: tableData, paging: false, searching: false });"
                      "}"
                "</script></head><body onload=\"init()\">"
                "<h3 style=\"text-align:center;\">Summary of base and derived metrics</h3>"
                "<table id=\"summary_table\" class=\"display\">"
                "<thead><tr><th>Group</th>"
                "%2</tr></thead>"
                "<tbody></tbody></table>"
                  "</body></html>";

    QString tableHeader;
    QString tableData;
    bool first = true;
    for (rapidjson::Value::MemberIterator itr = m_metrics.MemberBegin(); itr != m_metrics.MemberEnd(); ++itr) {
        QStringList parts = QString(itr->name.GetString()).split(" - ");
        if ((parts.size() == 2 && parts[0] != parts[1]) || !itr->value.IsObject())
            continue;

        rapidjson::Value& val = itr->value;
        tableData.append("['" + QString(parts[0]) + "',");
        for (rapidjson::Value::MemberIterator metrIt = val.MemberBegin(); metrIt != val.MemberEnd(); ++metrIt) {
            if (first)
                tableHeader.append("<th>" + QString(metrIt->name.GetString()) + "</th>");

            tableData.append(QString::number(metrIt->value.GetDouble(), 'g', 2) + ",");
        }
        tableData.chop(1);
        tableData.append("],");
        first = false;
    }

    tableData.chop(1);
    html = html.arg(tableData, tableHeader);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}

void CShowMetrics::generateCharts(QWebView *webView)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<script type=\"text/javascript\" src=\"qrc:/resources/CanvasJS/canvasjs.min.js\"></script>"
            "<script type=\"text/javascript\">"
                  "function init() {"
                    "drawCovChart();"
                    "drawSpecChart();"
                    "drawUniqChart();"
                  "}"
                  "function drawCovChart() {"
                    "var chart = new CanvasJS.Chart(\"covval_chart\",{"
                      "title:{"
                        "text: 'Coverage values and test efficiency',"
                        "fontSize: 16"
                      "},"
                      "legend: {"
                        "cursor: \"pointer\","
                        "itemclick: function (e) {"
                          "if (typeof (e.dataSeries.visible) === \"undefined\" || e.dataSeries.visible) {"
                            "e.dataSeries.visible = false;"
                          "} else {"
                            "e.dataSeries.visible = true;"
                          "}"
                          "chart.render();"
                        "}"
                      "},"
                      "axisX: {"
                        "labelAngle: -60"
                      "},"
                      "data: [%1]"
                    "});"
                    "chart.render();"
                  "}"
                  "function drawSpecChart() {"
                    "var chart = new CanvasJS.Chart(\"spec_chart\",{"
                      "title:{"
                        "text: 'Special and not special part of all tests in each group',"
                        "fontSize: 16"
                      "},"
                      "axisX: {"
                        "labelAngle: -60"
                      "},"
                      "data: [%2]"
                    "});"
                    "chart.render();"
                  "}"
                  "function drawUniqChart() {"
                    "var chart = new CanvasJS.Chart(\"uniq_chart\",{"
                      "title:{"
                        "text: 'COV metric uniqueness distribution',"
                        "fontSize: 16"
                      "},"
                      "axisX: {"
                        "labelAngle: -60"
                      "},"
                      "data: [%3]"
                    "});"
                    "chart.render();"
                  "}"
            "</script></head><body onload=\"init()\">"
            "<div id=\"covval_chart\" style=\"height:400px\"></div>"
            "<div id=\"spec_chart\" style=\"height:400px\"></div>"
            "<div id=\"uniq_chart\" style=\"height:400px;\"></div>"
              "</body></html>";

    QString dataSet = "{"
            "type: \"%1\","
            "showInLegend: true,"
            "legendText: '%2',"
            "dataPoints: [%3]"
          "},";

    QMap<QString, QString> chartData;
    QMap<QString, QString> specialData;
    QMap<QString, QString> uniqData;
    for (rapidjson::Value::MemberIterator groupIt = m_metrics.MemberBegin(); groupIt != m_metrics.MemberEnd(); ++groupIt) {
        QStringList parts = QString(groupIt->name.GetString()).split(" - ");
        if ((parts.size() == 2 && parts[0] != parts[1]) || !groupIt->value.IsObject())
            continue;

        rapidjson::Value& val = groupIt->value;
        if (val.HasMember("fault-detection"))
            chartData["fault-detection"].append("{ label: '" + parts[0] + "',y:" + QString::number(val["fault-detection"].GetDouble()) + "},");
        if (val.HasMember("tpce"))
            chartData["tpce"].append("{ label: '" + parts[0] + "',y:" + QString::number(val["tpce"].GetDouble()) + "},");
        if (val.HasMember("coverage-efficiency"))
            chartData["coverage-efficiency"].append("{ label: '" + parts[0] + "',y:" + QString::number(val["coverage-efficiency"].GetDouble()) + "},");
        if (val.HasMember("partition-efficiency"))
            chartData["partition-efficiency"].append("{ label: '" + parts[0] + "',y:" + QString::number(val["partition-efficiency"].GetDouble()) + "},");

        if (val.HasMember("specialization")) {
            IndexType nrOfTests = m_clusters.at(groupIt->name.GetString()).getTestCases().size();
            IndexType specTests = nrOfTests * val["specialization"].GetDouble();
            specialData["Special tests"].append("{label: '" + QString(parts[0]) + "',y:" + QString::number(specTests) + "},");
            specialData["Not special tests"].append("{label: '" + QString(parts[0]) + "',y:" + QString::number(nrOfTests - specTests) + "},");
        }

        if (val.HasMember("fault-detection") && val.HasMember("uniqueness")) {
            double uniqCov = val["fault-detection"].GetDouble() * val["uniqueness"].GetDouble();
            uniqData["Unique COV"].append("{label:'" + QString(parts[0]) + "',y:" + QString::number(uniqCov) + "},");
            uniqData["Common COV"].append("{label:'" + QString(parts[0]) + "',y:" + QString::number((val["fault-detection"].GetDouble() - uniqCov)) + "},");
        }
    }

    QString covVal;
    for (QMap<QString, QString>::iterator it = chartData.begin(); it != chartData.end(); ++it) {
        it.value().chop(1);
        covVal.append(dataSet.arg("column", it.key(), it.value()));
    }
    QString special;
    for (QMap<QString, QString>::iterator it = specialData.begin(); it != specialData.end(); ++it) {
        it.value().chop(1);
        special.append(dataSet.arg("stackedColumn", it.key(), it.value()));
    }
    QString uniq;
    for (QMap<QString, QString>::iterator it = uniqData.begin(); it != uniqData.end(); ++it) {
        it.value().chop(1);
        uniq.append(dataSet.arg("stackedColumn", it.key(), it.value()));
    }

    special.chop(1);
    uniq.chop(1);
    covVal.chop(1);
    html = html.arg(covVal, special, uniq);
    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}

void CShowMetrics::generateHeatMap(QWebView *webView)
{

}
