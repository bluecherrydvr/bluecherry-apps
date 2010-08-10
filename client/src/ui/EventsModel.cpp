#include "EventsModel.h"
#include "core/BluecherryApp.h"
#include "core/DVRServer.h"
#include "core/DVRCamera.h"

EventsModel::EventsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    /* Populate with fake data */
    QList<DVRServer*> servers = bcApp->servers();
    foreach (DVRServer *server, servers)
    {
        EventData *event = new EventData;
        event->server = server;
        event->location = QLatin1String("camera-1");
        event->type = QLatin1String("motion");
        event->date = QDateTime::currentDateTime().addSecs(-(qrand() % 86400)).addDays(-(qrand() % 4));
        event->duration = 320;
        event->level = EventLevel::Alarm;
        cachedEvents.append(event);

        connect(server, SIGNAL(serverRemoved(DVRServer*)), SLOT(serverRemoved(DVRServer*)));
    }

    applyFilters();
    sort(3, Qt::DescendingOrder);
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return items.size();
}

int EventsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QModelIndex EventsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row < 0 || column < 0 || row >= items.size() || column >= columnCount())
        return QModelIndex();

    return createIndex(row, column, items[row]);
}

QModelIndex EventsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant EventsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    EventData *data = reinterpret_cast<EventData*>(index.internalPointer());
    if (!data)
        return QVariant();

    if (role == EventDataPtr)
        return QVariant::fromValue(data);

    switch (index.column())
    {
    case 0:
        if (role == Qt::DisplayRole)
            return data->server->displayName();
        break;
    case 1:
        if (role == Qt::DisplayRole)
            return data->location;
        break;
    case 2:
        if (role == Qt::DisplayRole)
            return data->type;
        break;
    case 3:
        if (role == Qt::DisplayRole)
            return data->date.toString();
        else if (role == Qt::EditRole)
            return data->date;
        break;
    }

    return QVariant();
}

QVariant EventsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0: return tr("Server");
    case 1: return tr("Location");
    case 2: return tr("Type");
    case 3: return tr("Date");
    }

    return QVariant();
}

void EventsModel::serverRemoved(DVRServer *server)
{
    /* Remove all events from this server; could be much faster */
    for (int i = 0; i < items.size(); ++i)
    {
        if (items[i]->server == server)
        {
            beginRemoveRows(QModelIndex(), i, i);
            items.removeAt(i);
            endRemoveRows();
            --i;
        }
    }
}

class EventSort
{
public:
    const int column;
    const bool lessThan;

    EventSort(int c, bool l)
        : column(c), lessThan(l)
    {
    }

    bool operator()(const EventData *e1, const EventData *e2)
    {
        bool re;

        switch (column)
        {
        case 0: /* Server */
            re = QString::localeAwareCompare(e1->server->displayName(), e2->server->displayName()) <= 0;
            break;
        case 1: /* Location */
            re = QString::localeAwareCompare(e1->location, e2->location) <= 0;
            break;
        case 2: /* Type */
            re = QString::localeAwareCompare(e1->type, e2->type) <= 0;
            break;
        case 3: /* Date */
            re = e1->date <= e2->date;
            break;
        default:
            Q_ASSERT_X(false, "EventSort", "sorting not implemented for column");
            re = true;
        }

        if (lessThan)
            return re;
        else
            return !re;
    }
};

void EventsModel::sort(int column, Qt::SortOrder order)
{
    emit layoutAboutToBeChanged();
    bool lessThan = order == Qt::AscendingOrder;
    qSort(items.begin(), items.end(), EventSort(column, lessThan));
    emit layoutChanged();
}

void EventsModel::applyFilters(bool fromCache)
{
    if (fromCache)
    {
        beginResetModel();
        items.clear();

        for (QList<EventData*>::Iterator it = cachedEvents.begin(); it != cachedEvents.end(); ++it)
        {
            if (testFilter(*it))
                items.append(*it);
        }

        endResetModel();
    }
    else
    {
        for (int i = 0; i < items.size(); ++i)
        {
            if (!testFilter(items[i]))
            {
                emit beginRemoveRows(QModelIndex(), i, i);
                items.removeAt(i);
                emit endRemoveRows();
                --i;
            }
        }
    }

    emit filtersChanged();
}

bool EventsModel::testFilter(EventData *data)
{
    /* TODO: cameras */

    if (data->level < filterLevel ||
        (!filterDateBegin.isNull() && data->date < filterDateBegin) ||
        (!filterDateEnd.isNull() && data->date > filterDateEnd))
        return false;

    return true;
}

QString EventsModel::filterDescription() const
{
    QString re;

    if (filterLevel > EventLevel::Info)
        re = tr("%1 events").arg(filterLevel.uiString());
    else if (filterDateBegin.isNull() && filterDateEnd.isNull())
        re = tr("Recent events");
    else
        re = tr("All events");

    if (filterCameras.isEmpty())
        re += tr(" on all cameras");
    else if (filterCameras.size() > 1)
        re += tr(" on selected cameras");
    else
        re += tr(" on %1").arg((*filterCameras.begin())->displayName());

    if (!filterDateBegin.isNull() && !filterDateEnd.isNull())
        re += tr(" from %1 to %2");
    else if (!filterDateBegin.isNull())
        re += tr(" starting %1").arg(filterDateBegin.toString());
    else if (!filterDateEnd.isNull())
        re += tr(" ending %1").arg(filterDateEnd.toString());

    return re;
}

void EventsModel::setFilterDates(const QDateTime &begin, const QDateTime &end)
{
    bool fast = false;
    if (begin >= filterDateBegin && end <= filterDateEnd)
        fast = true;

    filterDateBegin = begin;
    filterDateEnd = end;

    applyFilters(!fast);
}

void EventsModel::setFilterLevel(EventLevel minimum)
{
    if (filterLevel == minimum)
        return;

    bool fast = minimum > filterLevel;
    filterLevel = minimum;

    applyFilters(!fast);
}
