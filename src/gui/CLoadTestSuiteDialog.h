#ifndef CLOADTESTSUITEDIALOG_H
#define CLOADTESTSUITEDIALOG_H

#include <QDialog>

namespace Ui {
class CLoadTestSuiteDialog;
}

class CLoadTestSuiteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CLoadTestSuiteDialog(QWidget *parent = 0);
    ~CLoadTestSuiteDialog();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void on_pushButtonCoverage_clicked();

    void on_pushButtonResults_clicked();

    void on_pushButtonChangeset_clicked();

    void on_buttonBox_accepted();

private:
    Ui::CLoadTestSuiteDialog *ui;
};

#endif // CLOADTESTSUITEDIALOG_H
