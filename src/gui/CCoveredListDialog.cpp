#include <QStringListModel>
#include "CCoveredListDialog.h"
#include "ui_CCoveredListDialog.h"

CCoveredListDialog::CCoveredListDialog(QWidget *parent, CSelectionData *data, IndexType id, bool isTest) :
    QDialog(parent),
    ui(new Ui::CCoveredListDialog)
{
    ui->setupUi(this);

    QStringList coveredElements;
    if (isTest) {
        IBitList &row = data->getCoverage()->getBitMatrix().getRow(id);
        for (int i = 0; i < data->getCoverage()->getCodeElements().size(); ++i) {
            if (!row[i])
                continue;

            coveredElements << data->getCoverage()->getCodeElements().getValue(i).c_str();
        }
    } else {
        for (int i = 0; i < data->getCoverage()->getTestcases().size(); ++i) {
            if (!data->getCoverage()->getBitMatrix().get(i, id))
                continue;

            coveredElements << data->getCoverage()->getTestcases().getValue(i).c_str();
        }
    }

    QStringListModel *model = new QStringListModel(ui->listViewElements);
    model->setStringList(coveredElements);
    m_filter = new QSortFilterProxyModel(ui->listViewElements);
    m_filter->setSourceModel(model);
    ui->listViewElements->setModel(m_filter);
    ui->listViewElements->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

CCoveredListDialog::~CCoveredListDialog()
{
    delete ui;
}

void CCoveredListDialog::on_buttonClose_clicked()
{
    close();
}

void CCoveredListDialog::on_lineEditFilter_textEdited(const QString &text)
{
    m_filter->setFilterFixedString(text);
}
