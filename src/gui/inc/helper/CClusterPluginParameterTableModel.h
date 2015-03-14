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

/**
 * @brief The CClusterPluginParameterTableModel class manages the required parameters for cluster plugins.
 */
class CClusterPluginParameterTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    /**
     * @brief Creates a new object.
     * @param parent
     * @param clusterParameters Required parameters for the selected cluster plugin.
     */
    explicit CClusterPluginParameterTableModel(QObject *parent, std::map<std::string, std::string> clusterParameters);
    enum {NAME = 0, TYPE, VALUE, MAX_COLS};

    /**
     * @brief Returns the number of rows.
     * @param parent
     * @return  Number of rows.
     */
    int rowCount(const QModelIndex &parent) const;

    /**
     * @brief Returns the number of columns.
     * @param parent
     * @return Number of columns.
     */
    int columnCount(const QModelIndex &parent) const;

    /**
     * @brief Returns the stored data in the given index.
     * @param index
     * @param role
     * @return Data at the given index.
     */
    QVariant data(const QModelIndex &index, int role) const;

    /**
     * @brief Returns the header label for the requested section.
     * @param section
     * @param orientation
     * @param role
     * @return Header label.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    /**
     * @brief Updates the data which stored in the model when changed at the given position.
     * @param index Position of the changed value.
     * @param value New value.
     * @param role
     * @return True if it successfully updated.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /**
     * @brief Returns the flags of the field at index position.
     * @param index
     * @return Flags of the specified field by index.
     */
    Qt::ItemFlags flags(const QModelIndex &index) const;

    /**
     * @brief Converts the table content to json.
     * @param doc Container of the parameters.
     */
    void getValues(rapidjson::Document &doc);

private:
    /**
     * @brief Name of the parameters.
     */
    std::map<std::string, std::string> m_clusterParameters;

    /**
     * @brief Value of the parameters.
     */
    std::map<std::string, std::string> m_clusterValues;
};

#endif // CCLUSTERPARAMETERTABLEMODEL_H
