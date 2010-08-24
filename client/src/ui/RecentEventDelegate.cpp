#include "RecentEventDelegate.h"
#include <QPainter>

RecentEventDelegate::RecentEventDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

QSize RecentEventDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect textRect = option.fontMetrics.boundingRect(displayText(index));
    return textRect.size();
}

void RecentEventDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    p->drawText(option.rect, displayText(index));
}

QString RecentEventDelegate::displayText(const QModelIndex &index) const
{
    int row = index.row();
    QModelIndex parent = index.parent();
    const QAbstractItemModel *model = index.model();

    QString text = tr("%1 on %2 (%3) %4");
    text = text.arg(model->index(row, 2, parent).data().toString(),
                    model->index(row, 1, parent).data().toString(),
                    model->index(row, 0, parent).data().toString(),
                    model->index(row, 3, parent).data().toString());
    return text;
}
