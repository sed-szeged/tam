#ifndef CCOVEREDLISTDIALOG_H
#define CCOVEREDLISTDIALOG_H

#include <QDialog>
#include <QSortFilterProxyModel>
#include "data/CSelectionData.h"

using namespace soda;

namespace Ui {
class CCoveredListDialog;
}

class CCoveredListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CCoveredListDialog(QWidget *parent, CSelectionData *data, IndexType id, bool isTest);
    ~CCoveredListDialog();

private slots:
    void on_buttonClose_clicked();

    void on_lineEditFilter_textEdited(const QString &text);

private:
    Ui::CCoveredListDialog *ui;
    QSortFilterProxyModel *m_filter;
};

#endif // CCOVEREDLISTDIALOG_H
