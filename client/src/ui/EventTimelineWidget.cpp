#include "EventTimelineWidget.h"
#include "EventsModel.h"
#include "core/DVRServer.h"
#include <QPaintEvent>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <qmath.h>

struct LocationData
{
    ServerData *serverData;
    QString location;
    QList<EventData*> events;
};

struct ServerData
{
    DVRServer *server;
    QHash<QString,LocationData*> locationsMap;
};

EventTimelineWidget::EventTimelineWidget(QWidget *parent)
    : QAbstractItemView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setAutoFillBackground(false);
}

EventTimelineWidget::~EventTimelineWidget()
{
    clearData();
}

void EventTimelineWidget::clearData()
{
    foreach (ServerData *server, serversMap)
    {
        qDeleteAll(server->locationsMap);
        delete server;
    }

    serversMap.clear();
    rowsMap.clear();
    timeStart = timeEnd = QDateTime();
}

void EventTimelineWidget::setModel(QAbstractItemModel *newModel)
{
    if (newModel == model())
        return;

    if (model())
    {
        model()->disconnect(this);
        clearData();
    }

    connect(newModel, SIGNAL(rowsRemoved(QModelIndex,int,int)), SLOT(rowsRemoved(QModelIndex,int,int)));
    connect(newModel, SIGNAL(modelReset()), SLOT(modelReset()));
    QAbstractItemView::setModel(newModel);

    addModelRows(0);
}

QRect EventTimelineWidget::visualRect(const QModelIndex &index) const
{
    if (!index.isValid())
        return QRect();

    EventData *event = rowData(index.row());
    if (!event)
        return QRect();

    ServerData *serverData;
    LocationData *locationData;
    if (!const_cast<EventTimelineWidget*>(this)->findEvent(event, false, &serverData, &locationData, 0))
        return QRect();

    QRect itemArea = viewportItemArea();
    int y = itemArea.top();
    for (QHash<DVRServer*,ServerData*>::ConstIterator it = serversMap.begin(); it != serversMap.end(); ++it)
    {
        y += rowHeight();
        if (*it != serverData)
        {
            y += rowHeight() * (*it)->locationsMap.size();
            continue;
        }

        for (QHash<QString,LocationData*>::ConstIterator locit = serverData->locationsMap.begin();
             locit != serverData->locationsMap.end(); ++locit)
        {
            if (*locit == locationData)
                break;
            y += rowHeight();
        }

        QRect re = timeCellRect(event->date, event->duration);
        re.translate(itemArea.topLeft());
        re.moveTop(y);
        re.setHeight(rowHeight());

        qDebug() << re;
        return re;
    }

    return QRect();
}

void EventTimelineWidget::scrollTo(const QModelIndex &index, ScrollHint hint)
{

}

EventData *EventTimelineWidget::eventAt(const QPoint &point) const
{
    QRect itemArea = viewportItemArea();
    if (!itemArea.contains(point))
        return 0;

    /* Iterate servers and locations to the specified y coordinate */
    int y = itemArea.top();
    int n = (point.y()-y)/rowHeight();

    for (QHash<DVRServer*,ServerData*>::ConstIterator it = serversMap.begin(); it != serversMap.end(); ++it)
    {
        if (--n < 0)
            return 0;

        int count = (*it)->locationsMap.size();
        if (n >= count)
        {
            n -= count;
            continue;
        }

        LocationData *location = *((*it)->locationsMap.begin() + n);

        /* This is slow and can likely be improved. */
        for (QList<EventData*>::ConstIterator evit = location->events.begin(); evit != location->events.end(); ++evit)
        {
            QRect eventRect = timeCellRect((*evit)->date, (*evit)->duration).translated(itemArea.left(), 0);
            if (point.x() >= eventRect.left() && point.x() <= eventRect.right())
                return *evit;
        }

        break;
    }

    return 0;
}

QModelIndex EventTimelineWidget::indexAt(const QPoint &point) const
{
    EventData *event = eventAt(point);
    if (!event)
        return QModelIndex();

    int row = rowsMap[event];
    return model()->index(row, 0);
}

QSize EventTimelineWidget::sizeHint() const
{
    return QSize(500, 300);
}

QModelIndex EventTimelineWidget::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    return QModelIndex();
}

bool EventTimelineWidget::isIndexHidden(const QModelIndex &index) const
{
    return false;
}

int EventTimelineWidget::horizontalOffset() const
{
    return 0;
}

int EventTimelineWidget::verticalOffset() const
{
    return 0;
}

void EventTimelineWidget::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{

}

QRegion EventTimelineWidget::visualRegionForSelection(const QItemSelection &selection) const
{
    return QRegion();
}

EventData *EventTimelineWidget::rowData(int row) const
{
    QModelIndex idx = model()->index(row, 0);
    return idx.data(EventsModel::EventDataPtr).value<EventData*>();
}

