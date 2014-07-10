#include "CWorkspace.h"
#include "CUnqliteWrapper.h"

#include "util/CSelectionStatistics.h"
#include "ui_CMainWindow.h"

CWorkspace::CWorkspace(CMainWindow *mainWindow) :
    m_isChangesAvailable(false), m_isSaved(false), m_fileName(QString()),
    m_testSuite(new CSelectionData()), m_mainWindow(mainWindow)
{
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
    rapidjson::Document clusters;
    clusters.SetObject();
    m_mainWindow->getClusterList()->toJson(clusters);
    if (!wrapper->storeDocument(CLUSTERS, clusters))
        return false;

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
    rapidjson::Document clusters;
    clusters.SetObject();
    wrapper->fetchById(CLUSTERS, 0, clusters);
    m_mainWindow->getClusterList()->fromJson(clusters);
    delete wrapper;
}

void CWorkspace::setCoveragePath(QString path)
{
    rapidjson::Value s;
    s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("coverage-binary", s, m_results[WS]->GetAllocator());
}

QString CWorkspace::getCoveragePath()
{
    if (m_results[WS]->FindMember("coverage-binary") == NULL || (*m_results[WS])["coverage-binary"].IsNull())
        return "";
    return (*m_results[WS])["coverage-binary"].GetString();
}

void CWorkspace::setResultsPath(QString path)
{
    rapidjson::Value s;
    s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("results-binary", s, m_results[WS]->GetAllocator());
}

QString CWorkspace::getResultsPath()
{
    if (m_results[WS]->FindMember("results-binary") == NULL || (*m_results[WS])["results-binary"].IsNull())
        return "";
    return (*m_results[WS])["results-binary"].GetString();
}

void CWorkspace::setChangesetPath(QString path)
{
    rapidjson::Value s;
    s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
    m_results[WS]->AddMember("changeset-binary", s, m_results[WS]->GetAllocator());

    if (!path.isEmpty())
        m_isChangesAvailable = true;
}

QString CWorkspace::getChangesetPath()
{
    if (m_results[WS]->FindMember("changeset-binary") == NULL || (*m_results[WS])["changeset-binary"].IsNull())
        return "";
    return (*m_results[WS])["changeset-binary"].GetString();
}

bool CWorkspace::isStatisticsCalculated()
{
    if (m_results[WS]->FindMember("statistics-calculated") == NULL)
        return false;

    return (*m_results[WS])["statistics-calculated"].GetBool();
}
