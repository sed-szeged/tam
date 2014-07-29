#ifndef CCREATECLUSTERDIALOG_H
#define CCREATECLUSTERDIALOG_H

#include "data/CSelectionData.h"
#include <QStandardItemModel>
#include <QDialog>

using namespace soda;

namespace Ui {
class CClusterEditorDialog;
}

class CClusterEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CClusterEditorDialog(QWidget *parent, CSelectionData *data, ClusterMap *clusters);
    ~CClusterEditorDialog();

    void addTestCases(QStringList list);
    void addCodeElements(QStringList list);

private:
    CClusterDefinition createCluster();

private slots:
    void on_toolButtonAddTests_clicked();
    void on_toolButtonAddCEs_clicked();

    void on_toolButtonRemoveTests_clicked();

    void on_toolButtonRemoveCEs_clicked();

    void on_buttonBox_accepted();

private:
    Ui::CClusterEditorDialog *ui;
    CSelectionData *m_data;
    ClusterMap *m_clusterMap;
    QStandardItemModel *m_testCases;
    QStandardItemModel *m_codeElements;


};

#endif // CCREATECLUSTERDIALOG_H
