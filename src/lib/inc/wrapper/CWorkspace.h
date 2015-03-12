/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CWORKSPACE_H
#define CWORKSPACE_H

#include <map>
#include "ui/CMainWindow.h"
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

class CUnqliteWrapper;

enum FileMask
{
    FILE_NONE = 0x0,
    FILE_COVERAGE = 0x1,
    FILE_RESULTS = 0x2,
    FILE_CHANGESET = 0x4
};

// names of the collections where results will be stored
#define NUM_OF_COLS 3
#define WS collections[0]
#define COV_STATS collections[1]
#define RES_STATS collections[2]
#define METRICS "metrics"
#define METRICS_MEAS "metrics-measurements"
#define SCORE "score"
#define SCORE_MEAS "score-measurements"
#define CLUSTERS "clusters"

static const char* collections[NUM_OF_COLS] = { "workspace", "coverage-statistics", "results-statistics" };

/**
 * @brief The CWorkspace class contains the settings, file paths and results of measurements. In addition provides methods for accessing these elements.
 */
class CWorkspace
{
public:
    /**
     * @brief Creates an object of CWorkspace with the given parameter.
     * @param mainWindow Instance of CMainWindow class.
     */
    CWorkspace(CMainWindow *mainWindow);
    ~CWorkspace();

    /**
     * @brief Saves the stored datas to a specified Unqlite database file.
     * @return True if the saving attempt was successful.
     */
    bool save();

    /**
     * @brief Loads the saved datas from an Unqlite database file.
     */
    void load();

    /**
     * @brief Informs the caller if there is an available test-suite in the workspace.
     * @return True if test-suite data is loaded from SoDA files.
     */
    bool isTestSuiteAvailable() { return m_isTestSuiteAvailable; }

    /**
     * @brief Sets the m_isTestSuiteAvailable variable when a test-suite is loaded.
     * @param available True if a test-suite is loaded.
     */
    void setTestSuiteAvailable(bool available) { m_isTestSuiteAvailable = available; }

    /**
     * @brief Returns true if statistics has already calculated on the current test-suite.
     * @return True if statistics has already calculated on the current test-suite.
     */
    bool isStatisticsCalculated();

    /**
     * @brief Returns true if the current workspace has already saved.
     * @return True if the current workspace has already saved.
     */
    bool isSaved() { return m_isSaved; }

    /**
     * @brief Sets the m_isSaved variable with the specified value.
     * @param saved True if the current workspace has been saved.
     */
    void setSaved(bool saved) { m_isSaved = saved; }

    /**
     * @brief Sets the path of the given coverage file in the workspace.
     * @param path Path of a SoDA coverage file.
     */
    void setCoveragePath(QString path);

    /**
     * @brief Returns the path of the current coverage file.
     * @return Path of the current coverage file as string.
     */
    QString getCoveragePath();

    /**
     * @brief Sets the path of the given results file in the workspace.
     * @param path Path of a SoDA results file.
     */
    void setResultsPath(QString path);

    /**
     * @brief Returns the path of the current results file.
     * @return Path of the current results file as string.
     */
    QString getResultsPath();

    /**
     * @brief Sets the path of the given changeset file in the workspace.
     * @param path Path of a SoDA changeset file.
     */
    void setChangesetPath(QString path);

    /**
     * @brief Returns the path of the current changeset file.
     * @return Path of the current changeset file as string.
     */
    QString getChangesetPath();

    /**
     * @brief Returns the path to the database file where the workspace is stored.
     * @return Path of the database file.
     */
    QString getFileName() { return m_fileName; }

    /**
     * @brief Sets the path of the database file where the current workspace is stored.
     * @param name Path of the database file.
     */
    void setFileName(QString name) { m_fileName = name; }

    /**
     * @brief Returns a pointer to the current test-suite.
     * @return A pointer to the test-suite of the current workspace.
     */
    CSelectionData* getTestSuite() { return m_testSuite; }

    /**
     * @brief Returns a pointer to a specified json object which holds various elements.
     * @param name Name of the requested object.
     * @return Json object which holds the specified data.
     */
    rapidjson::Document *getData(String name) { return m_data[name]; }

    /**
     * @brief Creates json objects for the specified measurement.
     * @param type Type of the measurement.
     * @param name Name of the measurement.
     */
    void addMeasurement(String type, String name);

    /**
     * @brief Removes the settings and the results of the specified measurement from the workspace.
     * @param type Type of the measurement.
     * @param name Name of the measurement.
     */
    void removeMeasurement(String type, String name);

    /**
     * @brief Removes all measurements from the current workspace.
     */
    void removeAllMeasurement();

    /**
     * @brief Returns a pointer to a json object which holds the settings of the specified measurements.
     * @param type Type of the measurement.
     * @param name Name of the measurement.
     * @return Pointer to a json object which holds the settings of the specified measurement.
     */
    rapidjson::Document *getMeasurement(String type, String name) { return m_measurements[type][name]; }

    /**
     * @brief Returns a pointer to a json object which holds the results of the specified measurements.
     * @param type Type of the measurement.
     * @param name Name of the measurement.
     * @return Pointer to a json object which holds the results.
     */
    rapidjson::Document *getMeasurementResults(String type, String name) { return m_measurementsResults[type][name]; }

    /**
     * @brief Returns the stored available file mask.
     * @return Stored file mask.
     */
    int getFileMask() { return m_availableFileMask; }

private:

    /**
     * @brief Saves the specified measurement data to the given database.
     * @param type Type of the measurement.
     * @param wrapper Pointer to the current database interface.
     */
    void saveMeasurement(String type, CUnqliteWrapper *wrapper);

    /**
     * @brief Loads the results of the specified measurement from the given database.
     * @param type Type of the measurement.
     * @param wrapper Pointer to the current database.
     */
    void loadResults(String type, CUnqliteWrapper *wrapper);

    /**
     * @brief Updates m_availableFileMask variable based on the available file paths.
     */
    void updateFileMask();

    /**
     * @brief m_isSaved stores whether the current workspace was saved.
     */
    bool m_isSaved;

    /**
     * @brief m_isTestSuiteAvailablestores stores if the current workspace has a test-suite loaded
     */
    bool m_isTestSuiteAvailable;

    /**
     * @brief Path of the database file.
     */
    QString m_fileName;

    /**
     * @brief Stores essential datas and settings about the current workspace in json.
     */
    std::map<String, rapidjson::Document *> m_data;

    /**
     * @brief Stores the settings of the created measurements in json format.
     */
    std::map<String, std::map<String, rapidjson::Document *> > m_measurements;

    /**
     * @brief Stores the results of the computed measurements in json.
     */
    std::map<String, std::map<String, rapidjson::Document *> > m_measurementsResults;

    /**
     * @brief Stores the loaded binary files.
     */
    CSelectionData *m_testSuite;

    /**
     * @brief Instance of the main window of the application.
     */
    CMainWindow *m_mainWindow;

    /**
     * @brief Stores which type of SoDA binary files are available in the current workspace.
     */
    int m_availableFileMask;
};

#endif // CWORKSPACE_H
