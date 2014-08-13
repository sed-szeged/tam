#include "CShowScores.h"

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
                "<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>"
                "<script type=\"text/javascript\">google.load('visualization', '1', {packages:['table']});</script>"
                "<script type=\"text/javascript\">"
                      "google.setOnLoadCallback(drawTable);"
                      "function drawTable() {"
                        "var data;"
                        "var table;"
                        "%1"
                      "}"
                "</script></head><body>"
                "<h3 style=\"text-align:center;\">Lists of possible failed code elements</h3>"
                "%2"
                "</body></html>";

    QString table = "data=new google.visualization.DataTable();"
            "data.addColumn('string','Code Element');"
            "%1"
            "data.addRows(["
              "%2"
            "]);"
            "table=new google.visualization.Table(document.getElementById('%3'));"
            "table.draw(data,{page:'enable',pageSize:20,sortAscending:false,sortColumn:1});";

    QString callback;
    QString body;
    for (rapidjson::Value::ConstMemberIterator clusterIt = scores->MemberBegin(); clusterIt != scores->MemberEnd(); ++clusterIt) {
        QString cluster = clusterIt->name.GetString();
        body.append("<h4 style=\"text-align:center;\">" + cluster + "</h4>");

        std::map<IndexType, std::map<String, double> > flScore;
        for (rapidjson::Value::ConstMemberIterator flIt = clusterIt->value.MemberBegin(); flIt != clusterIt->value.MemberEnd(); ++flIt) {
            QString flName = flIt->name.GetString();
            if (flName.contains("-fl")) {
                for (rapidjson::Value::ConstMemberIterator scoreIt = flIt->value.MemberBegin(); scoreIt != flIt->value.MemberEnd(); ++scoreIt) {
                    flScore[QString(scoreIt->name.GetString()).toLongLong()][flName.replace("-fl", "").toStdString()] = scoreIt->value.GetDouble();
                }
                continue;
            }

            body.append("<h5>" + flName + "</h5>");
            body.append("<div id=\"" + cluster + "_" + flIt->name.GetString() + "\"></div>");
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
            callback.append(table.arg("data.addColumn('number','suspiciousness value');", data, cluster + "_" + flIt->name.GetString()));
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
                    column.append("data.addColumn('number','" + QString(scoreIt->first.c_str()) + "');");
                data.append(QString::number(scoreIt->second) + ",");
            }
            data.chop(1);
            first = false;
            data.append("],");
        }
        data.chop(1);
        body.append("<h5>Fault localization score</h5>");
        body.append("<div id=\"" + cluster + "_fl\"></div>");
        callback.append(table.arg(column, data, cluster + "_fl"));
    }

    html = html.arg(callback, body);

    view->settings()->clearMemoryCaches();
    view->setHtml(html);
}
