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

class CClusterEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CClusterEditorDialog(QWidget *parent, CSelectionData *data, ClusterMap *clusters, QString clusterName = QString());
    ~CClusterEditorDialog();

    void addTestCases(QStringList list);
    void addCodeElements(QStringList list);

protected:
    bool eventFilter(QObject *object, QEvent *event);

private:
    CClusterDefinition createCluster();
    void fillListViews(QString clusterName);

private slots:
    void on_toolButtonAddTests_clicked();
    void on_toolButtonAddCEs_clicked();

    void on_toolButtonRemoveTests_clicked();

    void on_toolButtonRemoveCEs_clicked();

    void on_buttonBox_accepted();

    void on_lineEditFilterTests_textEdited(const QString &text);

    void on_lineEditFilterCE_textEdited(const QString &text);

private:
    Ui::CClusterEditorDialog *ui;
    CSelectionData *m_data;
    ClusterMap *m_clusterMap;
    QStandardItemModel *m_testCases;
    QStandardItemModel *m_codeElements;


};

#endif // CCREATECLUSTERDIALOG_H
