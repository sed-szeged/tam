#include "gui/CMainWindow.h"
#include "ui_CMainWindow.h"
#include "CClusterList.h"
#include "CWorkspace.h"

CClusterList::CClusterList(CMainWindow *mainWindow) :
    m_mainWindow(mainWindow),
    m_clusters(new ClusterMap())
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
    for (ClusterMap::iterator it = m_clusters->begin(); it != m_clusters->end(); ++it) {
        rapidjson::Value cluster;
        cluster.SetObject();

        rapidjson::Value tcids;
        tcids.SetArray();
        for (IntVector::const_iterator itr = it->second.getTestCases().begin(); itr != it->second.getTestCases().end(); ++itr) {
            tcids.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("tests", tcids, doc.GetAllocator());

        rapidjson::Value ceids;
        ceids.SetArray();
        for (IntVector::const_iterator itr = it->second.getCodeElements().begin(); itr != it->second.getCodeElements().end(); ++itr) {
            ceids.PushBack(*itr, doc.GetAllocator());
        }
        cluster.AddMember("code-elements", ceids, doc.GetAllocator());

        rapidjson::Value key;
        key.SetString(it->first.c_str(), doc.GetAllocator());
        doc.AddMember(key, cluster, doc.GetAllocator());
    }
}

void CClusterList::fromJson(rapidjson::Document &doc)
{
    for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin(); itr != doc.MemberEnd(); ++itr) {
        CClusterDefinition def;
        if (!itr->value.IsObject())
            continue;

        for (rapidjson::Value::ConstValueIterator it = itr->value["tests"].Begin(); it != itr->value["tests"].End(); ++it)
            def.addTestCase(it->GetUint64());
        for (rapidjson::Value::ConstValueIterator it = itr->value["code-elements"].Begin(); it != itr->value["code-elements"].End(); ++it)
            def.addCodeElement(it->GetUint64());

        (*m_clusters)[String(itr->name.GetString())] = def;
    }
}
