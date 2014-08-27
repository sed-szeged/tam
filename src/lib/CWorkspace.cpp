#include "CWorkspace.h"
#include "CUnqliteWrapper.h"

#include "util/CSelectionStatistics.h"
#include "ui_CMainWindow.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

CWorkspace::CWorkspace(CMainWindow *mainWindow) :
    m_isSaved(false), m_isTestSuiteAvailable(false), m_fileName(QString()),
    m_testSuite(new CSelectionData()), m_mainWindow(mainWindow),
    m_availableFileMask(FILE_NONE)
{
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        m_data[collections[i]] = new rapidjson::Document();
        m_data[collections[i]]->SetObject();
    }

    addMeasurement(METRICS, "default");
    addMeasurement(SCORE, "default");
}

CWorkspace::~CWorkspace()
{
    delete m_testSuite;
    for (std::map<std::string, rapidjson::Document*>::iterator it = m_data.begin(); it != m_data.end(); ++it) {
        delete it->second;
    }
    removeAllMeasurement();
}

bool CWorkspace::save()
{
    CUnqliteWrapper *wrapper = new CUnqliteWrapper(m_fileName.toStdString());
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        wrapper->dropCollection(collections[i]);
        if (!wrapper->storeDocument(collections[i], *m_data[collections[i]]))
            return false;
    }

    saveMeasurement(METRICS, wrapper);
    saveMeasurement(SCORE, wrapper);

    rapidjson::Document clusters;
    clusters.SetObject();
    m_mainWindow->getClusterList()->toJson(clusters);
    if (!wrapper->storeDocument(CLUSTERS, clusters))
        return false;

    delete wrapper;
    m_isSaved = true;
    return true;
}

void CWorkspace::saveMeasurement(String type, CUnqliteWrapper *wrapper)
{
    for (std::map<String, rapidjson::Document*>::iterator docIt = m_measurementsResults[type].begin(); docIt != m_measurementsResults[type].end(); ++docIt) {
        int id;
        wrapper->storeDocument(type, *docIt->second, &id);
        m_measurements[type][docIt->first]->AddMember("result-id", id, m_measurements[type][docIt->first]->GetAllocator());
    }

    for (std::map<String, rapidjson::Document*>::iterator docIt = m_measurements[type].begin(); docIt != m_measurements[type].end(); ++docIt)
        wrapper->storeDocument(type + "-measurements", *docIt->second);
}

void CWorkspace::load()
{
    CUnqliteWrapper *wrapper = new CUnqliteWrapper(m_fileName.toStdString());
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        wrapper->fetchById(collections[i], 0, *m_data[collections[i]]);
    }

    loadResults(METRICS, wrapper);
    loadResults(SCORE, wrapper);

    rapidjson::Document clusters;
    wrapper->fetchById(CLUSTERS, 0, clusters);
    m_mainWindow->getClusterList()->fromJson(clusters);
    delete wrapper;

    updateFileMask();
}

void CWorkspace::loadResults(String type, CUnqliteWrapper *wrapper)
{
    rapidjson::Document meas;
    wrapper->fetchAll(type + "-measurements", meas);
    if (!meas.IsArray())
        return;

    for (rapidjson::Document::ValueIterator it = meas.Begin(); it != meas.End(); ++it) {
        String measName = (*it)["name"].GetString();
        removeMeasurement(type, measName);
        addMeasurement(type, measName);
        m_measurements[type][measName]->CopyFrom((*it), m_measurements[type][measName]->GetAllocator());
        int id = (*m_measurements[type][measName])["result-id"].GetInt();
        wrapper->fetchById(type, id, *m_measurementsResults[type][measName]);

        if (measName != "default") {
            if (type == METRICS)
                m_mainWindow->getUi()->comboBoxMetricsMeasurement->addItem(measName.c_str());
            else
                m_mainWindow->getUi()->comboBoxScoreMeasurement->addItem(measName.c_str());
        }
    }
}

