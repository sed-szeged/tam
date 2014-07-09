#include "CTestSuiteLoader.h"
#include "CWorkspace.h"

CTestSuiteLoader::CTestSuiteLoader(QObject *parent) : QThread(parent)
{
}

CTestSuiteLoader::~CTestSuiteLoader()
{
    wait();
}

void CTestSuiteLoader::load(CWorkspace *workspace)
{
    m_covPath = workspace->getCoveragePath().toStdString();
    m_resPath = workspace->getResultsPath().toStdString();
    m_changesPath = workspace->getChangesetPath().toStdString();
    m_testSuite = workspace->getTestSuite();
    m_isStatisticsCalculated = workspace->isStatisticsCalculated();

    if (!isRunning())
        start();
}

void CTestSuiteLoader::run()
{
    emit updateStatusLabel("Loading coverage file...");
    m_testSuite->loadCoverage(m_covPath);
    emit updateStatusLabel("Loading results file...");
    m_testSuite->loadResults(m_resPath);

    if (!m_changesPath.empty()) {
        emit updateStatusLabel("Loading changeset file...");
        m_testSuite->loadChangeset(m_changesPath);
    }

    emit updateStatusLabel("Globalizing...");
    m_testSuite->globalize();
    emit processFinished("Test-suite loading finished");

}
