/*
 * Copyright (C): 2014-2015 Department of Software Engineering, University of Szeged
 *
 * Authors: David Havas <havasd@inf.u-szeged.hu>
 *
 * This file is part of TAM.
 *
 *  TAM is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  TAM is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with TAM.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    m_filter->setFilterCaseSensitivity(Qt::CaseInsensitive);
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
