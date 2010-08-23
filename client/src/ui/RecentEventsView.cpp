#include "RecentEventsView.h"
#include "EventsModel.h"
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>

RecentEventsView::RecentEventsView(QWidget *parent)
    : QAbstractItemView(parent), m_rowsBottom(0)
{
    setSelectionMode(QAbstractItemView::NoSelection);
}

void RecentEventsView::setModel(EventsModel *model)
{
    QAbstractItemView::setModel(model);
    doFullLayout();
}

QModelIndex RecentEventsView::indexAt(const QPoint &point) const
{
    int y = point.y() + verticalScrollBar()->value();

    if (y >= m_rowsBottom)
        return QModelIndex();

    for (int r = 1; r < m_rowPosition.size(); ++r)
    {
        Q_ASSERT(model());
        if (m_rowPosition[r] > y)
            return model()->index(r-1, 0, rootIndex());
    }

    return model()->index(m_rowPosition.size()-1, 0, rootIndex());
}

QRect RecentEventsView::visualRect(const QModelIndex &index) const
{
    int row = index.row();
    if (row < 0 || row >= m_rowPosition.size())
        return QRect();

    int y = m_rowPosition[row] - verticalScrollBar()->value();
    return QRect(0, y, viewport()->width(), ((row+1 < m_rowPosition.size()) ? m_rowPosition[row+1] : m_rowsBottom) - y);
}

bool RecentEventsView::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

void RecentEventsView::scrollTo(const QModelIndex &index, ScrollHint hint)
{

}

QRegion RecentEventsView::visualRegionForSelection(const QItemSelection &selection) const
{
    return QRegion();
}

void RecentEventsView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
}

QModelIndex RecentEventsView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}

int RecentEventsView::horizontalOffset() const
{
    return 0;
}

int RecentEventsView::verticalOffset() const
{
    return 0;
}

void RecentEventsView::doFullLayout()
{
    m_rowPosition.clear();

    int y = 0;
    for (int r = 0, n = model() ? model()->rowCount(rootIndex()) : 0; r < n; ++r)
    {
        QSize sz = sizeHintForIndex(model()->index(r, 0, rootIndex()));
        Q_ASSERT(sz.isValid());
        m_rowPosition.append(y);
        y += sz.height();
    }

    m_rowsBottom = y;
    updateScrollbars();
}

void RecentEventsView::updateScrollbars()
{
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setMaximum(qMax(0, m_rowsBottom - viewport()->height()));
}

void RecentEventsView::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    QRect rect = viewport()->rect();

    QStyleOptionViewItemV4 viewOpt = viewOptions();

    for (int r = 0; r < m_rowPosition.size(); ++r)
    {
        int y = m_rowPosition[r] - verticalScrollBar()->value();
        int height = ((r+1 < m_rowPosition.size()) ? m_rowPosition[r+1] : m_rowsBottom) - y;

        if (y >= event->rect().bottom())
            break;
        if (y+height < event->rect().top())
            continue;

        QModelIndex idx = model()->index(r, 0, rootIndex());
        Q_ASSERT(idx.isValid());

        QAbstractItemDelegate *delegate = itemDelegate(idx);
        Q_ASSERT(delegate);

        QStyleOptionViewItemV4 opt = viewOpt;
        opt.rect = QRect(0, y, rect.width(), height);

        delegate->paint(&p, opt, idx);
    }
}

void RecentEventsView::resizeEvent(QResizeEvent *event)
{
    QAbstractItemView::resizeEvent(event);
    updateScrollbars();
}
