#include "gui/CMainWindow.h"
#include "ui_CMainWindow.h"
#include "CClusterList.h"
#include "CWorkspace.h"

CClusterList::CClusterList(CMainWindow *mainWindow) :
    m_mainWindow(mainWindow),
    m_clusters(new std::map<std::string, CClusterDefinition>())
{
}

CClusterList::~CClusterList()
{
    delete m_clusters;
}

void CClusterList::createClusters()
{
    ITestSuiteClusterPlugin *clusterAlgorithm = m_mainWindow->getKernel()->getTestSuiteClusterPluginManager().getPlugin(m_mainWindow->getUi()->comboBoxClusterPlugins->currentText().toStdString());
    clusterAlgorithm->init(*m_mainWindow->getWorkspace()->getResultsByName(WS));
    clusterAlgorithm->execute(*m_mainWindow->getWorkspace()->getTestSuite(), *m_clusters);
}

void CClusterList::toJson(rapidjson::Document &doc)
{
    for (std::map<std::string, CClusterDefinition>::iterator it = m_clusters->begin(); it != m_clusters->end(); ++it) {
        rapidjson::Value cluster;
        cluster.SetObject();

        rapidjson::Value tcid;
        tcid.SetArray();
        for (IntVector::const_iterator itr = it->second.getTestCases().begin(); itr != it->second.getTestCases().end(); ++itr) {
            tcid.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("tests", tcid, doc.GetAllocator());

        rapidjson::Value ceid;
        ceid.SetArray();
        for (IntVector::const_iterator itr = it->second.getCodeElements().begin(); itr != it->second.getCodeElements().end(); ++itr) {
            ceid.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("code-elements", ceid, doc.GetAllocator());

        doc.AddMember(it->first.c_str(), cluster, doc.GetAllocator());
    }
}

void CClusterList::fromJson(rapidjson::Document &doc)
{
    for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
        CClusterDefinition def;
        for (rapidjson::Value::ConstValueIterator it = itr->value["tests"].Begin(); it != itr->value["tests"].End(); ++it)
            def.addTestCase(it->GetUint64());
        for (rapidjson::Value::ConstValueIterator it = itr->value["code-elements"].Begin(); it != itr->value["code-elements"].End(); ++it)
            def.addCodeElement(it->GetUint64());

        (*m_clusters)[String(itr->name.GetString())] = def;
    }
}
