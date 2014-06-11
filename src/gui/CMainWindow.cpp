#include "CMainWindow.h"
#include "CLoadTestSuiteDialog.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"
#include <iostream>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    m_workspace(new CWorkspace("Default workspace", this))
{
    ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
    delete ui;
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

void CMainWindow::on_actionNewWorkspace_triggered()
{

}

void CMainWindow::on_actionDumpCoverage_triggered()
{
    StringVector vec = m_workspace->getTestSuite()->getCoverage()->getCodeElements().getValueList();
    for (StringVector::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        ui->textBrowserOutput->append((*it).c_str());
    }
}
