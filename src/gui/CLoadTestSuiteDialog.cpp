#include "CLoadTestSuiteDialog.h"
#include "CMainWindow.h"
#include "ui_CLoadTestSuiteDialog.h"
#include <QFileDialog>

CLoadTestSuiteDialog::CLoadTestSuiteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLoadTestSuiteDialog)
{
    ui->setupUi(this);
}

CLoadTestSuiteDialog::~CLoadTestSuiteDialog()
{
    delete ui;
}

void CLoadTestSuiteDialog::on_pushButtonCoverage_clicked()
{
    ui->textEditCoverage->setText(QFileDialog::getOpenFileName(this));
}

void CLoadTestSuiteDialog::on_pushButtonResults_clicked()
{
    ui->textEditResults->setText(QFileDialog::getOpenFileName(this));
}

void CLoadTestSuiteDialog::on_pushButtonChangeset_clicked()
{
    ui->textEditResults->setText(QFileDialog::getOpenFileName(this));
}

void CLoadTestSuiteDialog::on_buttonBox_accepted()
{
    qobject_cast<CMainWindow*>(parent())->loadSelectionData(ui->textEditCoverage->toPlainText(), ui->textEditResults->toPlainText(), ui->textEditChangeset->toPlainText());
}
