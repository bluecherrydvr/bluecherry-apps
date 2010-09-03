#include "EventsModel.h"
#include "core/BluecherryApp.h"
#include "core/DVRServer.h"
#include "core/DVRCamera.h"
#include <QTextDocument>
#include <QColor>

EventsModel::EventsModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<DVRServer*> servers = bcApp->servers();
    foreach (DVRServer *server, servers)
        connect(server, SIGNAL(serverRemoved(DVRServer*)), SLOT(serverRemoved(DVRServer*)));

    createTestData();

    applyFilters();
    sort(3, Qt::DescendingOrder);
}

/* Randomized events for testing until real ones are available */
void EventsModel::createTestData()
{
    unsigned seed = QDateTime::currentDateTime().time().msec() * QDateTime::currentDateTime().time().second();
    qsrand(seed);
    qDebug("seed: %u", seed);

    QList<DVRServer*> servers = bcApp->servers();
    if (servers.isEmpty())
        return;

    QDateTime end = QDateTime::currentDateTime().addSecs(-3600);
    int duration = 86400 * 7; /* one week */

    int count = (qrand() % 285) + 15;
    for (int i = 0; i < count; ++i)
    {
        EventData *event = new EventData(servers[qrand() % servers.size()]);
        event->date = end.addSecs(-((qrand() * qrand()) % duration));
        event->duration = 1 + (qrand() % 1299);

        bool useCamera = qrand() % 6;
        if (useCamera && !event->server->cameras().isEmpty())
        {
            event->setLocation(QString::fromLatin1("camera-%1").arg(qrand() % event->server->cameras().size()));
            event->type = (qrand() % 10) ? QLatin1String("motion") : QLatin1String("video signal loss");
            event->level = (qrand() % 3) ? EventLevel::Warning : EventLevel::Alarm;
            if (event->type == EventType::CameraVideoLost)
                event->level = EventLevel::Critical;
        }
        else
        {
            event->setLocation(QString::fromLatin1("system"));
            event->type = (qrand() % 10) ? QLatin1String("disk-space") : QLatin1String("crash");
            event->level = (qrand() % 5) ? EventLevel::Info : EventLevel::Critical;
        }

        cachedEvents.append(event);
    }
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
    {
        return QVariant::fromValue(data);
    }
    else if (role == Qt::ToolTipRole)
    {
        return tr("%1 (%2)<br>%3 on %4<br>%5").arg(data->uiType(), data->uiLevel(), Qt::escape(data->uiLocation()),
                                                   Qt::escape(data->uiServer()), data->date.toString());
    }
    else if (role == Qt::ForegroundRole)
    {
        return data->uiColor();
    }

    switch (index.column())
    {
    case 0:
        if (role == Qt::DisplayRole)
            return data->server->displayName();
        break;
    case 1:
        if (role == Qt::DisplayRole)
            return data->uiLocation();
        break;
    case 2:
        if (role == Qt::DisplayRole)
            return data->uiType();
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
            re = QString::localeAwareCompare(e1->uiLocation(), e2->uiLocation()) <= 0;
            break;
        case 2: /* Type */
            re = QString::localeAwareCompare(e1->uiType(), e2->uiType()) <= 0;
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
    if (data->level < filterLevel ||
        (!filterDateBegin.isNull() && data->date < filterDateBegin) ||
        (!filterDateEnd.isNull() && data->date > filterDateEnd))
        return false;

    QHash<DVRServer*, QSet<int> >::Iterator it = filterSources.find(data->server);
    if (!filterSources.isEmpty() && (it == filterSources.end() || !it->contains(data->locationId)))
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

    bool allCameras = true;
    for (QHash<DVRServer*, QSet<int> >::ConstIterator it = filterSources.begin();
         it != filterSources.end(); ++it)
    {
        if (it->count() != it.key()->cameras().size()+1)
            allCameras = false;
    }

    if (!filterSources.isEmpty() && filterSources.size() != bcApp->servers().size())
    {
        if (filterSources.size() == 1)
        {
            /* Single server */
            if (!allCameras)
            {
                if (filterSources.begin()->size() == 1)
                    re += tr(" on %1").arg(*filterSources.begin()->begin());
                else
                    re += tr(" on selected cameras");
            }

            re += tr(" from %1").arg(filterSources.begin().key()->displayName());
        }
        else if (!allCameras)
            re += tr(" on selected cameras");
        else
            re += tr(" from selected servers");
    }
    else if (!allCameras)
        re += tr(" on selected cameras");

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

void EventsModel::setFilterSources(const QMap<DVRServer*, QList<int> > &sources)
{
    bool fast = false;

    if (sources.size() <= filterSources.size())
    {
        fast = true;
        /* If the new sources contain any that the old don't, we can't do fast filtering */
        for (QMap<DVRServer*,QList<int> >::ConstIterator nit = sources.begin(); nit != sources.end(); ++nit)
        {
            QHash<DVRServer*, QSet<int> >::Iterator oit = filterSources.find(nit.key());
            if (oit == filterSources.end())
            {
                fast = false;
                break;
            }

            for (QList<int>::ConstIterator it = nit->begin(); it != nit->end(); ++it)
            {
                if (!oit->contains(*it))
                {
                    fast = false;
                    break;
                }
            }

            if (!fast)
                break;
        }
    }

    filterSources.clear();
    for (QMap<DVRServer*, QList<int> >::ConstIterator nit = sources.begin(); nit != sources.end(); ++nit)
        filterSources.insert(nit.key(), nit->toSet());

    applyFilters(!fast);
}
