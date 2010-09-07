#include "EventTagsView.h"
#include "EventTagsDelegate.h"
#include "EventTagsModel.h"
#include <QMouseEvent>

EventTagsView::EventTagsView(QWidget *parent)
    : QListView(parent), cachedSizeHint(0, 0)
{
    setItemDelegate(new EventTagsDelegate(this));
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setMouseTracking(true);
}

QSize EventTagsView::minimumSizeHint() const
{
    return QSize();
}

QSize EventTagsView::sizeHint() const
{
    QMargins margin = contentsMargins();
    return cachedSizeHint + QSize(margin.left() + margin.right(), margin.top() + margin.bottom());
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

void EventTagsView::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index;
    if (event->button() == Qt::LeftButton && (index = indexAt(event->pos())).isValid())
    {
        Q_ASSERT(qobject_cast<EventTagsDelegate*>(itemDelegate(index)));
        EventTagsDelegate *delegate = static_cast<EventTagsDelegate*>(itemDelegate(index));

        QRect indexRect = visualRect(index);
        QPoint indexPos = event->pos() - indexRect.topLeft();

        if (delegate->hitTestDelButton(index, indexRect.size(), indexPos))
        {
            removeTag(index);
            event->accept();
            return;
        }
    }

    QListView::mousePressEvent(event);
}

void EventTagsView::mouseMoveEvent(QMouseEvent *event)
{
    QModelIndex idx = indexAt(event->pos());
    if (idx.isValid())
        update(idx);

    QListView::mouseMoveEvent(event);
}

void EventTagsView::removeTag(const QModelIndex &index)
{
    EventTagsModel *m = qobject_cast<EventTagsModel*>(model());
    Q_ASSERT(m);
    if (!m)
        return;

    m->removeTag(index);
}
