/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

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
                  "var chart = new CanvasJS.Chart(\"test_chart\", {"
                    "title:{"
                      "text: 'Number of test cases in each group',"
                      "fontSize: 16"
                    "},"
                    "axisX: {"
                      "labelAngle: -60"
                    "},"
                    "data: ["
                      "{"
                       "type: \"column\","
                       "dataPoints: [%1]"
                      "}"
                    "]"
                  "});"
                  "chart.render();"

                  "chart = new CanvasJS.Chart(\"ce_chart\", {"
                    "title:{"
                      "text: 'Number of code elements in each group',"
                      "fontSize: 16"
                    "},"
                    "axisX: {"
                      "labelAngle: -60"
                    "},"
                    "data: ["
                      "{"
                       "type: \"column\","
                       "dataPoints: [%2]"
                      "}"
                    "]"
                  "});"
                  "chart.render();"

                  "var tableData = [%3];"
                  "$('#group_table').DataTable({ data: tableData, paging: false, searching: false });"
                "}"
            "</script></head><body onload=\"init()\">"
            "<table id=\"group_table\" class=\"display\">"
            "<thead><tr><th>Group</th>"
            "<th>Number of test cases</th>"
            "<th>Number of code elements</th></tr></thead>"
            "<tbody></tbody></table></br>"
            "<div id=\"test_chart\" style=\"height:500px;\"></div>"
            "<div id=\"ce_chart\" style=\"height:500px;\"></div>"
            "</body></html>";

    QString testData;
    QString ceData;
    QString tableData;
    for (ClusterMap::iterator it = clusters.begin(); it != clusters.end(); ++it) {
        QStringList parts = QString(it->first.c_str()).split(" - ");
        if (parts.size() == 2 && parts[0] != parts[1])
            continue;

        tableData.append("['" + QString(parts[0]) + "'," + QString::number(it->second.getTestCases().size()) + "," + QString::number(it->second.getCodeElements().size()) + "],");
        testData.append("{label:'" + QString(parts[0]) + "',y:" + QString::number(it->second.getTestCases().size()) + "},");
        ceData.append("{label:'" + QString(parts[0]) + "',y:" + QString::number(it->second.getCodeElements().size()) + "},");
    }
    tableData.chop(1);
    testData.chop(1);
    ceData.chop(1);
    html = html.arg(testData, ceData, tableData);

    webView->settings()->clearMemoryCaches();
    webView->setHtml(html);
}
