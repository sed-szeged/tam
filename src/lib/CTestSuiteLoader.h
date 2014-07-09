#ifndef CTESTSUITELOADER_H
#define CTESTSUITELOADER_H

#include <QThread>
#include "data/CSelectionData.h"

using namespace soda;

class CWorkspace;

class CTestSuiteLoader : public QThread
{
    Q_OBJECT

public:
    CTestSuiteLoader(QObject *parent = 0);
    ~CTestSuiteLoader();

    void load(CWorkspace *workspace);

protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:
    String m_covPath;
    String m_resPath;
    String m_changesPath;
    bool m_isStatisticsCalculated;
    CSelectionData *m_testSuite;
};

#endif // CTESTSUITELOADER_H
