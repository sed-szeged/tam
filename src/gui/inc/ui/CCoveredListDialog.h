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
