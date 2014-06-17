#include "CMainWindow.h"
#include "CLoadTestSuiteDialog.h"
#include "CShowStatisticsDialog.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"
#include <QFileDialog>
#include <iostream>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    m_workspace(NULL)
{
    ui->setupUi(this);
    m_workspace = new CWorkspace(this);
}

CMainWindow::~CMainWindow()
{
    delete ui;
    if (m_workspace)
        delete m_workspace;
}

void CMainWindow::on_actionExit_triggered()
{
    qApp->exit();
}

void CMainWindow::on_actionLoadTestSuite_triggered()
{
    CLoadTestSuiteDialog dia(this);
    dia.exec();
}

void CMainWindow::on_actionDumpCoverage_triggered()
{
    rapidjson::StringBuffer s;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
    for (int i = 0; i < NUM_OF_COLS; ++i) {
        s.Clear();
        m_workspace->getResultsByName(collections[i])->Accept(writer);
        ui->textBrowserOutput->append(s.GetString());
    }
}

void CMainWindow::on_actionShowStatistics_triggered()
{
    CShowStatisticsDialog dia(this);
    dia.exec();
}

void CMainWindow::on_actionNewWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/
    delete m_workspace;
    m_workspace = new CWorkspace(this);
}

void CMainWindow::on_actionLoadWorkspace_triggered()
{
    /*if (!m_workspace->isSaved())
        saveWorkspace();*/

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open workspace file"),
                                                    QString(),
                                                    tr("Unqlite database file (*.db);;All Files (*)"));
    if (!fileName.isEmpty()) {
        delete m_workspace;
        m_workspace = new CWorkspace(this);
        m_workspace->setFileName(fileName);
        m_workspace->load();
        ui->textBrowserOutput->append("Workspace loaded from file: " + m_workspace->getFileName());
    }
}

void CMainWindow::on_actionSaveWorkspace_triggered()
{
    if (saveWorkspace())
        ui->textBrowserOutput->append("Workspace saved successfully to " + m_workspace->getFileName());
    else
        ui->textBrowserOutput->append("Unable to save workspace to " + m_workspace->getFileName());
}

bool CMainWindow::saveWorkspace()
{
    if (m_workspace->getFileName().isEmpty())
        return saveWorkspaceAs();

    return m_workspace->save();
}

bool CMainWindow::saveWorkspaceAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("Unqlite database file (*.db);;All Files (*)"));
    if (fn.isEmpty())
        return false;
    if (!fn.endsWith(".db", Qt::CaseInsensitive))
        fn += ".db"; // default
    m_workspace->setFileName(fn);
    return saveWorkspace();
}
