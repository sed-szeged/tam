#ifndef CFLSCORE_H
#define CFLSCORE_H

#include <QThread>
#include "data/CSelectionData.h"
#include "rapidjson/document.h"
#include "engine/CKernel.h"

using namespace soda;

class CMainWindow;

class CFLScore : public QThread
{
    Q_OBJECT
public:
    CFLScore(QObject *parent = 0);
    void calculateScore(IndexType revision, StringVector faultLocalizationTechniques, StringVector selectedClusters, CMainWindow *mainWindow);
protected:
    void run();

signals:
    void updateStatusLabel(QString label);
    void processFinished(QString msg);

private:

    IndexType m_revision;
    IntVector m_failedCodeElements;
    StringVector m_faultLocalizationTechniques;
    StringVector m_selectedClusters;
    CSelectionData *m_testSuite;
    rapidjson::Document *m_results;
    CKernel *m_kernel;
    ClusterMap *m_clusters;
};

#endif // CFLSCORE_H
