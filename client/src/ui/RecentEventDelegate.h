#ifndef RECENTEVENTDELEGATE_H
#define RECENTEVENTDELEGATE_H

#include <QStyledItemDelegate>

class RecentEventDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RecentEventDelegate(QObject *parent = 0);

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QString displayText(const QModelIndex &index) const;
};

#endif // RECENTEVENTDELEGATE_H
