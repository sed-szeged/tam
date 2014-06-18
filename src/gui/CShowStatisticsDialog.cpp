#include "CShowStatisticsDialog.h"
#include "CMainWindow.h"
#include "ui_CShowStatisticsDialog.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"
#include <iostream>

struct ChartData
{
    QString title;
    QString columnNames;
    QString hAxisName;
    QString vAxisName;
    QString dataSource;
    QString dataColumn;
};

static const ChartData chartData[6] = {
    { "Code element coverage histogram", "'Number of affected test cases by a code element', 'Occurences'", "Number of affected test cases by a code element", "Number of code elements with the same number of affected test cases", COV_STATS, "code_coverage_histogram" },
    { "Test case coverage histogram", "'Number of covered code elements in a test case', 'Occurences'", "Number of covered code elements in a test case", "Number of test cases with the same number of covered code elements", COV_STATS, "test_coverage_histogram" },
    { "Results fail histogram", "'Number of failed test cases in a revision', 'Occurences'", "Number of failed test cases in a revision", "Number of revisions with the same number of failed test cases", RES_STATS, "fail_histogram" },
    { "Revision fail histogram", "'Revision number', 'Number of failed test cases'", "Revision number", "Number of failed test cases", RES_STATS, "revision_fail_histogram"},
    { "Executed histogram", "'Number of executed test cases', 'Occurences'", "Number of executed revisions in a test case", "Number of test cases with the same number of execution", RES_STATS, "executed_histogram" },
    { "Test case info", "'Test case id', 'Executed', 'Fail'", "Test case id", "Number of Executed/Failed revisions", RES_STATS, "test_case_info" }
};

CShowStatisticsDialog::CShowStatisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CShowStatisticsDialog)
{
    ui->setupUi(this);
    fillGeneralTab();
    // 0 is general
    for (int i = 1; i < ui->tabWidget->count(); ++i) {
        QWidget* widget = ui->tabWidget->widget(i);
        QList<QWebView*> webView = widget->findChildren<QWebView*>();
        if (webView.count() == 1)
            generateChartForTab(webView[0], i - 1);
    }
}

CShowStatisticsDialog::~CShowStatisticsDialog()
{
    delete ui;
}

void CShowStatisticsDialog::fillGeneralTab()
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
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

    rapidjson::Document *covData = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(COV_STATS);
    rapidjson::Document *resData = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(RES_STATS);

    html = html.arg(QString::number((*covData)["number_of_code_elements"].GetInt()),
            QString::number((*covData)["number_of_test_cases"].GetInt()),
            QString::number((*covData)["density"].GetDouble()),
            QString::number((*covData)["average_test_cases_per_code_elements"].GetDouble()),
            QString::number((*covData)["average_code_elements_per_test_cases"].GetDouble()),
            QString::number((*resData)["number_of_revisions"].GetInt()),
            QString::number((*resData)["number_of_test_cases"].GetInt()),
            QString::number((*resData)["number_of_total_fails"].GetInt()),
            QString::number((*resData)["average_failed_test_cases_per_revision"].GetDouble()));

    ui->webViewGen->settings()->clearMemoryCaches();
    ui->webViewGen->setHtml(html);
}

void CShowStatisticsDialog::generateChartForTab(QWebView *view, int tabindex)
{
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<title> Google Visualization API Sample </title>"
            "<!-- One script tag loads all the required libraries! Do not specify any chart types in the autoload statement. -->"
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">"
              "google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});"
              "google.setOnLoadCallback(drawChart);"

              "function drawChart() {"
                "var data = google.visualization.arrayToDataTable(["
                      "[%1],"
                "%2]);"

                "var options = {"
                  "title: '%3',"
                  "width: 1000,"
                  "legend: { position: 'none' },"
                  "hAxis: {title: '%4'},"
                  "vAxis: {title: '%5'}"
                "};"
                "var chart = new google.visualization.ColumnChart(document.getElementById('chart'));"
                "chart.draw(data, options);"
              "}"
            "</script></head><body>"
            "<div id=\"chart\" style=\"height:800px; width:1000px;\"></div>"
              "</body></html>";

    rapidjson::Document *data = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(chartData[tabindex].dataSource.toStdString());
    QString dataStr;
    if (chartData[tabindex].dataColumn == "test_case_info") {
        const rapidjson::Value& val = (*data)["test_case_info"];
        for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
            dataStr.append("['" + QString(itr->name.GetString()) + "'," + QString::number(itr->value["executed"].GetInt()) + "," +  QString::number(itr->value["fail"].GetInt()) + "],");
        }
        dataStr.chop(1);
    } else
        convertJsonToStringArray(data, chartData[tabindex].dataColumn, dataStr);

    html = html.arg(chartData[tabindex].columnNames, dataStr, chartData[tabindex].title, chartData[tabindex].hAxisName, chartData[tabindex].vAxisName);

    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}

void CShowStatisticsDialog::convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str)
{
    const rapidjson::Value& val = (*data)[element.toStdString().c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
        if (QString(itr->name.GetString()).compare("0") == 0)
            continue;

        str.append("['" + QString(itr->name.GetString()) + "'," + QString::number(itr->value.GetInt()) + "],");
    }
    str.chop(1);
}
