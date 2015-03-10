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

#ifndef CCLUSTERPARAMETERTABLEMODEL_H
#define CCLUSTERPARAMETERTABLEMODEL_H

#include <QAbstractTableModel>
#include <map>
#include <rapidjson/document.h>

class CClusterPluginParameterTableModel : public QAbstractTableModel
{
    Q_OBJECT
    std::map<std::string, std::string> m_clusterParameters;
    std::map<std::string, std::string> m_clusterValues;

public:
    explicit CClusterPluginParameterTableModel(QObject *parent, std::map<std::string, std::string> clusterParameters);
    enum {NAME = 0, TYPE, VALUE, MAX_COLS};

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    void toJson(rapidjson::Document &doc);
    void getValues(rapidjson::Document &doc);
};

#endif // CCLUSTERPARAMETERTABLEMODEL_H
