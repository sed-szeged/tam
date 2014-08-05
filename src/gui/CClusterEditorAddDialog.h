#ifndef CCLUSTEREDITORADDDIALOG_H
#define CCLUSTEREDITORADDDIALOG_H

#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QDialog>
#include "data/CIDManager.h"

using namespace soda;

namespace Ui {
class CClusterEditorAddDialog;
}

class CClusterEditorAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CClusterEditorAddDialog(QWidget *parent, CIDManager *manager, QStandardItemModel *addedItems, bool isCodeElements = false, bool isFailedCodeElements = false);
    ~CClusterEditorAddDialog();

private slots:
    void on_lineEditFilter_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::CClusterEditorAddDialog *ui;
    QSortFilterProxyModel *m_filter;
    bool m_isCodeElements;
    bool m_isFailedCodeElements;
};

#endif // CCLUSTEREDITORADDDIALOG_H
