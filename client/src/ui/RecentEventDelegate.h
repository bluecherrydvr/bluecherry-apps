#ifndef RECENTEVENTDELEGATE_H
#define RECENTEVENTDELEGATE_H

#include <QStyledItemDelegate>

class QTextLayout;

class RecentEventDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RecentEventDelegate(QObject *parent = 0);

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    void createText(const QModelIndex &index, const QStyleOptionViewItem &option, QTextLayout *layout) const;
};

#endif // RECENTEVENTDELEGATE_H
