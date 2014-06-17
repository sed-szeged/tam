#ifndef CWORKSPACE_H
#define CWORKSPACE_H

#include <map>
#include "gui/CMainWindow.h"
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

// names of the collections which results will be stored
#define NUM_OF_COLS 3
#define WS collections[0]
#define COV_STATS collections[1]
#define RES_STATS collections[2]

static const char* collections[] = { "workspace", "coverage-statistics", "results-statistics" };

class CWorkspace
{
public:
    CWorkspace(CMainWindow *mainWindow);
    ~CWorkspace();

    void loadTestSuite();
    void calcStatistics();
    bool save();
    void load();
    bool isSaved() { return m_isSaved; }
    void setSaved(bool saved) { m_isSaved = saved; }

    QString getCoveragePath();
    QString getResultsPath();
    QString getChangesetPath();

    QString getFileName() { return m_fileName; }
    void setFileName(QString name) { m_fileName = name; }

    CSelectionData* getTestSuite() { return m_testSuite; }
    rapidjson::Document* getResultsByName(String name) { return m_results[name]; }

    void setBinaryPaths(QString coverage, QString results, QString changes);

private:

    bool m_isChangesAvailable;
    bool m_isSaved;
    QString m_fileName;
    std::map<String, rapidjson::Document*> m_results;
    CSelectionData *m_testSuite;
    CMainWindow *m_mainWindow;
};

#endif // CWORKSPACE_H
