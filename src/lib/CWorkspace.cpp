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

    if (!m_results[WS]->HasMember("metrics"))
        m_results[WS]->AddMember("metrics", rapidjson::Value(rapidjson::kObjectType).Move(), m_results[WS]->GetAllocator());
    if (!m_results[WS]->HasMember("score"))
        m_results[WS]->AddMember("score", rapidjson::Value(rapidjson::kObjectType).Move(), m_results[WS]->GetAllocator());
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
        wrapper->dropCollection(collections[i]);
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

    if (!m_results[WS]->HasMember("metrics"))
        m_results[WS]->AddMember("metrics", rapidjson::Value(rapidjson::kObjectType).Move(), m_results[WS]->GetAllocator());
    if (!m_results[WS]->HasMember("score"))
        m_results[WS]->AddMember("score", rapidjson::Value(rapidjson::kObjectType).Move(), m_results[WS]->GetAllocator());

    delete wrapper;
}

void CWorkspace::setCoveragePath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("coverage-binary");
    if (memberIt == m_results[WS]->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
        m_results[WS]->AddMember("coverage-binary", s, m_results[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
}

QString CWorkspace::getCoveragePath()
{
    rapidjson::Document::MemberIterator member = m_results[WS]->FindMember("coverage-binary");
    if (member == m_results[WS]->MemberEnd() || member->value.IsNull())
        return "";
    return member->value.GetString();
}

void CWorkspace::setResultsPath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("results-binary");
    if (memberIt == m_results[WS]->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
        m_results[WS]->AddMember("results-binary", s, m_results[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
}

QString CWorkspace::getResultsPath()
{
    rapidjson::Document::MemberIterator member = m_results[WS]->FindMember("results-binary");
    if (member == m_results[WS]->MemberEnd() || member->value.IsNull())
        return "";
    return member->value.GetString();
}

void CWorkspace::setChangesetPath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("changeset-binary");
    if (memberIt == m_results[WS]->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
        m_results[WS]->AddMember("changeset-binary", s, m_results[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());

    if (!path.isEmpty())
        m_isChangesAvailable = true;
}

QString CWorkspace::getChangesetPath()
{
    rapidjson::Document::MemberIterator member = m_results[WS]->FindMember("changeset-binary");
    if (member == m_results[WS]->MemberEnd() || member->value.IsNull())
        return "";
    return member->value.GetString();
}

bool CWorkspace::isStatisticsCalculated()
{
    rapidjson::Document::MemberIterator member = m_results[WS]->FindMember("statistics-calculated");
    if (member == m_results[WS]->MemberEnd())
        return false;

    return member->value.GetBool();
}
