#include <QStringListModel>
#include "CClusterEditorDialog.h"
#include "CClusterEditorAddDialog.h"
#include "ui_CClusterEditorAddDialog.h"

#include <iostream>

CClusterEditorAddDialog::CClusterEditorAddDialog(QWidget *parent, CIDManager *manager, QStandardItemModel *addedItems, bool isCodeElements) :
    QDialog(parent),
    ui(new Ui::CClusterEditorAddDialog),
    m_isCodeElements(isCodeElements)
{
    ui->setupUi(this);

    QStringListModel *model = new QStringListModel(ui->listViewElements);
    QStringList list;
    StringVector val = manager->getValueList();
    for (StringVector::const_iterator it = val.begin(); it != val.end(); ++it) {
        if (addedItems->findItems(tr((*it).c_str())).size())
            continue;

        list << (*it).c_str();
    }
    model->setStringList(list);
    m_filter = new QSortFilterProxyModel(ui->listViewElements);
    m_filter->setSourceModel(model);
    ui->listViewElements->setModel(m_filter);
    ui->listViewElements->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

CClusterEditorAddDialog::~CClusterEditorAddDialog()
{
    delete ui;
}

void CClusterEditorAddDialog::on_lineEditFilter_textEdited(const QString &text)
{
    m_filter->setFilterFixedString(text);
}

void CClusterEditorAddDialog::on_buttonBox_accepted()
{
    QModelIndexList indexes = ui->listViewElements->selectionModel()->selectedIndexes();
    if (indexes.empty())
        return;
    QStringList list;
    for (QModelIndexList::iterator it = indexes.begin(); it != indexes.end(); ++it) {
        list.append((*it).data().toString());
    }

    if (list.empty())
        return;

    if (m_isCodeElements) {
        qobject_cast<CClusterEditorDialog*>(parent())->addCodeElements(list);
    } else {
        qobject_cast<CClusterEditorDialog*>(parent())->addTestCases(list);
    }
}
