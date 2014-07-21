#include "CStatisticsThread.h"
#include "CWorkspace.h"
#include "util/CSelectionStatistics.h"

CStatisticsThread::CStatisticsThread(QObject *parent) : QThread(parent)
{
}

CStatisticsThread::~CStatisticsThread()
{
    wait();
}

void CStatisticsThread::calcStatistics(CWorkspace *workspace)
{
    m_testSuite = workspace->getTestSuite();
    m_covStats = workspace->getResultsByName(COV_STATS);
    m_resStats = workspace->getResultsByName(RES_STATS);

    if (!isRunning())
        start();
}

void CStatisticsThread::run()
{
    CSelectionStatistics stat(m_testSuite);

    emit updateStatusLabel("Calculating coverage statistics...");
    stat.calcCoverageRelatedStatistics(*m_covStats);

    emit updateStatusLabel("Calculating results statistics...");
    stat.calcCovResultsSummary(*m_resStats);

    emit updateStatusLabel("Calculating fail statistics...");
    stat.calcFailStatistics(*m_resStats);

    emit processFinished("Calculation of statistics has been finished.");
}
