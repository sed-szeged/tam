#ifndef CSHOWSTATISTICS_H
#define CSHOWSTATISTICS_H

#include "rapidjson/document.h"
#include "lib/CWorkspace.h"
#include <QWebView>


class CShowStatistics
{
public:
    CShowStatistics(CWorkspace *workspace);
    ~CShowStatistics();

    void generateChartForTab(QWebView *view, int tabindex);
    void fillGeneralTab(QWebView *view);

private:
    void convertJsonToStringArray(rapidjson::Document *data, QString element, QString &str);

    CWorkspace *m_workspace;
};

#endif // CSHOWSTATISTICS_H
