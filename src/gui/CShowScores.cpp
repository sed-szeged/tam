#include "CShowScores.h"
#include <iostream>

CShowScores::CShowScores()
{
}

CShowScores::~CShowScores()
{
}

void CShowScores::generateResults(QWebView *view, rapidjson::Document *scores, CSelectionData *selection)
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
                        "$('a').on('click', function(){"
                          "$(this).next().toggle();"
                        "});"
                        "var tableData;"
                        "%1"
                      "}"
                "</script></head><body onload=\"init()\">"
                "<h2 style=\"text-align:center;\">Lists of possible failed code elements</h2>"
                "%2"
                "</body></html>";

    QString tableData = "tableData = [%1];"
            "$('#%2').DataTable({"
            "\"language\": { \"emptyTable\": \"The suspiciousness value is 0 for every method.\""
            "}, data: tableData, order: [ 1, 'desc' ], pageLength: 20 });";
    QString tableDef = "<table id=\"%1\" class=\"display\">"
            "<thead><tr><th>Code element</th>"
            "%2</tr></thead>"
            "<tbody></tbody></table>";

    QString callback;
    QString body;
    for (rapidjson::Value::ConstMemberIterator clusterIt = scores->MemberBegin(); clusterIt != scores->MemberEnd(); ++clusterIt) {
        QString cluster = clusterIt->name.GetString();
        body.append("<h3 style=\"text-align:center;\">" + cluster + "</h3>");

        std::map<IndexType, std::map<String, double> > flScore;
        for (rapidjson::Value::ConstMemberIterator flIt = clusterIt->value.MemberBegin(); flIt != clusterIt->value.MemberEnd(); ++flIt) {
            QString flName = flIt->name.GetString();
            if (flName.contains("-fl")) {
                for (rapidjson::Value::ConstMemberIterator scoreIt = flIt->value.MemberBegin(); scoreIt != flIt->value.MemberEnd(); ++scoreIt) {
                    flScore[QString(scoreIt->name.GetString()).toLongLong()][flName.replace("-fl", "").toStdString()] = scoreIt->value.GetDouble();
                }
                continue;
            }

            body.append("<a href=\"#\"><h4>" + flName + "</h4></a>");
            body.append(tableDef.arg(cluster.replace(" ", "") + "_" + flIt->name.GetString(), "<th>suspiciousness value</th>"));
            QString data;
            for (rapidjson::Value::ConstMemberIterator scoreIt = flIt->value.MemberBegin(); scoreIt != flIt->value.MemberEnd(); ++scoreIt) {
                if (scoreIt->value.GetDouble() == 0)
                    continue;

                QString codeElementName;
                if (selection->getCodeElements()->size() == 0) // test suite not available
                    codeElementName = scoreIt->name.GetString();
                else
                    codeElementName = selection->getCodeElements()->getValue(QString(scoreIt->name.GetString()).toLongLong()).c_str();

                data.append("['" + codeElementName + "'," +
                            QString::number(scoreIt->value.GetDouble()) + "],");
            }
            data.chop(1);
            callback.append(tableData.arg(data, cluster.replace(" ", "") + "_" + flIt->name.GetString()));
        }

        if (flScore.empty())
            continue;

        QString column;
        QString data;
        bool first = true;
        for (std::map<IndexType, std::map<String, double> >::iterator ceIt = flScore.begin(); ceIt != flScore.end(); ++ceIt) {
            QString codeElementName;
            if (selection->getCodeElements()->size() == 0) // test suite not available
                codeElementName = QString::number(ceIt->first);
            else
                codeElementName = selection->getCodeElements()->getValue(ceIt->first).c_str();

            data.append("['" + codeElementName + "',");
            for (std::map<String, double>::iterator scoreIt = ceIt->second.begin(); scoreIt != ceIt->second.end(); ++scoreIt) {
                if (first)
                    column.append("<th>" + QString(scoreIt->first.c_str()) + "</th>");
                data.append(QString::number(scoreIt->second) + ",");
            }
            data.chop(1);
            first = false;
            data.append("],");
        }
        data.chop(1);
        body.append("<h4>Fault localization score</h4>");
        body.append(tableDef.arg(cluster + "_fl", column));
        callback.append(tableData.arg(data, cluster + "_fl"));
    }

    html = html.arg(callback, body);

    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}
