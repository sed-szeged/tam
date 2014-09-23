#ifndef CTABLEVIEWBUTTONDELEGATE_H
#define CTABLEVIEWBUTTONDELEGATE_H

#include <QItemDelegate>

class CTableViewButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    CTableViewButtonDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
};

#endif // CTABLEVIEWBUTTONDELEGATE_H
