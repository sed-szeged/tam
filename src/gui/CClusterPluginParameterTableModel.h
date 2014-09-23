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
