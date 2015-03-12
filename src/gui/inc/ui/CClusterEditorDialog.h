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

#ifndef CCREATECLUSTERDIALOG_H
#define CCREATECLUSTERDIALOG_H

#include "data/CSelectionData.h"
#include <QStandardItemModel>
#include <QDialog>

using namespace soda;

namespace Ui {
class CClusterEditorDialog;
}

/**
 * @brief The CClusterEditorDialog class implements a dialog which helps in editing existing groups.
 */
class CClusterEditorDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Creates a new object and initialises the a dialog.
     * @param parent Parent object.
     * @param data Contains code element and test case data.
     * @param clusters Container of groups.
     * @param clusterName Name of the selected group.
     */
    explicit CClusterEditorDialog(QWidget *parent, CSelectionData *data, ClusterMap *clusters, QString clusterName = QString());
    ~CClusterEditorDialog();

    /**
     * @brief Adds the given test cases to the current group.
     * @param list List of test case names.
     */
    void addTestCases(QStringList list);

    /**
     * @brief Adds the given code elements to the current group.
     * @param list List of code elements.
     */
    void addCodeElements(QStringList list);

protected:

    /**
     * @brief Prevent calling event when the dialog is in invalid state.
     * @param object Object which takes the action.
     * @param event Event which is called.
     * @return True if the current event have to be interrupted.
     */
    bool eventFilter(QObject *object, QEvent *event);

private:

    /**
     * @brief Creates a new cluster.
     * @return Object which holds the a created cluster.
     */
    CClusterDefinition createCluster();

    /**
     * @brief Fills the code elements and test cases list views with the content of the specified group.
     * @param clusterName Specified group.
     */
    void fillListViews(QString clusterName);

private slots:

    /**
     * @brief Called upon clicking on the add test cases tool button.
     */
    void on_toolButtonAddTests_clicked();

    /**
     * @brief Called upon clickin on the add code elements tool button.
     */
    void on_toolButtonAddCEs_clicked();

    /**
     * @brief Called upon clicking on the remove test cases tool button.
     */
    void on_toolButtonRemoveTests_clicked();

    /**
     * @brief Called upon clicking on the remove code elements tool button.
     */
    void on_toolButtonRemoveCEs_clicked();

    /**
     * @brief Called upon clicking on the OK button.
     */
    void on_buttonBox_accepted();

    /**
     * @brief Slot which is called upon keypress on the connected lineEdit.
     * @param text Content of the lineEdit.
     */
    void on_lineEditFilterTests_textEdited(const QString &text);

    /**
     * @brief Slot which is called upon keypress on the connected lineEdit.
     * @param text Content of the lineEdit.
     */
    void on_lineEditFilterCE_textEdited(const QString &text);

private:
    Ui::CClusterEditorDialog *ui;

    /**
     * @brief Current test-suite.
     */
    CSelectionData *m_data;

    /**
     * @brief Container of groups.
     */
    ClusterMap *m_clusterMap;

    /**
     * @brief Contains the test cases which are in the group.
     */
    QStandardItemModel *m_testCases;

    /**
     * @brief Contains the code elements which are in the group.
     */
    QStandardItemModel *m_codeElements;


};

#endif // CCREATECLUSTERDIALOG_H
