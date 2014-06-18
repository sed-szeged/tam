#include "CShowStatisticsDialog.h"
#include "CMainWindow.h"
#include "ui_CShowStatisticsDialog.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <iostream>

CShowStatisticsDialog::CShowStatisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CShowStatisticsDialog)
{
    ui->setupUi(this);
    createHtml();
}

CShowStatisticsDialog::~CShowStatisticsDialog()
{
    delete ui;
}

void CShowStatisticsDialog::createHtml() {
    QString html = "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
          "<head>"
            "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>"
            "<title> Google Visualization API Sample </title>"
            "<!-- One script tag loads all the required libraries! Do not specify any chart types in the autoload statement. -->"
            "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
            "<script type=\"text/javascript\">"
              "google.load(\"visualization\", \"1\", {packages:[\"corechart\"]});"
              "google.setOnLoadCallback(drawCodeCovChart);"
              "google.setOnLoadCallback(drawTestCovChart);"
              "google.setOnLoadCallback(drawFailResChart);"
              "google.setOnLoadCallback(drawRevFailResChart);"
              "google.setOnLoadCallback(drawExecResChart);"
              "google.setOnLoadCallback(drawTestCaseInfoResChart);"

              "function drawCodeCovChart() {"
                "var data = google.visualization.arrayToDataTable(["
                      "['Number of affected test cases by a code element', 'Occurences'],"
                "%1]);"

                "var options = {"
                  "title: 'Code element coverage histogram',"
                  "legend: { position: 'none' },"
                  "hAxis: {title: 'Number of affected test cases by a code element'},"
                  "vAxis: {title: 'Number of code elements with the same number of affected test cases'}"
                "};"
                "var chart = new google.visualization.ColumnChart(document.getElementById('code_cov_hist'));"
                "chart.draw(data, options);"
              "}"
              "function drawTestCovChart() {"
                  "var data = google.visualization.arrayToDataTable(["
                    "['Number of covered code elements in a test case', 'Occurences'],"
                "%2]);"

                "var options = {"
                    "title: 'Test case coverage histogram',"
                    "legend: { position: 'none' },"
                    "hAxis: {title: 'Number of covered code elements in a test case'},"
                    "vAxis: {title: 'Number of test cases with the same number of covered code elements'}"
                "};"
                "var chart = new google.visualization.ColumnChart(document.getElementById('test_cov_hist'));"
                "chart.draw(data, options);"
              "}"

            "function drawFailResChart() {"
                "var data = google.visualization.arrayToDataTable(["
                  "['Number of failed test cases in a revision', 'Occurences'],"
              "%3]);"

              "var options = {"
                  "title: 'Results fail histogram',"
                  "legend: { position: 'none' },"
                  "hAxis: {title: 'Number of failed test cases in a revision'},"
                  "vAxis: {title: 'Number of revisions with the same number of failed test cases'}"
              "};"
              "var chart = new google.visualization.ColumnChart(document.getElementById('fail_res_hist'));"
              "chart.draw(data, options);"
            "}"
            "function drawRevFailResChart() {"
                "var data = google.visualization.arrayToDataTable(["
                  "['Revision number', 'Number of failed test cases'],"
              "%12]);"

              "var options = {"
                  "title: 'Revision fail histogram',"
                  "legend: { position: 'none' },"
                  "hAxis: {title: 'Revision number'},"
                  "vAxis: {title: 'Number of failed test cases'}"
              "};"
              "var chart = new google.visualization.ColumnChart(document.getElementById('rev_fail_res_hist'));"
              "chart.draw(data, options);"
            "}"
            "function drawTestCaseInfoResChart() {"
                "var data = google.visualization.arrayToDataTable(["
                  "['Test case id', 'Executed', 'Fail'],"
              "%14]);"

              "var options = {"
                  "title: 'Test case info',"
                  "legend: { position: 'none' },"
                  "hAxis: {title: 'Test case id'},"
                  "vAxis: {title: 'Number of Executed/Failed revisions'}"
              "};"
              "var chart = new google.visualization.ColumnChart(document.getElementById('tc_info_res'));"
              "chart.draw(data, options);"
            "}"
            "function drawExecResChart() {"
                "var data = google.visualization.arrayToDataTable(["
                  "['Number of executed test cases', 'Occurences'],"
              "%15]);"

              "var options = {"
                  "title: 'Executed histogram',"
                  "legend: { position: 'none' },"
                  "hAxis: {title: 'Number of executed revisions in a test case'},"
                  "vAxis: {title: 'Number of test cases with the same number of execution'}"
              "};"
              "var chart = new google.visualization.ColumnChart(document.getElementById('exec_res_hist'));"
              "chart.draw(data, options);"
            "}"
            "</script></head><body><div>"
            "<h3>Coverage statistics</h3>"
            "Number of code elements: %4</br>"
            "Number of test cases: %5</br>"
            "Density of the coverage matrix: %6</br>"
            "Average test cases per code elements: %7</br>"
            "Average code elements per test cases: %8</br>"
            "<div id=\"code_cov_hist\" style=\"height:500px; width:1000px;\"></div>"
            "<div id=\"test_cov_hist\" style=\"height:500px; width:1000px;\"></div>"
            "</div>"

            "<div><h3>Results statistics</h3>"
            "Number of revisions: %9</br>"
            "Number of test cases: %10</br>"
            "Number of total failed test cases: %11</br>"
            "Average failed test cases per revision: %13</br>"
            "<div id=\"fail_res_hist\" style=\"height:500px; width:1000px;\"></div>"
            "<div id=\"rev_fail_res_hist\" style=\"height:500px; width:1000px;\"></div>"
            "<div id=\"exec_res_hist\" style=\"height:500px; width:1000px;\"></div>"
            "<div id=\"tc_info_res\" style=\"height:500px; width:1000px;\"></div>"
              "</div></body></html>";

    rapidjson::Document *covData = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(COV_STATS);

    QString codeHist;
    convertJsonToStringArray(covData, "code_coverage_histogram", codeHist);
    QString testHist;
    convertJsonToStringArray(covData, "test_coverage_histogram", testHist);

    rapidjson::Document *resData = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(RES_STATS);

    QString execHist;
    convertJsonToStringArray(resData, "executed_histogram", execHist);
    QString failHist;
    convertJsonToStringArray(resData, "fail_histogram", failHist);
    QString revFailHist;
    convertJsonToStringArray(resData, "revision_fail_histogram", revFailHist);
    QString tcInfo;
    const rapidjson::Value& val = (*resData)["test_case_info"];
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
        if (QString(itr->name.GetString()).compare("0") == 0)
            continue;

        tcInfo.append("['" + QString(itr->name.GetString()) + "'," + QString::number(itr->value["executed"].GetInt()) + "," +  QString::number(itr->value["fail"].GetInt()) + "],");
    }
    tcInfo.chop(1);

    html = html.arg(codeHist,
                    testHist,
                    failHist,
                    QString::number((*covData)["number_of_code_elements"].GetInt()),
            QString::number((*covData)["number_of_test_cases"].GetInt()),
            QString::number((*covData)["density"].GetDouble()),
            QString::number((*covData)["average_test_cases_per_code_elements"].GetDouble()),
            QString::number((*covData)["average_code_elements_per_test_cases"].GetDouble())).arg(
            QString::number((*resData)["number_of_revisions"].GetInt()),
            QString::number((*resData)["number_of_test_cases"].GetInt()),
            QString::number((*resData)["number_of_total_fails"].GetInt()),
            revFailHist,
            QString::number((*resData)["average_failed_test_cases_per_revision"].GetDouble()),
            tcInfo, execHist);


    ui->webView->settings()->clearMemoryCaches();
    ui->webView->setHtml(html);
}

void CShowStatisticsDialog::convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str)
{
    const rapidjson::Value& val = (*data)[element.toStdString().c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
        /*if (QString(itr->name.GetString()).compare("0") == 0)
            continue;*/

        str.append("['" + QString(itr->name.GetString()) + "'," + QString::number(itr->value.GetInt()) + "],");
    }
    str.chop(1);
}
