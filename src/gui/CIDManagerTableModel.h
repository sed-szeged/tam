#ifndef CTESTSTABLEMODEL_H
#define CTESTSTABLEMODEL_H

#include <QAbstractTableModel>
#include "interface/IIDManager.h"

using namespace soda;

class CIDManagerTableModel : public QAbstractTableModel
{
    Q_OBJECT
    IIDManager *m_data;
    StringVector m_names;
    IntVector m_ids;
public:
    explicit CIDManagerTableModel(QObject *parent, IIDManager *idManager);
    enum {ID = 0, NAME, MAX_COLS};

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
};

#endif // CTESTSTABLEMODEL_H
