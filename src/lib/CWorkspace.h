#ifndef CWORKSPACE_H
#define CWORKSPACE_H

#include <map>
#include "gui/CMainWindow.h"
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

// names of the collections where results will be stored
#define NUM_OF_COLS 5
#define WS collections[0]
#define COV_STATS collections[1]
#define RES_STATS collections[2]
#define METRICS "metrics"
#define METRICS_MEAS "metrics-measurements"
#define SCORE "score"
#define SCORE_MEAS "score-measurements"
#define CLUSTERS "clusters"

static const char* collections[NUM_OF_COLS] = { "workspace", "coverage-statistics", "results-statistics", "metrics", "score" };

class CWorkspace
{
public:
    CWorkspace(CMainWindow *mainWindow);
    ~CWorkspace();

    bool save();
    void load();
    bool isStatisticsCalculated();
    bool isSaved() { return m_isSaved; }
    void setSaved(bool saved) { m_isSaved = saved; }

    void setCoveragePath(QString path);
    QString getCoveragePath();
    void setResultsPath(QString path);
    QString getResultsPath();
    void setChangesetPath(QString path);
    QString getChangesetPath();

    QString getFileName() { return m_fileName; }
    void setFileName(QString name) { m_fileName = name; }

    CSelectionData* getTestSuite() { return m_testSuite; }
    rapidjson::Document* getResultsByName(String name) { return m_results[name]; }
    void addMeasurement(String type, String name);
    void removeMeasurement(String type, String name);
    rapidjson::Document* getMeasurement(String type, String name) { return m_measurements[type][name]; }

private:

    bool m_isChangesAvailable;
    bool m_isSaved;
    QString m_fileName;
    std::map<String, rapidjson::Document*> m_results;

    std::map<String, std::map<String, rapidjson::Document*> > m_measurements;
    //std::map<String, std::map<String, rapidjson::Document*> > m_results;
    CSelectionData *m_testSuite;
    CMainWindow *m_mainWindow;
};

#endif // CWORKSPACE_H
