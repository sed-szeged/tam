#include "CNewWorkspaceDialog.h"
#include "ui_CNewWorkspaceDialog.h"

CNewWorkspaceDialog::CNewWorkspaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CNewWorkspaceDialog)
{
    ui->setupUi(this);
}

CNewWorkspaceDialog::~CNewWorkspaceDialog()
{
    delete ui;
}
