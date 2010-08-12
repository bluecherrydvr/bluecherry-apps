#ifndef EVENTTIMELINEWIDGET_H
#define EVENTTIMELINEWIDGET_H

#include <QAbstractItemView>
#include <QDateTime>

class DVRServer;
struct ServerData;
class EventData;
struct LocationData;
class QRubberBand;

class EventTimelineWidget : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(double zoomLevel READ zoomLevel WRITE setZoomLevel)
    Q_PROPERTY(int zoomSeconds READ zoomSeconds WRITE setZoomSeconds NOTIFY zoomSecondsChanged)

public:
    explicit EventTimelineWidget(QWidget *parent = 0);
    ~EventTimelineWidget();

    /* Zoom in the number of seconds of time visible on screen */
    int zoomSeconds() const;
    int minZoomSeconds() const { return qMin(timeSeconds, 60); }
    int maxZoomSeconds() const { return timeSeconds; }

    /* Zoom level in the range of 0-100, 0 showing everything with no scroll */
    double zoomLevel() const;

    virtual QSize sizeHint() const;
    virtual QRect visualRect(const QModelIndex &index) const;
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint);
    virtual QModelIndex indexAt(const QPoint &point) const;
    virtual void setModel(QAbstractItemModel *model);

public slots:
    void setZoomLevel(double level);
    void setZoomSeconds(int seconds);

signals:
    void zoomSecondsChanged(int zoomSeconds);
    void zoomRangeChanged(int min, int max);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual bool viewportEvent(QEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

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
    void setViewStartOffset(int secs);

private:
    QHash<DVRServer*,ServerData*> serversMap;
    QHash<EventData*,int> rowsMap;

    /* Total span of time represented by the timeline, rounded from dataTime */
    QDateTime timeStart, timeEnd;
    /* Span of time represented in data; end is inclusive of the duration */
    QDateTime dataTimeStart, dataTimeEnd;
    /* Span of time shown in the viewport */
    QDateTime viewTimeStart, viewTimeEnd;
    /* Span of seconds between timeStart and timeEnd */
    int timeSeconds;
    /* Span of seconds between viewTimeStart and viewTimeEnd */
    int viewSeconds;
    /* Seconds of time per primary tick (a x-axis label), derived from the view area
     * and a minimum width and rounded up to a user-friendly duration in updateTimeRange */
    int primaryTickSecs;

    int cachedTopPadding;

    /* Mouse events */
    QPoint mouseClickPos;
    QRubberBand *mouseRubberBand;

    int leftPadding() const { return 50; }
    int topPadding() const { return cachedTopPadding; }
    int rowHeight() const { return 20; }
    int cellMinimum() const { return 8; }

    EventData *rowData(int row) const;
    bool findEvent(EventData *event, bool create, ServerData **server, LocationData **location, int *position);

    void addModelRows(int first, int last = -1);
    void clearData();
    /* Update the rowsMap starting with the item at row 'start' and continuing to the end */
    void updateRowsMap(int start = 0);
    /* Call when the time range in the underlying data may have changed. If fromData is true, dataTimeStart
     * and dataTimeEnd will be updated. Must be called regardless, to update various other cached data. */
    void updateTimeRange(bool fromData = true);
    /* Ensure that the view time is within the boundaries of the data, changing it (scrolling or zooming) if necessary */
    void ensureViewTimeSpan();
    /* Update the scroll bar position, which is necessary when viewSeconds has changed */
    void updateScrollBars();

    EventData *eventAt(const QPoint &point) const;

    /* Area of the viewport containing items */
    QRect viewportItemArea() const;
    /* X-coordinate offset from the edge of the viewport item area for the given time */
    int timeXOffset(const QDateTime &time) const;
    QRect timeCellRect(const QDateTime &start, int duration) const;

    void paintRow(QPainter *p, QRect rect, LocationData *locationData);
};

#endif // EVENTTIMELINEWIDGET_H