bool EventTimelineWidget::findEvent(EventData *event, bool create, ServerData **server,
                                    LocationData **location, int *position)
{
    if (server)
        *server = 0;
    if (location)
        *location = 0;
    if (position)
        *position = -1;

    /* Find associated server */
    QHash<DVRServer*,ServerData*>::ConstIterator it = serversMap.find(event->server);
    if (it == serversMap.end())
    {
        if (!create)
            return false;

        ServerData *serverData = new ServerData;
        serverData->server = event->server;
        it = serversMap.insert(serverData->server, serverData);
    }

    ServerData *serverData = *it;
    if (server)
        *server = serverData;

    /* Find associated location (within the server) */
    QHash<QString,LocationData*>::ConstIterator lit = serverData->locationsMap.find(event->location);
    if (lit == serverData->locationsMap.end())
    {
        if (!create)
            return false;

        LocationData *locationData = new LocationData;
        locationData->location = event->location;
        locationData->serverData = serverData;
        lit = serverData->locationsMap.insert(locationData->location, locationData);
    }

    LocationData *locationData = *lit;
    if (location)
        *location = locationData;

    if (position && create)
    {
        /* Find the position where this event belongs */
        int p = 0;
        for (int n = locationData->events.size(); p < n; ++p)
        {
            if (event->date < locationData->events[p]->date)
                break;
        }
        *position = p;
    }
    else if (position)
        *position = locationData->events.indexOf(event);

    return true;
}

void EventTimelineWidget::updateTimeRange()
{
    timeStart = timeEnd = QDateTime();

    for (QHash<EventData*,int>::Iterator it = rowsMap.begin(); it != rowsMap.end(); ++it)
    {
        QDateTime date = it.key()->date;
        if (timeStart.isNull() || date < timeStart)
            timeStart = date;
        if (timeEnd.isNull() || date > timeEnd)
            timeEnd = date;
    }

    timeSeconds = timeStart.secsTo(timeEnd);
}

void EventTimelineWidget::updateRowsMap(int row)
{
    for (int n = model()->rowCount(); row < n; ++row)
    {
        EventData *data = rowData(row);
        if (!data)
            continue;

        rowsMap.insert(data, row);
    }
}

void EventTimelineWidget::addModelRows(int first, int last)
{
    if (last < 0)
        last = model()->rowCount() - 1;

    for (int i = first; i <= last; ++i)
    {
        EventData *data = rowData(i);
        if (!data)
            continue;

        LocationData *locationData;
        int pos;
        findEvent(data, true, 0, &locationData, &pos);

        locationData->events.insert(pos, data);
        rowsMap.insert(data, i);

        /* Update time span */
        if (timeStart.isNull() || data->date < timeStart)
            timeStart = data->date;
        if (timeEnd.isNull() || data->date > timeEnd)
            timeEnd = data->date;
    }

    updateRowsMap(last+1);
    timeSeconds = timeStart.secsTo(timeEnd);
}

void EventTimelineWidget::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());
    addModelRows(start, end);
    QAbstractItemView::rowsInserted(parent, start, end);
}

void EventTimelineWidget::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());

    for (int i = start; i <= end; ++i)
    {
        EventData *data = rowData(i);
        if (!data)
            continue;

        ServerData *serverData;
        LocationData *locationData;
        if (!findEvent(data, false, &serverData, &locationData, 0))
            continue;

        locationData->events.removeOne(data);
        rowsMap.remove(data);

        if (locationData->events.isEmpty())
        {
            serverData->locationsMap.remove(locationData->location);
            delete locationData;

            if (serverData->locationsMap.isEmpty())
            {
                serversMap.remove(serverData->server);
                delete serverData;
            }
        }
    }

    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

void EventTimelineWidget::rowsRemoved(const QModelIndex &parent, int start, int end)
{
    Q_ASSERT(!parent.isValid());

    updateRowsMap(start);
    updateTimeRange();
}

void EventTimelineWidget::modelReset()
{
    clearData();
    addModelRows(0);
}

void EventTimelineWidget::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    int firstRow = topLeft.row(), lastRow = bottomRight.row();

    for (int row = firstRow; row <= lastRow; ++row)
    {
        EventData *data = rowData(row);
        Q_ASSERT(rowsMap[data] == row);

        /* Try to find this event to handle (relatively quickly) the common case when
         * location/server do not change. */
        ServerData *server = 0;
        if (findEvent(data, false, &server, 0, 0) || !server)
            continue;

        /* Brute-force search of all locations in this server to find the old one and move it.
         * Server cannot change. */
        foreach (LocationData *location, server->locationsMap)
        {
            int pos = location->events.indexOf(data);
            if (pos < 0)
                continue;

            location->events.removeAt(pos);
            if (location->events.isEmpty())
            {
                server->locationsMap.remove(location->location);
                delete location;
            }

            break;
        }

        LocationData *location;
        int pos;
        findEvent(data, true, &server, &location, &pos);
        location->events.insert(pos, data);
    }

    updateTimeRange();
}

