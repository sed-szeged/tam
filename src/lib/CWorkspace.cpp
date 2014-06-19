#include "CWorkspace.h"
#include "CUnqliteWrapper.h"

#include "util/CSelectionStatistics.h"
#include "ui_CMainWindow.h"
#include <iostream>
#include "rapidjson/prettywriter.h"

CWorkspace::CWorkspace(CMainWindow *mainWindow) :
    m_isChangesAvailable(false), m_isSaved(false), m_fileName(QString()),
    m_testSuite(new CSelectionData()), m_mainWindow(mainWindow)
{
    m_mainWindow->getUi()->textBrowserOutput->append("New workspace created...");
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        m_results[collections[i]] = new rapidjson::Document();
        m_results[collections[i]]->SetObject();
    }
}

CWorkspace::~CWorkspace()
{
    delete m_testSuite;
    for (std::map<std::string, rapidjson::Document*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
        delete it->second;
    }
}

bool CWorkspace::save()
{
    CUnqliteWrapper *wrapper = new CUnqliteWrapper(m_fileName.toStdString());
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        if (!wrapper->storeDocument(collections[i], *m_results[collections[i]]))
            return false;
    }
    delete wrapper;
    m_isSaved = true;
    return true;
}

void CWorkspace::load()
{
    CUnqliteWrapper *wrapper = new CUnqliteWrapper(m_fileName.toStdString());
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        wrapper->fetchById(collections[i], 0, *m_results[collections[i]]);
    }
    delete wrapper;
}

QString CWorkspace::getCoveragePath()
{
    if (m_results[WS]->FindMember("coverage_binary") == NULL || (*m_results[WS])["coverage_binary"].IsNull())
        return "";
    return (*m_results[WS])["coverage_binary"].GetString();
}

QString CWorkspace::getResultsPath()
{
    if (m_results[WS]->FindMember("results_binary") == NULL || (*m_results[WS])["results_binary"].IsNull())
        return "";
    return (*m_results[WS])["results_binary"].GetString();
}

QString CWorkspace::getChangesetPath()
{
    if (m_results[WS]->FindMember("changeset_binary") == NULL || (*m_results[WS])["changeset_binary"].IsNull())
        return "";
    return (*m_results[WS])["changeset_binary"].GetString();
}

void CWorkspace::setBinaryPaths(QString coverage, QString results, QString changes)
{
    rapidjson::Value s;
    s.SetString(coverage.toStdString().c_str(), coverage.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("coverage_binary", s, m_results[WS]->GetAllocator());

    s.SetString(results.toStdString().c_str(), results.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("results_binary", s, m_results[WS]->GetAllocator());

    s.SetString(changes.toStdString().c_str(), changes.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("changeset_binary", s, m_results[WS]->GetAllocator());

    if (!changes.isEmpty())
        m_isChangesAvailable = true;
}

void CWorkspace::loadTestSuite()
{
    m_mainWindow->getUi()->textBrowserOutput->append("Loading coverage data from path: " + QString((*m_results[WS])["coverage_binary"].GetString()) + " ...");
    m_testSuite->loadCoverage((*m_results[WS])["coverage_binary"].GetString());

    m_mainWindow->getUi()->textBrowserOutput->append("Loading results data from path: " + QString((*m_results[WS])["results_binary"].GetString()) + " ...");
    m_testSuite->loadResults((*m_results[WS])["results_binary"].GetString());

    if (m_isChangesAvailable) {
        m_mainWindow->getUi()->textBrowserOutput->append("Loading changeset data from path: " + QString((*m_results[WS])["changes_binary"].GetString()) + " ...");
        m_testSuite->loadChangeset((*m_results[WS])["changes_binary"].GetString());
    }

    m_testSuite->globalize();
    m_mainWindow->getUi()->textBrowserOutput->append("Test-Suite loading finished.");
    calcStatistics();
}

void CWorkspace::calcStatistics()
{
    m_mainWindow->getUi()->textBrowserOutput->append("Calculating statistics...");
    CSelectionStatistics stat(m_testSuite);

    stat.calcCoverageRelatedStatistics(*m_results[COV_STATS]);

    stat.calcCovResultsSummary(*m_results[RES_STATS]);
    stat.calcFailStatistics(*m_results[RES_STATS]);

    m_mainWindow->getUi()->textBrowserOutput->append("done.");
}
