#ifndef CFLSCORE_H
#define CFLSCORE_H

#include "data/CSelectionData.h"
#include "rapidjson/document.h"
#include "engine/CKernel.h"

using namespace soda;

class CFLScore
{
public:
    CFLScore();
    void calculateScore();
protected:
    void run();
private:
    IndexType m_revision;
    StringVector m_failedCodeElements;
    StringVector m_faultLocalizationTechniques;
    CSelectionData *m_selectionData;
    rapidjson::Document *m_results;
    CKernel *m_kernel;
    std::map<std::string, CClusterDefinition> *m_clusters;
};

#endif // CFLSCORE_H
