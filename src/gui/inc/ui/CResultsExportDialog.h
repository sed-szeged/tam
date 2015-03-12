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
#include "wrapper/CWorkspace.h"

namespace Ui {
class CResultsExportDialog;
}

/**
 * @brief The CResultsExportDialog class implemnets the exporting of metrics or fault localization techniques results.
 */
class CResultsExportDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief Creates a new object and initialises the dialog.
     * @param parent Parent object.
     * @param measurements Available measurements.
     * @param workspace Current workspace.
     * @param isMetrics Whether we are exporting metrics or fault localization techniques results.
     */
    explicit CResultsExportDialog(QWidget *parent, QStringList measurements, CWorkspace *workspace, bool isMetrics);
    ~CResultsExportDialog();

private slots:

    /**
     * @brief Slot which is called upon clicking on OK button.
     */
    void on_buttonBox_accepted();

signals:

    /**
     * @brief Signal which updates the status label on the main window.
     * @param label Label to be set on the status label.
     */
    void updateStatusLabel(QString label);

private:

    /**
     * @brief Exports the selected metric results.
     * @param dir Output directory.
     */
    void exportMetricsResults(QString dir);

    /**
     * @brief Exports the selected fault localization techniques results.
     * @param dir Output directory.
     */
    void exportScoreResults(QString dir);

    Ui::CResultsExportDialog *ui;

    /**
     * @brief Current workspace.
     */
    CWorkspace *m_workspace;

    /**
     * @brief Whether we are exporting metrics or fault localization techniques results.
     */
    bool m_isMetrics;
};

#endif // CRESULTSEXPORTDIALOG_H
