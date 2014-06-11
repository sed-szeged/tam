#ifndef CWORKSPACE_H
#define CWORKSPACE_H

#include <map>
#include "gui/CMainWindow.h"
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

class CWorkspace
{
public:
    CWorkspace(const char* name, CMainWindow *mainWindow);
    ~CWorkspace();

    void loadTestSuite(QString coverage, QString results, QString changes);
    void calcStatistics();

    CSelectionData* getTestSuite() { return m_testSuite; }
    String getName() { return m_name; }
    void setName(String name) { m_name = name; }

private:

    bool m_isChangesAvailable;
    String m_name;
    std::map<std::string, rapidjson::Document*> m_results;
    CSelectionData *m_testSuite;
    CMainWindow *m_mainWindow;
};

#endif // CWORKSPACE_H
