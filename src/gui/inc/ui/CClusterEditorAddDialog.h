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

/**
 * @brief The CClusterEditorAddDialog class implements a dialog which helps adding new test cases and code elements to our group.
 */
class CClusterEditorAddDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Creates a new object and initialises the dialog.
     * @param parent Parent object.
     * @param manager Holds the names of the test cases or code elements.
     * @param addedItems Model which holds already added items.
     * @param isCodeElements Whether code elements or test cases.
     * @param isFailedCodeElements True if the dialog is used at fault localization techniques page.
     */
    explicit CClusterEditorAddDialog(QWidget *parent, CIDManager *manager, QStandardItemModel *addedItems, bool isCodeElements = false, bool isFailedCodeElements = false);
    ~CClusterEditorAddDialog();

private slots:

    /**
     * @brief Slot which is called upon keypress on the connected lineEdit.
     * @param text Content of the lineEdit.
     */
    void on_lineEditFilter_textEdited(const QString &text);

    /**
     * @brief Slot which is called on OK button click.
     */
    void on_buttonBox_accepted();

private:
    Ui::CClusterEditorAddDialog *ui;

    /**
     * @brief Helps in filtering the content of the list.
     */
    QSortFilterProxyModel *m_filter;

    /**
     * @brief True on changing the code element part of the selected group.
     */
    bool m_isCodeElements;

    /**
     * @brief True if this dialog is used for adding failed code elements at fault localization technique calculation.
     */
    bool m_isFailedCodeElements;
};

#endif // CCLUSTEREDITORADDDIALOG_H
