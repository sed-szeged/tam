#include "CLoadTestSuiteDialog.h"
#include "CMainWindow.h"
#include "ui_CLoadTestSuiteDialog.h"
#include "lib/CWorkspace.h"
#include <QFileDialog>
#include <QMessageBox>

CLoadTestSuiteDialog::CLoadTestSuiteDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CLoadTestSuiteDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->installEventFilter(this);
    CWorkspace *workspace = qobject_cast<CMainWindow*>(parent)->getWorkspace();
    ui->textEditCoverage->setText(workspace->getCoveragePath());
    ui->textEditResults->setText(workspace->getResultsPath());
    ui->textEditChangeset->setText(workspace->getChangesetPath());
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

bool CLoadTestSuiteDialog::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->buttonBox->button(QDialogButtonBox::Ok) && event->type() == QEvent::MouseButtonRelease) {
        if (ui->textEditCoverage->toPlainText().isEmpty() || ui->textEditResults->toPlainText().isEmpty()) {
            QMessageBox::critical(this, "Error", "Mising results or coverage file");
            return true;
        }
    }
    return false;
}

void CLoadTestSuiteDialog::on_buttonBox_accepted()
{
    qobject_cast<CMainWindow*>(parent())->getWorkspace()->setBinaryPaths(ui->textEditCoverage->toPlainText(), ui->textEditResults->toPlainText(), ui->textEditChangeset->toPlainText());
    qobject_cast<CMainWindow*>(parent())->getWorkspace()->loadTestSuite();
}
