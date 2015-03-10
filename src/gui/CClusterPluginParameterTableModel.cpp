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

#include "CClusterPluginParameterTableModel.h"
#include "engine/plugin/ITestSuiteClusterPlugin.h"

CClusterPluginParameterTableModel::CClusterPluginParameterTableModel(QObject *parent, std::map<std::string, std::string> clusterParameters) : QAbstractTableModel(parent),
    m_clusterParameters(clusterParameters)
{
}

int CClusterPluginParameterTableModel::rowCount(const QModelIndex &parent) const
{
    return m_clusterParameters.size();
}

int CClusterPluginParameterTableModel::columnCount(const QModelIndex &parent) const
{
    return MAX_COLS;
}

QVariant CClusterPluginParameterTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= m_clusterParameters.size() || index.row() < 0)
        return QVariant();

    std::map<std::string, std::string>::const_iterator it = m_clusterParameters.begin();
    std::size_t numOfElements = std::distance(m_clusterParameters.begin(), m_clusterParameters.end());
    std::advance(it, index.row() % numOfElements);

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column())
        {
        case NAME:
            return QString(it->first.c_str());
        case TYPE:
            return QString(it->second.c_str());
        case VALUE:
            if (m_clusterValues.find(it->first) != m_clusterValues.end())
                return QString(m_clusterValues.at(it->first).c_str());
            else
                return QVariant();
        }
    }
    return QVariant();
}

QVariant CClusterPluginParameterTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Parameter");
        case 1:
            return tr("Type");
        case 2:
            return tr("Value");
        }
    }
    return QVariant();
}

bool CClusterPluginParameterTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() != VALUE)
            return false;

        std::map<std::string, std::string>::const_iterator it = m_clusterParameters.begin();
        std::size_t numOfElements = std::distance(m_clusterParameters.begin(), m_clusterParameters.end());
        std::advance(it, index.row() % numOfElements);

        m_clusterValues[it->first] = value.toString().toStdString();
        emit(dataChanged(index, index));

        return true;
    }

    return false;
}

Qt::ItemFlags CClusterPluginParameterTableModel::flags(const QModelIndex &index) const
{
     if (!index.isValid())
         return Qt::ItemIsEnabled;

     Qt::ItemFlags flags = QAbstractTableModel::flags(index);
     if (index.column() == VALUE && index.sibling(index.row(), 1).data().toString() != TYPE_PATH)
         flags |= Qt::ItemIsEditable;

     return flags;
}

void CClusterPluginParameterTableModel::getValues(rapidjson::Document &doc)
{
    for (std::map<std::string, std::string>::const_iterator paramIt = m_clusterValues.begin(); paramIt != m_clusterValues.end(); ++paramIt) {
        if (m_clusterParameters[paramIt->first] == TYPE_PATH) {
            rapidjson::Value key(paramIt->first.c_str(), doc.GetAllocator());
            rapidjson::Value val(paramIt->second.c_str(), doc.GetAllocator());
            doc.AddMember(key, val, doc.GetAllocator());
        } else if (m_clusterParameters[paramIt->first] == TYPE_INTARRAY) {
            rapidjson::Value key(paramIt->first.c_str(), doc.GetAllocator());
            rapidjson::Value val(rapidjson::kArrayType);
            QStringList nums = QString(paramIt->second.c_str()).split(',');
            for (QStringList::iterator numIt = nums.begin(); numIt != nums.end(); ++numIt) {
                int num = (*numIt).toInt();
                if (num > 0)
                    val.PushBack(rapidjson::Value(num), doc.GetAllocator());
            }
            doc.AddMember(key, val, doc.GetAllocator());
        }
    }
}
