#ifndef CSTATISTICSTHREAD_H
#define CSTATISTICSTHREAD_H

#include <QThread>
#include "data/CSelectionData.h"
#include "rapidjson/document.h"

using namespace soda;

class CWorkspace;

class CStatisticsThread : public QThread
{
    Q_OBJECT
public:
    CStatisticsThread(QObject *parent = 0);
    ~CStatisticsThread();

    void calcStatistics(CWorkspace *workspace);

protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:
    CSelectionData *m_testSuite;
    rapidjson::Document *m_covStats;
    rapidjson::Document *m_resStats;
};

#endif // CSTATISTICSTHREAD_H
