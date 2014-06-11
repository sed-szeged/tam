#include "CWorkspace.h"
#include "util/CSelectionStatistics.h"
#include "ui_CMainWindow.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CWorkspace::CWorkspace(const char *name, CMainWindow *mainWindow) :
    m_isChangesAvailable(false), m_name(name),
    m_testSuite(new CSelectionData()), m_mainWindow(mainWindow)
{
}

CWorkspace::~CWorkspace()
{
    delete m_testSuite;
    for (std::map<std::string, rapidjson::Document*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
        delete it->second;
    }
}

void CWorkspace::loadTestSuite(QString coverage, QString results, QString changes)
{
    m_mainWindow->getUi()->textBrowserOutput->append("Loading coverage data from path: " + coverage + " ...");
    m_testSuite->loadCoverage(coverage.toStdString());

    m_mainWindow->getUi()->textBrowserOutput->append("Loading results data from path: " + results + " ...");
    m_testSuite->loadResults(results.toStdString());

    if (!changes.isEmpty()) {
        m_mainWindow->getUi()->textBrowserOutput->append("Loading changeset data from path: " + changes + " ...");
        m_testSuite->loadChangeset(changes.toStdString());
        m_isChangesAvailable = true;
    }

    m_testSuite->globalize();
    m_mainWindow->getUi()->textBrowserOutput->append("Test-Suite loading finished.");
    calcStatistics();
}

void CWorkspace::calcStatistics()
{
    m_mainWindow->getUi()->textBrowserOutput->append("Calculating statistics...");
    CSelectionStatistics stat(m_testSuite);

    m_results["coverage-statistics"] = new rapidjson::Document();
    m_results["coverage-statistics"]->SetObject();
    stat.calcCoverageRelatedStatistics(*m_results["coverage-statistics"]);
    m_results["results-statistics"] = new rapidjson::Document();
    m_results["results-statistics"]->SetObject();
    stat.calcCovResultsSummary(*m_results["results-statistics"]);
    stat.calcFailStatistics(*m_results["results-statistics"]);
    m_mainWindow->getUi()->textBrowserOutput->append("done.");
    m_mainWindow->getUi()->textBrowserOutput->clear();

    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
    m_results["results-statistics"]->Accept(writer);
    m_mainWindow->getUi()->textBrowserOutput->append(s.GetString());
}
