#include "CIDManagerTableModel.h"

CIDManagerTableModel::CIDManagerTableModel(QObject *parent, IIDManager* idManager) : QAbstractTableModel(parent),
    m_data(idManager), m_ids(idManager->getIDList()), m_names(idManager->getValueList())
{
}

int CIDManagerTableModel::rowCount(const QModelIndex &parent) const
{
    return m_data->size();
}

int CIDManagerTableModel::columnCount(const QModelIndex &parent) const
{
    return MAX_COLS;
}

QVariant CIDManagerTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= m_data->size() || index.row() < 0)
        return QVariant();

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        switch(index.column())
        {
        case ID:
            return QVariant::fromValue(m_ids[index.row()]);
        case NAME:
            return QString(m_names[index.row()].c_str());
        }
    }
    return QVariant();
}

QVariant CIDManagerTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Id");
        case 1:
            return tr(" Name");
        }
    }
    return QVariant();
}
