#ifndef CRESULTSEXPORTDIALOG_H
#define CRESULTSEXPORTDIALOG_H

#include <QDialog>
#include "lib/CWorkspace.h"

namespace Ui {
class CResultsExportDialog;
}

class CResultsExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CResultsExportDialog(QWidget *parent, QStringList measurements, CWorkspace *workspace, bool isMetrics);
    ~CResultsExportDialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void updateStatusLabel(QString label);

private:

    void exportMetricsResults(QString dir);
    void exportScoreResults(QString dir);

    Ui::CResultsExportDialog *ui;

    CWorkspace *m_workspace;
    bool m_isMetrics;
};

#endif // CRESULTSEXPORTDIALOG_H
