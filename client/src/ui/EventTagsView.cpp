#include "EventTagsView.h"
#include "EventTagsDelegate.h"

EventTagsView::EventTagsView(QWidget *parent)
    : QListView(parent), cachedSizeHint(0, 0)
{
    setItemDelegate(new EventTagsDelegate(this));
}

QSize EventTagsView::minimumSizeHint() const
{
    return QSize();
}

QSize EventTagsView::sizeHint() const
{
    return cachedSizeHint;
}

void EventTagsView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    calculateSizeHint();
}

void EventTagsView::reset()
{
    QListView::reset();
    calculateSizeHint();
}

void EventTagsView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i)
    {
        QSize isz = sizeHintForIndex(model()->index(i, modelColumn(), parent));
        cachedSizeHint.rwidth() = qMax(cachedSizeHint.width(), isz.width());
        cachedSizeHint.rheight() += isz.height();
    }

    updateGeometry();
}

void EventTagsView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    for (int i = start; i <= end; ++i)
    {
        QSize isz = sizeHintForIndex(model()->index(i, modelColumn(), parent));
        cachedSizeHint.rheight() -= isz.height();
    }

    updateGeometry();
}

void EventTagsView::calculateSizeHint()
{
    ensurePolished();
    QSize size(0, 0);

    QModelIndex parent = rootIndex();
    for (int i = 0, n = model()->rowCount(parent); i < n; ++i)
    {
        QSize isz = sizeHintForIndex(model()->index(i, modelColumn(), parent));
        size.rwidth() = qMax(size.width(), isz.width());
        size.rheight() += isz.height();
    }

    cachedSizeHint = size;
    updateGeometry();
}
