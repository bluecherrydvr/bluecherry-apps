#ifndef RECENTEVENTSVIEW_H
#define RECENTEVENTSVIEW_H

#include <QAbstractItemView>

class EventsModel;

class RecentEventsView : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit RecentEventsView(QWidget *parent = 0);

    virtual void setModel(EventsModel *model);

    virtual QModelIndex indexAt(const QPoint &point) const;
    bool isIndexHidden(const QModelIndex &index) const;

    virtual void scrollTo(const QModelIndex &index, ScrollHint hint);
    virtual QRect visualRect(const QModelIndex &index) const;
    QRegion visualRegionForSelection(const QItemSelection &selection) const;

    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    using QAbstractItemView::setModel;

private:
    QList<int> m_rowPosition;
    int m_rowsBottom;

    void doFullLayout();
    void updateScrollbars();
};

#endif // RECENTEVENTSVIEW_H
