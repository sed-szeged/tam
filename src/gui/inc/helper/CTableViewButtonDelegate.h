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

#ifndef CTABLEVIEWBUTTONDELEGATE_H
#define CTABLEVIEWBUTTONDELEGATE_H

#include <QItemDelegate>

/**
 * @brief The CTableViewButtonDelegate class implements a special button item which contains a textbox
 * and a button which opens a file browser. Mostly overrides the functions of the base class.
 */
class CTableViewButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:

    /**
     * @brief Creates a new object.
     * @param parent
     */
    CTableViewButtonDelegate(QObject *parent = 0);

    /**
     * @brief Reimplemented from QItemDelegate::paint().
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /**
     * @brief Reimplemented from QItemDeleage::editorEvent().
     * @param event
     * @param model
     * @param option
     * @param index
     * @return
     */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // CTABLEVIEWBUTTONDELEGATE_H
