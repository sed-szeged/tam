#include "CShowStatistics.h"
#include "CMainWindow.h"
#include "ui_CMainWindow.h"

struct ChartData
{
    QString title;
    QStringList columnNames;
    QString dataSource;
    QString dataColumn;
};

static const ChartData chartData[6] = {
    { "Code element coverage histogram", { "Number of affected test cases by a code element", "Occurences" }, COV_STATS, "code_coverage_histogram" },
    { "Test case coverage histogram", { "Number of covered code elements in a test case", "Occurences" }, COV_STATS, "test_coverage_histogram" },
    { "Results fail histogram", { "Number of failed test cases in a revision", "Occurences" }, RES_STATS, "fail_histogram" },
    { "Revision fail histogram", { "Revision number", "Number of failed test cases" }, RES_STATS, "revision_fail_histogram"},
    { "Executed histogram", { "Number of executed test cases", "Occurences" }, RES_STATS, "executed_histogram" },
    { "Test case info", { "Test case id", "Executed", "Fail" }, RES_STATS, "test_case_info" }
};

CShowStatistics::CShowStatistics(CWorkspace *workspace) :
    m_workspace(workspace)
{
}

CShowStatistics::~CShowStatistics()
{
}

void CShowStatistics::fillGeneralTab(QWebView *view)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<title>General</title>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "</head><body><div>"
            "<h3>Coverage statistics</h3>"
            "Number of code elements: %1</br>"
            "Number of test cases: %2</br>"
            "Density of the coverage matrix: %3</br>"
            "Average test cases per code elements: %4</br>"
            "Average code elements per test cases: %5</br>"
            "</div>"
            "%6"
            "</body></html>";

    QString resultsHtml = "<div><h3>Results statistics</h3>"
            "Number of revisions: %1</br>"
            "Number of test cases: %2</br>"
            "Number of total failed test cases: %3</br>"
            "Average failed test cases per revision: %4</br></div>";

    rapidjson::Document *covData = m_workspace->getData(COV_STATS);
    rapidjson::Document *resData = m_workspace->getData(RES_STATS);

    if (m_workspace->getFileMask() & FILE_RESULTS) {
        resultsHtml = resultsHtml.arg(QString::number((*resData)["number_of_revisions"].GetInt()),
                QString::number((*resData)["number_of_test_cases"].GetInt()),
                QString::number((*resData)["number_of_total_fails"].GetInt()),
                QString::number((*resData)["average_failed_test_cases_per_revision"].GetDouble()));
    } else
        resultsHtml = "";

    html = html.arg(QString::number((*covData)["number_of_code_elements"].GetInt()),
            QString::number((*covData)["number_of_test_cases"].GetInt()),
            QString::number((*covData)["density"].GetDouble()),
            QString::number((*covData)["average_test_cases_per_code_elements"].GetDouble()),
            QString::number((*covData)["average_code_elements_per_test_cases"].GetDouble()),
            resultsHtml);

    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}

void CShowStatistics::generateChartForTab(QWebView *view, int tabindex)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<title>Table</title>"
            "<link rel=\"stylesheet\" type=\"text/css\" href=\"qrc:/resources/DataTables/css/jquery.dataTables.min.css\">"
            "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.js\"></script>"
            "<script type=\"text/javascript\" charset=\"utf8\" src=\"qrc:/resources/DataTables/js/jquery.dataTables.min.js\"></script>"
            "<script type=\"text/javascript\" src=\"qrc:/resources/CanvasJS/canvasjs.min.js\"></script>"
            "<script>"
              "function init(){"
                "var tableData = [%1];"
                "$('#group_table').DataTable({ data: tableData, order: [ 1, 'desc' ] });"
                "var chart = new CanvasJS.Chart(\"chart\",{"
                  "zoomEnabled: true,"
                  "axisX:{"
                    "title:'%2',"
                    "titleFontSize: 16"
                  "},"
                  "axisY:{"
                    "title:'%3',"
                    "titleFontSize: 16"
                  "},"
                  "title:{"
                    "text: '%4',"
                    "fontSize: 16"
                  "},"
                  "data: [{type: \"column\",dataPoints: [%5]}]"
                "});"
                "chart.render();"
                "%6"
              "}"
            "</script></head><body onLoad=\"init()\">"
            "<h3 style=\"text-align:center;\">%7</h3>"
            "<table id=\"group_table\" class=\"display\">"
            "<thead><tr>%8</tr></thead>"
            "<tbody></tbody></table>"
            "<div id=\"chart\" style=\"height:500px;\"></div>"
            "</body></html>";

    rapidjson::Document *data = m_workspace->getData(chartData[tabindex].dataSource.toStdString());
    QString dataStr;
    QString dataChart;
    QString tableHeader;
    QString hideChart;
    if (chartData[tabindex].dataColumn == "test_case_info") {
        const rapidjson::Value& val = (*data)["test_case_info"];
        for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
            dataStr.append("[" + QString(itr->name.GetString()) + "," + QString::number(itr->value["executed"].GetInt()) + "," +  QString::number(itr->value["fail"].GetInt()) + "],");
        }

        for (int i = 0; i < 3; ++i)
            tableHeader.append("<th>" + chartData[tabindex].columnNames[i] + "</th>");

        hideChart.append("$(\"#chart\").hide();");
        dataStr.chop(1);
    } else {
        convertJsonToStringArray(data, chartData[tabindex].dataColumn, dataStr);
        convertJsonToStringArray(data, chartData[tabindex].dataColumn, dataChart, true);

        for (int i = 0; i < 2; ++i)
            tableHeader.append("<th>" + chartData[tabindex].columnNames[i] + "</th>");
    }

    html = html.arg(dataStr, chartData[tabindex].columnNames[0], chartData[tabindex].columnNames[1], chartData[tabindex].title, dataChart, hideChart, chartData[tabindex].title, tableHeader);
    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}

void CShowStatistics::convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str, bool chart)
{
    const rapidjson::Value& val = (*data)[element.toStdString().c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
        if (QString(itr->name.GetString()).compare("0") == 0)
            continue;

        if (chart)
            str.append("{label:'" + QString(itr->name.GetString()) + "',y:" + QString::number(itr->value.GetInt()) + "},");
        else
            str.append("[" + QString(itr->name.GetString()) + "," + QString::number(itr->value.GetInt()) + "],");
    }
    str.chop(1);
}
