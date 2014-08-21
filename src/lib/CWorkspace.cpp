#include "CWorkspace.h"
#include "CUnqliteWrapper.h"

#include "util/CSelectionStatistics.h"
#include "ui_CMainWindow.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

CWorkspace::CWorkspace(CMainWindow *mainWindow) :
    m_isSaved(false), m_fileName(QString()),
    m_testSuite(new CSelectionData()), m_mainWindow(mainWindow),
    m_availableFileMask(FILE_NONE)
{
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        m_results[collections[i]] = new rapidjson::Document();
        m_results[collections[i]]->SetObject();
    }

    addMeasurement("metric", "default");
    addMeasurement("score", "default");
}

CWorkspace::~CWorkspace()
{
    delete m_testSuite;
    for (std::map<std::string, rapidjson::Document*>::iterator it = m_results.begin(); it != m_results.end(); ++it) {
        delete it->second;
    }
    removeAllMeasurement();
}

bool CWorkspace::save()
{
    CUnqliteWrapper *wrapper = new CUnqliteWrapper(m_fileName.toStdString());
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        wrapper->dropCollection(collections[i]);
        if (!wrapper->storeDocument(collections[i], *m_results[collections[i]]))
            return false;
    }

    for (std::map<String, rapidjson::Document*>::iterator docIt = m_measurements["metric"].begin(); docIt != m_measurements["metric"].end(); ++docIt)
        wrapper->storeDocument(METRICS_MEAS, *docIt->second);
    for (std::map<String, rapidjson::Document*>::iterator docIt = m_measurements["score"].begin(); docIt != m_measurements["score"].end(); ++docIt)
        wrapper->storeDocument(SCORE_MEAS, *docIt->second);

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
    wrapper->fetchById(CLUSTERS, 0, clusters);
    m_mainWindow->getClusterList()->fromJson(clusters);

    rapidjson::Document meas;
    wrapper->fetchAll(METRICS_MEAS, meas);
    if (meas.IsArray()) {
        for (rapidjson::Document::ValueIterator it = meas.Begin(); it != meas.End(); ++it) {
            String measName = (*it)["name"].GetString();
            removeMeasurement("metric", measName);
            addMeasurement("metric", measName);
            m_measurements["metric"][measName]->CopyFrom((*it), m_measurements["metric"][measName]->GetAllocator());
        }
    }
    delete wrapper;

    updateFileMask();
}

void CWorkspace::setCoveragePath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("coverage-binary");
    if (memberIt == m_results[WS]->MemberEnd()) {
        rapidjson::Value s(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
        m_results[WS]->AddMember("coverage-binary", s, m_results[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());

    if (!path.isEmpty())
        m_availableFileMask |= FILE_COVERAGE;
    else
        m_availableFileMask &= ~FILE_COVERAGE;
}

QString CWorkspace::getCoveragePath()
{
    if (!(m_availableFileMask & FILE_COVERAGE))
        return "";

    return (*m_results[WS])["coverage-binary"].GetString();
}

void CWorkspace::setResultsPath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("results-binary");
    if (memberIt == m_results[WS]->MemberEnd()) {
        rapidjson::Value s(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());
        m_results[WS]->AddMember("results-binary", s, m_results[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_results[WS]->GetAllocator());

    if (!path.isEmpty())
        m_availableFileMask |= FILE_RESULTS;
    else
        m_availableFileMask &= ~FILE_RESULTS;
}

QString CWorkspace::getResultsPath()
{
    if (!(m_availableFileMask & FILE_RESULTS))
        return "";

    return (*m_results[WS])["results-binary"].GetString();
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
        m_availableFileMask |= FILE_CHANGESET;
    else
        m_availableFileMask &= ~FILE_CHANGESET;
}

QString CWorkspace::getChangesetPath()
{
    if (!(m_availableFileMask & FILE_CHANGESET))
        return "";

    return (*m_results[WS])["changeset-binary"].GetString();
}

bool CWorkspace::isStatisticsCalculated()
{
    rapidjson::Document::MemberIterator member = m_results[WS]->FindMember("statistics-calculated");
    if (member == m_results[WS]->MemberEnd())
        return false;

    return member->value.GetBool();
}

void CWorkspace::addMeasurement(String type, String name)
{
    m_measurements[type][name] = new rapidjson::Document();
    m_measurements[type][name]->SetObject();
    rapidjson::Value val(name.c_str(), m_measurements[type][name]->GetAllocator());
    m_measurements[type][name]->AddMember(rapidjson::StringRef("name"), val, m_measurements[type][name]->GetAllocator());
}

void CWorkspace::removeMeasurement(String type, String name)
{
    if (!m_measurements[type].count(name))
        return;

    delete m_measurements[type][name];
    m_measurements[type].erase(name);
}

void CWorkspace::removeAllMeasurement()
{
    for (std::map<String, std::map<String, rapidjson::Document*> >::iterator it = m_measurements.begin(); it != m_measurements.end(); ++it) {
        for (std::map<String, rapidjson::Document*>::iterator docIt = it->second.begin(); docIt != it->second.end(); ++docIt)
            delete docIt->second;
    }
    m_measurements.clear();
}

void CWorkspace::updateFileMask()
{
    rapidjson::Document::MemberIterator memberIt = m_results[WS]->FindMember("coverage-binary");
    if (memberIt != m_results[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_COVERAGE;
    memberIt = m_results[WS]->FindMember("results-binary");
    if (memberIt != m_results[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_RESULTS;
    memberIt = m_results[WS]->FindMember("changeset-binary");
    if (memberIt != m_results[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_CHANGESET;
}
