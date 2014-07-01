#include <iostream>
#include "CShowMetrics.h"

CShowMetrics::CShowMetrics(rapidjson::Document& metrics, std::map<std::string, CClusterDefinition>& clusterList) :
    m_metrics(metrics), m_clusterList(clusterList)
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
                "<style type='text/css'>"
                    ".left-border { border: 10px solid black; }"
                    ".right-border { border-right: 2px solid black; }"
                "</style>"
                "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
                "<script type=\"text/javascript\">google.load('visualization', '1.1', {packages: ['corechart']});</script>"
                "<script type=\"text/javascript\">google.load('visualization', '1', {packages: ['table']});</script>"
                "<script type=\"text/javascript\">"
                      "google.setOnLoadCallback(drawTable);"
                      "function drawTable() {"
                        "var data = new google.visualization.DataTable();"
                        "data.addColumn('string', '');"
                        "data.addColumn('number', 'tpce');"
                        "data.addColumn('number', 'cov');"
                        "data.addColumn('number', 'effcov');"
                        "data.addColumn('number', 'part');"
                        "data.addColumn('number', 'effpart');"
                        "data.addColumn('number', 'spec');"
                        "data.addColumn('number', 'uniq');"
                        "data.addRows(["
                          "%1"
                        "]);"

                        "var table = new google.visualization.Table(document.getElementById('summary_table'));"
                        "var formatter = new google.visualization.NumberFormat({decimalSymbol: '.',fractionDigits: 2});"
                        "for(i=1; i<8;++i)"
                        "  formatter.format(data, i);"

                        "table.draw(data, {allowHtml: true});"
                      "}"
                "</script></head><body>"
                "<h3 style=\"text-align:center;\">Summary of base and derived metrics</h3>"
                "<div id=\"summary_table\"></div>"
                  "</body></html>";

    QString table;
    for (rapidjson::Value::MemberIterator itr = m_metrics.MemberBegin(); itr != m_metrics.MemberEnd(); ++itr) {
        QStringList parts = QString(itr->name.GetString()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;

        rapidjson::Value& val = itr->value;
        table.append("['" + QString(parts[0]) + "'," + QString::number(val["tpce"].GetDouble()) + "," +
                QString::number(val["fault-detection"].GetDouble()) + "," + QString::number(val["coverage-efficiency"].GetDouble()) + "," +
                QString::number(val["partition-metric"].GetDouble()) + "," + QString::number(val["partition-efficiency"].GetDouble()) + "," +
                QString::number(val["specialization"].GetDouble()) + "," + QString::number(val["uniqueness"].GetDouble()) + "],");
    }

    table.chop(1);
    html = html.arg(table);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}

void CShowMetrics::generateCharts(QWebView *webView)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">google.load('visualization', '1.1', {packages: ['controls']});</script>"
            "<script type=\"text/javascript\">"
                  "google.setOnLoadCallback(drawCovChart);"
                  "google.setOnLoadCallback(drawSpecChart);"
                  "google.setOnLoadCallback(drawUniqChart);"
                  "function drawCovChart() {"
                    "var data = new google.visualization.DataTable();"
                    "data.addColumn('string', 'Group');"
                    "data.addColumn('number', 'cov');"
                    "data.addColumn('number', 'tpce');"
                    "data.addColumn('number', 'effcov');"
                    "data.addColumn('number', 'effpart');"
                    "data.addRows(["
                      "%1"
                    "]);"

                    "var wrapper = new google.visualization.ChartWrapper({"
                      "'chartType': 'ColumnChart',"
                      "'containerId': 'covval_chart',"
                      "'dataTable': data,"
                      "'options': {"
                        "title: 'Coverage values and test efficiency',"
                        "legend: {position: 'bottom'},"
                      "}"
                    "});"
                    // Draw chart
                    "wrapper.draw();"
                  "}"
                  "function drawSpecChart() {"
                      "var data = new google.visualization.DataTable();"
                      "data.addColumn('string', 'Group');"
                      "data.addColumn('number', 'Special tests');"
                      "data.addColumn('number', 'Not special tests');"
                      "data.addRows(["
                        "%2"
                      "]);"

                      "var wrapper = new google.visualization.ChartWrapper({"
                        "'chartType': 'ColumnChart',"
                        "'containerId': 'spec_chart',"
                        "'dataTable': data,"
                        "'options': {"
                          "title: 'Special and not special part of all tests in each group',"
                          "legend: {position: 'bottom'},"
                          "isStacked: true,"
                        "}"
                      "});"
                      // Draw chart
                      "wrapper.draw();"
                  "}"
                  "function drawUniqChart() {"
                    "var data = new google.visualization.DataTable();"
                    "data.addColumn('string', 'Group');"
                    "data.addColumn('number', 'Unique COV');"
                    "data.addColumn('number', 'Common COV');"
                    "data.addRows(["
                      "%3"
                    "]);"

                    "var wrapper = new google.visualization.ChartWrapper({"
                      "'chartType': 'ColumnChart',"
                      "'containerId': 'uniq_chart',"
                      "'dataTable': data,"
                      "'options': {"
                        "title: 'COV metric uniqueness distribution',"
                        "legend: {position: 'bottom'},"
                        "isStacked: true,"
                      "}"
                    "});"
                    // Draw chart
                    "wrapper.draw();"
                  "}"
            "</script></head><body>"
            "<div id=\"covval_chart\" style=\"height:400px;\"></div>"
            "<div id=\"spec_chart\" style=\"height:400px;\"></div>"
            "<div id=\"uniq_chart\" style=\"height:400px;\"></div>"
              "</body></html>";

    QString covVal;
    QString special;
    QString uniq;
    for (rapidjson::Value::MemberIterator itr = m_metrics.MemberBegin(); itr != m_metrics.MemberEnd(); ++itr) {
        QStringList parts = QString(itr->name.GetString()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;

        rapidjson::Value& val = itr->value;
        covVal.append("['" + QString(parts[0]) + "'," + QString::number(val["fault-detection"].GetDouble()) + "," + QString::number(val["tpce"].GetDouble()) + "," + QString::number(val["coverage-efficiency"].GetDouble()) + "," + QString::number(val["partition-efficiency"].GetDouble()) + "],");

        IndexType nrOfTests = m_clusterList.at(itr->name.GetString()).getTestCases().size();
        IndexType specTests = nrOfTests * val["specialization"].GetDouble();
        if (specTests != nrOfTests)
            special.append("['" + QString(parts[0]) + "'," + QString::number(specTests) + "," + QString::number(nrOfTests - specTests) + "],");

        double uniqCov = val["fault-detection"].GetDouble() * val["uniqueness"].GetDouble();
        if (uniqCov != val["fault-detection"].GetDouble())
            uniq.append("['" + QString(parts[0]) + "'," + QString::number(uniqCov) + "," + QString::number((val["fault-detection"].GetDouble() - uniqCov)) + "],");
    }

    covVal.chop(1);
    special.chop(1);
    uniq.chop(1);

    html = html.arg(covVal, special, uniq);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}
