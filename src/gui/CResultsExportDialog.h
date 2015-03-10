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

#ifndef CRESULTSEXPORTDIALOG_H
#define CRESULTSEXPORTDIALOG_H

#include <QDialog>
#include "lib/CWorkspace.h"

namespace Ui {
class CResultsExportDialog;
}

class CResultsExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CResultsExportDialog(QWidget *parent, QStringList measurements, CWorkspace *workspace, bool isMetrics);
    ~CResultsExportDialog();

private slots:
    void on_buttonBox_accepted();

signals:
    void updateStatusLabel(QString label);

private:

    void exportMetricsResults(QString dir);
    void exportScoreResults(QString dir);

    Ui::CResultsExportDialog *ui;

    CWorkspace *m_workspace;
    bool m_isMetrics;
};

#endif // CRESULTSEXPORTDIALOG_H
