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
