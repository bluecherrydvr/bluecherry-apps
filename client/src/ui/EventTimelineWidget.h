#ifndef EVENTTIMELINEWIDGET_H
#define EVENTTIMELINEWIDGET_H

#include <QAbstractItemView>
#include <QDateTime>

class DVRServer;
struct ServerData;
class EventData;
struct LocationData;

class EventTimelineWidget : public QAbstractItemView
{
    Q_OBJECT

public:
    explicit EventTimelineWidget(QWidget *parent = 0);
    ~EventTimelineWidget();

    virtual QSize sizeHint() const;

    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint);
    virtual QModelIndex indexAt(const QPoint &point) const;
    virtual void setModel(QAbstractItemModel *model);

protected:
    virtual void paintEvent(QPaintEvent *event);

    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual bool isIndexHidden(const QModelIndex &index) const;

    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;

    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command);
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    /* layoutChanged, rowsMoved */

private slots:
    void rowsRemoved(const QModelIndex &parent, int start, int end);
    void modelReset();

private:
    QHash<DVRServer*,ServerData*> serversMap;
    QHash<EventData*,int> rowsMap;

    /* Time represented in the viewport's item area; defines the x scale */
    QDateTime timeStart, timeEnd;
    int timeSeconds;

    int leftPadding() const { return 50; }
    int topPadding() const { return 50; }
    int rowHeight() const { return 20; }
    int cellMinimum() const { return 8; }

    EventData *rowData(int row) const;
    bool findEvent(EventData *event, bool create, ServerData **server, LocationData **location, int *position);

    void addModelRows(int first, int last = -1);
    void clearData();
    /* Update the rowsMap starting with the item at row 'start' and continuing to the end */
    void updateRowsMap(int start = 0);
    void updateTimeRange();

    /* Area of the viewport containing items */
    QRect viewportItemArea() const;
    /* X-coordinate offset from the edge of the viewport item area for the given time */
    int timeXOffset(const QDateTime &time) const;
    QRect timeCellRect(const QDateTime &start, int duration) const;

    void paintRow(QPainter *p, QRect rect, LocationData *locationData);
};

#endif // EVENTTIMELINEWIDGET_H
