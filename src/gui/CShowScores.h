#ifndef CSHOWSCORES_H
#define CSHOWSCORES_H

#include <QWebView>
#include "rapidjson/document.h"
#include "data/CSelectionData.h"

using namespace soda;

class CShowScores
{
public:
    CShowScores();
    ~CShowScores();

    void generateResults(QWebView *view, rapidjson::Document *scores, CSelectionData *selection);
};

#endif // CSHOWSCORES_H
