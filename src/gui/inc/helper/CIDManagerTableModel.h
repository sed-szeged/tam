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

#ifndef CTESTSTABLEMODEL_H
#define CTESTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "interface/IIDManager.h"

using namespace soda;

/**
 * @brief The CIDManagerTableModel class implements a model which lists code elements and test cases.
 */
class CIDManagerTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    /**
     * @brief Creates a new object.
     * @param parent
     * @param idManager Container of code elements or test cases.
     */
    explicit CIDManagerTableModel(QObject *parent, IIDManager *idManager);
    enum {ID = 0, NAME, MAX_COLS};

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

private:

    /**
     * @brief Instance of IDManager.
     */
    IIDManager *m_data;

    /**
     * @brief List of names for speeding up indexing.
     */
    StringVector m_names;

    /**
     * @brief List of ids for speeding up indexing.
     */
    IntVector m_ids;
};

#endif // CTESTSTABLEMODEL_H
