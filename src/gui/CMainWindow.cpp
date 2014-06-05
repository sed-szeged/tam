#include "CMainWindow.h"
#include "CLoadTestSuiteDialog.h"
#include "ui_CMainWindow.h"
#include <iostream>

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CMainWindow),
    selectionData(new CSelectionData())
{
    ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
    delete selectionData;
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

void CMainWindow::loadSelectionData(QString coverage, QString results, QString changes)
{
    if (!coverage.isEmpty()) {
        ui->textBrowserOutput->append("Loading coverage data from path: " + coverage + " ...");
        selectionData->loadCoverage(coverage.toStdString());
    }

    if (!results.isEmpty()) {
        ui->textBrowserOutput->append("Loading coverage data from path: " + coverage + " ...");
        selectionData->loadResults(results.toStdString());
    }
    if (!results.isEmpty()) {
        ui->textBrowserOutput->append("Loading coverage data from path: " + coverage + " ...");
        selectionData->loadChangeset(changes.toStdString());
    }
    ui->textBrowserOutput->append("Test-suite loading finished.");
}

void CMainWindow::on_actionDumpCoverage_triggered()
{
    const IIDManager& idm = selectionData->getCoverage()->getCodeElements();
    for (IndexType idx = 0; idx < idm.size(); ++idx) {
        ui->textBrowserOutput->append(QString(idm[idx].c_str()));
    }
}
