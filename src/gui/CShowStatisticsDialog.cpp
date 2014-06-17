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
              //"google.setOnLoadCallback(drawCodeCovChart);"
              "google.setOnLoadCallback(drawTestCovChart);"

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
                "%7]);"

                "var options = {"
                    "title: 'Test case coverage histogram',"
                    "legend: { position: 'none' },"
                    "hAxis: {title: 'Number of covered code elements in a test case'},"
                    "vAxis: {title: 'Number of test cases with the same number of covered code elements'}"
                "};"
                "var chart = new google.visualization.ColumnChart(document.getElementById('test_cov_hist'));"
                "chart.draw(data, options);"
              "}"
            "</script></head><body><div>"
            "<h3>Coverage statistics</h3>"
            "Number of code elements: %2</br>"
            "Number of test cases: %3</br>"
            "Density of the coverage matrix: %4</br>"
            "Average test cases per code elements: %5</br>"
            "Average code elements per test cases: %6</br>"
            "<div id=\"code_cov_hist\" style=\"height:500px; width:1000px;\"></div>"
            "<div id=\"test_cov_hist\" style=\"height:500px; width:1000px;\"></div>"
            "</div>"

            /*"<div><h3>Results statistics</h3>"
            "Number of revisions: %8</br>"
            "Number of test cases: %9</br>"
            "Number of total failed test cases: %10</br>"
            "<div id=\"exec_res_hist\"></div>"
            "<div id=\"fail_res_hist\"></div>"
            "<div id=\"rev_fail_res_hist\"></div>"
            "<div id=\"tc_info_res\"></div></div>"*/


              "</body></html>";

    rapidjson::Document* data = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getResultsByName(COV_STATS);

    QString codeHist;
    convertJsonToStringArray(data, "code_coverage_histogram", codeHist);
    QString testHist;
    convertJsonToStringArray(data, "test_coverage_histogram", testHist);
    QString execHist;
    QString failHist;
    QString revFailHist;

    html = html.arg(codeHist, QString::number((*data)["number_of_code_elements"].GetInt()), QString::number((*data)["number_of_test_cases"].GetInt()), QString::number((*data)["density"].GetDouble()), QString::number((*data)["average_test_cases_per_code_elements"].GetDouble()), QString::number((*data)["average_code_elements_per_test_cases"].GetDouble()), testHist);

    ui->webView->settings()->clearMemoryCaches();
    ui->webView->setHtml(html);
}

void CShowStatisticsDialog::convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str)
{
    const rapidjson::Value& codeCov = (*data)[element.toStdString().c_str()];
    for (rapidjson::Value::ConstMemberIterator itr = codeCov.MemberBegin(); itr != codeCov.MemberEnd(); ++itr) {
        if (QString(itr->name.GetString()).compare("0") == 0)
            continue;

        str.append("['" + QString(itr->name.GetString()) + "'," + QString::number(itr->value.GetInt()) + "],");
    }
    str.chop(1);
}