void CWorkspace::setCoveragePath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_data[WS]->FindMember("coverage-binary");
    if (memberIt == m_data[WS]->MemberEnd()) {
        rapidjson::Value s(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());
        m_data[WS]->AddMember("coverage-binary", s, m_data[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());

    if (!path.isEmpty())
        m_availableFileMask |= FILE_COVERAGE;
    else
        m_availableFileMask &= ~FILE_COVERAGE;
}

QString CWorkspace::getCoveragePath()
{
    if (!(m_availableFileMask & FILE_COVERAGE))
        return "";

    return (*m_data[WS])["coverage-binary"].GetString();
}

void CWorkspace::setResultsPath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_data[WS]->FindMember("results-binary");
    if (memberIt == m_data[WS]->MemberEnd()) {
        rapidjson::Value s(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());
        m_data[WS]->AddMember("results-binary", s, m_data[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());

    if (!path.isEmpty())
        m_availableFileMask |= FILE_RESULTS;
    else
        m_availableFileMask &= ~FILE_RESULTS;
}

QString CWorkspace::getResultsPath()
{
    if (!(m_availableFileMask & FILE_RESULTS))
        return "";

    return (*m_data[WS])["results-binary"].GetString();
}

void CWorkspace::setChangesetPath(QString path)
{
    rapidjson::Document::MemberIterator memberIt = m_data[WS]->FindMember("changeset-binary");
    if (memberIt == m_data[WS]->MemberEnd()) {
        rapidjson::Value s;
        s.SetString(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());
        m_data[WS]->AddMember("changeset-binary", s, m_data[WS]->GetAllocator());
    } else
        memberIt->value.SetString(path.toStdString().c_str(), path.length(), m_data[WS]->GetAllocator());

    if (!path.isEmpty())
        m_availableFileMask |= FILE_CHANGESET;
    else
        m_availableFileMask &= ~FILE_CHANGESET;
}

QString CWorkspace::getChangesetPath()
{
    if (!(m_availableFileMask & FILE_CHANGESET))
        return "";

    return (*m_data[WS])["changeset-binary"].GetString();
}

bool CWorkspace::isStatisticsCalculated()
{
    rapidjson::Document::MemberIterator member = m_data[WS]->FindMember("statistics-calculated");
    if (member == m_data[WS]->MemberEnd())
        return false;

    return member->value.GetBool();
}

void CWorkspace::addMeasurement(String type, String name)
{
    m_measurements[type][name] = new rapidjson::Document();
    m_measurements[type][name]->SetObject();
    rapidjson::Value val(name.c_str(), m_measurements[type][name]->GetAllocator());
    m_measurements[type][name]->AddMember(rapidjson::StringRef("name"), val, m_measurements[type][name]->GetAllocator());

    m_measurementsResults[type][name] = new rapidjson::Document();
    m_measurementsResults[type][name]->SetObject();
}

void CWorkspace::removeMeasurement(String type, String name)
{
    if (!m_measurements[type].count(name))
        return;

    delete m_measurements[type][name];
    m_measurements[type].erase(name);

    delete m_measurementsResults[type][name];
    m_measurementsResults[type].erase(name);
}

void CWorkspace::removeAllMeasurement()
{
    for (std::map<String, std::map<String, rapidjson::Document*> >::iterator it = m_measurements.begin(); it != m_measurements.end(); ++it) {
        for (std::map<String, rapidjson::Document*>::iterator docIt = it->second.begin(); docIt != it->second.end(); ++docIt)
            delete docIt->second;
    }
    m_measurements.clear();

    for (std::map<String, std::map<String, rapidjson::Document*> >::iterator it = m_measurementsResults.begin(); it != m_measurementsResults.end(); ++it) {
        for (std::map<String, rapidjson::Document*>::iterator docIt = it->second.begin(); docIt != it->second.end(); ++docIt)
            delete docIt->second;
    }
    m_measurementsResults.clear();
}

void CWorkspace::updateFileMask()
{
    rapidjson::Document::MemberIterator memberIt = m_data[WS]->FindMember("coverage-binary");
    if (memberIt != m_data[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_COVERAGE;
    memberIt = m_data[WS]->FindMember("results-binary");
    if (memberIt != m_data[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_RESULTS;
    memberIt = m_data[WS]->FindMember("changeset-binary");
    if (memberIt != m_data[WS]->MemberEnd() && memberIt->value.GetString() != "")
        m_availableFileMask |= FILE_CHANGESET;
}
