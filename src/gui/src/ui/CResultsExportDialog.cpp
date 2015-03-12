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

#include "ui/CResultsExportDialog.h"
#include "ui_CResultsExportDialog.h"

#include "rapidjson/document.h"

#include <QStandardItemModel>
#include <QFileDialog>

#include <iostream>

CResultsExportDialog::CResultsExportDialog(QWidget *parent, QStringList measurements, CWorkspace *workspace, bool isMetrics) :
    QDialog(parent),
    ui(new Ui::CResultsExportDialog),
    m_workspace(workspace),
    m_isMetrics(isMetrics)
{
    ui->setupUi(this);
    QStandardItemModel *model = new QStandardItemModel(this);
    for (int i = 0; i < measurements.size(); ++i) {
        QStandardItem *item = new QStandardItem(tr(measurements[i].toStdString().c_str()));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);
        model->appendRow(item);
    }
    ui->listViewMeasurements->setModel(model);
    connect(this, SIGNAL(updateStatusLabel(QString)), qobject_cast<CMainWindow*>(parent), SLOT(tmpStatusUpdate(QString)));
}

CResultsExportDialog::~CResultsExportDialog()
{
    delete ui;
}

void CResultsExportDialog::on_buttonBox_accepted()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select directory"));
    if (dir.isEmpty())
        return;

    if (m_isMetrics)
        exportMetricsResults(dir);
    else
        exportScoreResults(dir);
}

void CResultsExportDialog::exportMetricsResults(QString dir)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->listViewMeasurements->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() != Qt::Checked)
            continue;

        String rev = QString::number((*m_workspace->getMeasurement(METRICS, model->item(i)->text().toStdString()))["revision"].GetInt()).toStdString();
        std::ofstream of((dir.toStdString() + String("/") + model->item(i)->text().toStdString() + "-metrics-results-" + rev + ".csv").c_str());
        rapidjson::Document *result = m_workspace->getMeasurementResults(METRICS, model->item(i)->text().toStdString());
        bool first = true;
        for (rapidjson::Value::MemberIterator groupIt = result->MemberBegin(); groupIt != result->MemberEnd(); ++groupIt) {
            if (first)
                of << "group;";

            String value = groupIt->name.GetString() + (String)";";
            for (rapidjson::Value::MemberIterator metricIt = groupIt->value.MemberBegin(); metricIt != groupIt->value.MemberEnd(); ++metricIt) {
                if (first)
                    of << metricIt->name.GetString() << ";";
                value += QString::number(metricIt->value.GetDouble()).toStdString() + ";";
            }
            if (first) {
                of << std::endl;
                first = false;
            }
            of << value << std::endl;
        }
        of.close();
    }
    emit updateStatusLabel("Metrics results exported.");
}

void CResultsExportDialog::exportScoreResults(QString dir)
{
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->listViewMeasurements->model());
    for (int i = 0; i < model->rowCount(); ++i) {
        if (model->item(i)->checkState() != Qt::Checked)
            continue;

        String rev = QString::number((*m_workspace->getMeasurement(SCORE, model->item(i)->text().toStdString()))["revision"].GetInt()).toStdString();
        rapidjson::Document *result = m_workspace->getMeasurementResults(SCORE, model->item(i)->text().toStdString());

        for (rapidjson::Value::MemberIterator groupIt = result->MemberBegin(); groupIt != result->MemberEnd(); ++groupIt) {
            for (rapidjson::Value::MemberIterator flTechIt = groupIt->value.MemberBegin(); flTechIt != groupIt->value.MemberEnd(); ++flTechIt) {
                std::ofstream of((dir.toStdString() + String("/") + model->item(i)->text().toStdString() + String("-") + groupIt->name.GetString() + String("-") + flTechIt->name.GetString() + "-results-" + rev + ".csv").c_str());
                of << "code element;suspiciousness value;" << std::endl;
                for (rapidjson::Value::MemberIterator flValIt = flTechIt->value.MemberBegin(); flValIt != flTechIt->value.MemberEnd(); ++flValIt) {
                    String codeElement;
                    if (m_workspace->isTestSuiteAvailable())
                        codeElement = m_workspace->getTestSuite()->getCodeElements()->getValue(QString(flValIt->name.GetString()).toULongLong());
                    else
                        codeElement = flValIt->name.GetString();

                    of << codeElement << ";" << flValIt->value.GetDouble() << ";" << std::endl;
                }
                of.close();
            }
        }
    }
    emit updateStatusLabel("Fault-localization results exported.");
}
