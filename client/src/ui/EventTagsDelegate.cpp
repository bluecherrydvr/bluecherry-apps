#include "EventTagsDelegate.h"
#include <QPainter>
#include <QFontMetrics>
#include <QCursor>
#include <QStyleOptionViewItemV4>

EventTagsDelegate::EventTagsDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void EventTagsDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QStyleOptionViewItemV4 opt4 = option;
    Q_ASSERT(opt4.widget);

    QRect delRect = QRect(option.rect.right()-16, option.rect.top(), 16, option.rect.height());

    painter->save();
    if (option.state & QStyle::State_MouseOver && delRect.contains(opt4.widget->mapFromGlobal(QCursor::pos())))
    {
        painter->setPen(Qt::red);
        QFont font = painter->font();
        font.setBold(true);
        painter->setFont(font);
    }

    painter->drawText(option.rect.adjusted(0, 0, -3, 0), Qt::AlignRight | Qt::AlignVCenter, QLatin1String("x"));
    painter->restore();
}

bool EventTagsDelegate::hitTestDelButton(const QModelIndex &index, const QSize &size, const QPoint &pos) const
{
    Q_UNUSED(index);
    /* This is a bit much for the text delete button, but that will likely be an icon instead. */

    if (pos.x() >= size.width()-16)
        return true;

    return false;
}