QRect EventTimelineWidget::viewportItemArea() const
{
    return viewport()->rect().adjusted(50, 50, 0, 0);
}

QRect EventTimelineWidget::timeCellRect(const QDateTime &time, int duration) const
{
    Q_ASSERT(time >= timeStart);
    Q_ASSERT(time <= timeEnd);

    double range = qMax(timeSeconds, 1);

    /* Save enough room for a zero-duration item at timeEnd */
    int width = viewportItemArea().width() - cellMinimum();

    QRect r;
    r.setX(qRound((timeStart.secsTo(time) / range) * width));
    r.setWidth(qMax(cellMinimum(), qRound((duration / range) * width)));
    return r;
}

void EventTimelineWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(viewport());
    //p.setBackground(palette().brush(QPalette::Window));
    p.eraseRect(event->rect());

    QRect r = viewport()->rect();

    QAbstractItemModel *model = this->model();
    if (!model)
        return;

    /* Draw timeline (x-axis) */
    int y = 0;

    /* Dates across the top; first one is fully qualified (space permitting) */
    bool first = true;
    int numDays = 0;
    for (QDate date = timeStart.date(), last = timeEnd.date(); date <= last; date = date.addDays(1), ++numDays)
    {
        QRect dateRect = timeCellRect(qMax(QDateTime(date), timeStart), 60*60*24);
        dateRect.setHeight(r.height());
        dateRect.translate(50, 0);
        QString dateStr = date.toString(first ? tr("dddd, MMM d yyyy") : tr("dddd, MMM d"));

        p.drawText(dateRect, 0, dateStr, &dateRect);
        y = qMax(y, dateRect.bottom());

        first = false;
    }

    /* Hours */
    int ny = y;
    QVector<QLine> lines;
    lines.reserve(qCeil(double(timeSeconds)/3600));
    for (QDateTime dt = timeStart; dt <= timeEnd; )
    {
        QTime time = dt.time();
        int secSpan = (60-time.minute()-1)*60 + (60-time.second());

        QRect hourRect = timeCellRect(dt, secSpan);
        hourRect.moveTop(y);
        hourRect.setHeight(r.height());
        hourRect.translate(50, 0);
        QString hourStr = time.toString(tr("h:mm"));

        p.drawText(hourRect, 0, hourStr, &hourRect);
        ny = qMax(ny, hourRect.bottom());

        lines.append(QLine(hourRect.x(), -5, hourRect.x(), 5));

        dt = dt.addSecs(secSpan);
        Q_ASSERT(dt.time().hour() != time.hour());
        Q_ASSERT(dt.time().minute() == 0 && dt.time().second() == 0);
    }

    //y = ny + 8;
    y = topPadding();
    for (QVector<QLine>::Iterator it = lines.begin(); it != lines.end(); ++it)
        it->translate(0, y);

    p.drawLines(lines);

    /* Loop servers */
    y = topPadding();
    QRect textRect(0, 0, leftPadding(), rowHeight());
    QFont serverFont = p.font();
    serverFont.setBold(true);

    /* BUG: No sorting is done here; this shouldn't be looping the hash */
    for (QHash<DVRServer*,ServerData*>::Iterator it = serversMap.begin(); it != serversMap.end(); ++it)
    {
        ServerData *server = *it;

        textRect.moveTop(y);
        p.save();
        p.setFont(serverFont);
        p.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, server->server->displayName());
        p.restore();
        y += rowHeight();

        for (QHash<QString,LocationData*>::Iterator lit = server->locationsMap.begin();
             lit != server->locationsMap.end(); ++lit)
        {
            p.fillRect(QRect(0, y, r.width(), rowHeight()), QColor(230, 230, 230));

            textRect.moveTop(y);
            p.drawText(textRect.adjusted(6, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, (*lit)->location);

            QRect rowRect(leftPadding(), y, r.width(), rowHeight());
            paintRow(&p, rowRect, *lit);
            y += rowRect.height();
        }
    }

    p.drawLine(leftPadding(), topPadding(), leftPadding(), r.height());
    p.drawLine(leftPadding(), topPadding(), r.width(), topPadding());
}

void EventTimelineWidget::paintRow(QPainter *p, QRect r, LocationData *locationData)
{
    for (QList<EventData*>::Iterator it = locationData->events.begin(); it != locationData->events.end(); ++it)
    {
        EventData *data = *it;

        QRect cellRect = timeCellRect(data->date, data->duration);
        cellRect.translate(r.x(), r.y());
        cellRect.setHeight(r.height());
        p->fillRect(cellRect, Qt::blue);
    }
}
