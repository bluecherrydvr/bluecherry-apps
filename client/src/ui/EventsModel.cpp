#include "EventsModel.h"
#include "core/BluecherryApp.h"
#include "core/DVRServer.h"

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
        event->date = QDateTime::currentDateTime().addSecs(-30238);
        event->duration = 320;
        event->level = EventData::Alarm;
        events.append(event);

        connect(server, SIGNAL(serverRemoved(DVRServer*)), SLOT(serverRemoved(DVRServer*)));
    }
}

int EventsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return events.size();
}

int EventsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 4;
}

QModelIndex EventsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row < 0 || column < 0 || row >= events.size() || column >= columnCount())
        return QModelIndex();

    return createIndex(row, column, events[row]);
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
    for (int i = 0; i < events.size(); ++i)
    {
        if (events[i]->server == server)
        {
            beginRemoveRows(QModelIndex(), i, i);
            events.removeAt(i);
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

    bool operator()(const EventsModel::EventData *e1, const EventsModel::EventData *e2)
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
    qSort(events.begin(), events.end(), EventSort(column, lessThan));
    emit layoutChanged();
}
