#include <QStringListModel>
#include "CClusterEditorDialog.h"
#include "CClusterEditorAddDialog.h"
#include "CMainWindow.h"
#include "ui_CClusterEditorAddDialog.h"
#include "ui_CMainWindow.h"
#include "lib/CWorkspace.h"

#include <iostream>

CClusterEditorAddDialog::CClusterEditorAddDialog(QWidget *parent, CIDManager *manager, QStandardItemModel *addedItems, bool isCodeElements, bool isFailedCodeElements) :
    QDialog(parent),
    ui(new Ui::CClusterEditorAddDialog),
    m_isCodeElements(isCodeElements),
    m_isFailedCodeElements(isFailedCodeElements)
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
    m_filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
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
        if (m_isFailedCodeElements) {
            QStandardItemModel *failedCodeElementModel = qobject_cast<QStandardItemModel*>(qobject_cast<CMainWindow*>(parent())->getUi()->listViewFailedCodeElements->model());
            for (QStringList::iterator ceIt = list.begin(); ceIt != list.end(); ++ceIt) {
                QStandardItem *item = new QStandardItem(*ceIt);
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
                failedCodeElementModel->appendRow(item);

                rapidjson::Value &settings = (*qobject_cast<CMainWindow*>(parent())->getWorkspace()->getMeasurement("score", qobject_cast<CMainWindow*>(parent())->getScoreMeasurement()));
                rapidjson::Document::AllocatorType &allocator = qobject_cast<CMainWindow*>(parent())->getWorkspace()->getMeasurement("score", qobject_cast<CMainWindow*>(parent())->getScoreMeasurement())->GetAllocator();
                if (!settings.HasMember("failed-code-elements")) {
                    settings.AddMember("failed-code-elements", rapidjson::Value(rapidjson::kArrayType).Move(), allocator);
                }
                for (rapidjson::Value::ValueIterator valIt = settings["failed-code-elements"].Begin(); valIt != settings["failed-code-elements"].End(); ++valIt) {
                    if (valIt->GetString() != (*ceIt).toStdString())
                        continue;
                    return;
                }
                rapidjson::Value val((*ceIt).toStdString().c_str(), allocator);
                settings["failed-code-elements"].PushBack(val, allocator);
            }
        } else
            qobject_cast<CClusterEditorDialog*>(parent())->addCodeElements(list);
    } else {
        qobject_cast<CClusterEditorDialog*>(parent())->addTestCases(list);
    }
}
