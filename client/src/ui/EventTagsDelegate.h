#ifndef EVENTTAGSDELEGATE_H
#define EVENTTAGSDELEGATE_H

#include <QStyledItemDelegate>

class EventTagsDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EventTagsDelegate(QObject *parent = 0);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool hitTestDelButton(const QModelIndex &index, const QSize &size, const QPoint &pos) const;
};

#endif // EVENTTAGSDELEGATE_H
