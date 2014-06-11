#ifndef CNEWWORKSPACEDIALOG_H
#define CNEWWORKSPACEDIALOG_H

#include <QDialog>

namespace Ui {
class CNewWorkspaceDialog;
}

class CNewWorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CNewWorkspaceDialog(QWidget *parent = 0);
    ~CNewWorkspaceDialog();

private:
    Ui::CNewWorkspaceDialog *ui;
};

#endif // CNEWWORKSPACEDIALOG_H
