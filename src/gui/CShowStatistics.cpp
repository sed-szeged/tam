#include "CShowStatistics.h"
#include "CMainWindow.h"
#include "ui_CMainWindow.h"

struct ChartData
{
    QString title;
    QString columnNames;
    QString dataSource;
    QString dataColumn;
};

static const ChartData chartData[6] = {
    { "Code element coverage histogram", "'Number of affected test cases by a code element', 'Occurences'", COV_STATS, "code_coverage_histogram" },
    { "Test case coverage histogram", "'Number of covered code elements in a test case', 'Occurences'", COV_STATS, "test_coverage_histogram" },
    { "Results fail histogram", "'Number of failed test cases in a revision', 'Occurences'", RES_STATS, "fail_histogram" },
    { "Revision fail histogram", "'Revision number', 'Number of failed test cases'", RES_STATS, "revision_fail_histogram"},
    { "Executed histogram", "'Number of executed test cases', 'Occurences'", RES_STATS, "executed_histogram" },
    { "Test case info", "'Test case id', 'Executed', 'Fail'", RES_STATS, "test_case_info" }
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

            "<div><h3>Results statistics</h3>"
            "Number of revisions: %6</br>"
            "Number of test cases: %7</br>"
            "Number of total failed test cases: %8</br>"
            "Average failed test cases per revision: %9</br>"
              "</div></body></html>";

    rapidjson::Document *covData = m_workspace->getResultsByName(COV_STATS);
    rapidjson::Document *resData = m_workspace->getResultsByName(RES_STATS);

    html = html.arg(QString::number((*covData)["number_of_code_elements"].GetInt()),
            QString::number((*covData)["number_of_test_cases"].GetInt()),
            QString::number((*covData)["density"].GetDouble()),
            QString::number((*covData)["average_test_cases_per_code_elements"].GetDouble()),
            QString::number((*covData)["average_code_elements_per_test_cases"].GetDouble()),
            QString::number((*resData)["number_of_revisions"].GetInt()),
            QString::number((*resData)["number_of_test_cases"].GetInt()),
            QString::number((*resData)["number_of_total_fails"].GetInt()),
            QString::number((*resData)["average_failed_test_cases_per_revision"].GetDouble()));

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
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">google.load('visualization', '1.1', {packages: ['table']});</script>"
            "<script type=\"text/javascript\">"
              "google.setOnLoadCallback(drawChart);"
              "function drawChart() {"
                "var data = google.visualization.arrayToDataTable(["
                      "[%1],"
                "%2]);"
                "var table = new google.visualization.Table(document.getElementById('table'));"
                "table.draw(data, {page:'enable',pageSize:60});"
              "}"
            "</script></head><body>"
            "<h3 style=\"text-align:center;\">%3</h3>"
            "<div id=\"table\">"
              "</div></body></html>";

    rapidjson::Document *data = m_workspace->getResultsByName(chartData[tabindex].dataSource.toStdString());
    QString dataStr;
    if (chartData[tabindex].dataColumn == "test_case_info") {
        const rapidjson::Value& val = (*data)["test_case_info"];
        for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
            dataStr.append("[" + QString(itr->name.GetString()) + "," + QString::number(itr->value["executed"].GetInt()) + "," +  QString::number(itr->value["fail"].GetInt()) + "],");
        }
        dataStr.chop(1);
    } else
        convertJsonToStringArray(data, chartData[tabindex].dataColumn, dataStr);

    html = html.arg(chartData[tabindex].columnNames, dataStr, chartData[tabindex].title);

    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}

void CShowStatistics::convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str)
{
    const rapidjson::Value& val = (*data)[element.toStdString().c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
        if (QString(itr->name.GetString()).compare("0") == 0)
            continue;

        str.append("[" + QString(itr->name.GetString()) + "," + QString::number(itr->value.GetInt()) + "],");
    }
    str.chop(1);
}
