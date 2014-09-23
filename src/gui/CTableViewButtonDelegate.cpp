#include "CTableViewButtonDelegate.h"
#include "CClusterPluginParameterTableModel.h"
#include "engine/plugin/ITestSuiteClusterPlugin.h"

#include <QApplication>
#include <QEvent>
#include <QMouseEvent>
#include <QFileDialog>
#include <QToolButton>
#include <QLineEdit>
#include <QPainter>

CTableViewButtonDelegate::CTableViewButtonDelegate(QObject *parent) : QItemDelegate(parent)
{
}

void CTableViewButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.sibling(index.row(), 1).data().toString() != TYPE_PATH)
        return;

    QStyleOptionButton button;
    QRect r = option.rect;//getting the rect of the cell
    int x,y,w,h;
    x = r.left() + r.width() - 30;//the X coordinate
    y = r.top();//the Y coordinate
    w = 30;//button width
    h = r.height();//button height
    button.rect = QRect(x,y,w,h);
    button.text = "...";
    button.state = QStyle::State_Enabled;
    QString visibleText = "";
    for (int i = index.data().toString().length(); i >= 0; --i) {
        if (painter->fontMetrics().width(visibleText) < (r.width() - 30))
            visibleText = index.data().toString()[i] + visibleText;
        else
            break;
    }
    painter->drawText(r.left(), r.top(), r.width() - 30, r.height(), Qt::AlignLeft | Qt::AlignCenter, visibleText);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);
}

bool CTableViewButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *e = (QMouseEvent *)event;
        int clickX = e->x();
        int clickY = e->y();

        QRect r = option.rect;//getting the rect of the cell
        int x,y,w,h;
        x = r.left() + r.width() - 30;//the X coordinate
        y = r.top();//the Y coordinate
        w = 30;//button width
        h = 30;//button height

        if (clickX > x && clickX < x + w) {
            if (clickY > y && clickY < y + h) {
                QString fileName = QFileDialog::getOpenFileName(0, tr("Select input files"), QFileInfo(index.data().toString()).filePath());
                if (!fileName.isEmpty())
                    model->setData(index, fileName);
            }
        }
    }
}
