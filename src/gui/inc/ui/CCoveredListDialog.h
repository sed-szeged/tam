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

/**
 * @brief The CCoveredListDialog class implements a dialog which lists
 * the covered test cases by a selected code element or the covered code elements by a selected test case.
 */
class CCoveredListDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Creates a new object and initialises the dialog.
     * @param parent Parent object.
     * @param data Current test-suite.
     * @param id Selected item id.
     * @param isTest True if the id is belongs to a test case.
     */
    explicit CCoveredListDialog(QWidget *parent, CSelectionData *data, IndexType id, bool isTest);
    ~CCoveredListDialog();

private slots:

    /**
     * @brief Slot which is called upon clicking the close button.
     */
    void on_buttonClose_clicked();

    /**
     * @brief Slot which is called upon keypress on the connected lineEdit.
     * @param text Content of the lineEdit.
     */
    void on_lineEditFilter_textEdited(const QString &text);

private:
    Ui::CCoveredListDialog *ui;

    /**
     * @brief Helper model for filtering the content of the list.
     */
    QSortFilterProxyModel *m_filter;
};

#endif // CCOVEREDLISTDIALOG_H
