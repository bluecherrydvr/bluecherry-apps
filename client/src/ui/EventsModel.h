#ifndef EVENTSMODEL_H
#define EVENTSMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>
#include <QSet>

class DVRServer;
class DVRCamera;

class EventLevel
{
public:
    enum Level
    {
        Info = 0,
        Warning,
        Alarm,
        Critical,
        Minimum = Info
    } level;

    EventLevel() : level(Info) { }
    EventLevel(Level l) : level(l) { }
    EventLevel(const QString &l) { *this = l; }

    EventLevel &operator=(Level l)
    {
        level = l;
        return *this;
    }

    EventLevel &operator=(const QString &l)
    {
        if (l == QLatin1String("info"))
            level = Info;
        else if (l == QLatin1String("warning"))
            level = Warning;
        else if (l == QLatin1String("alarm"))
            level = Alarm;
        else if (l == QLatin1String("critical"))
            level = Critical;
        else
            level = Info;

        return *this;
    }

    bool operator<(const EventLevel &o) const { return level < o.level; }
    bool operator>(const EventLevel &o) const { return level > o.level; }
    bool operator==(const EventLevel &o) const { return level == o.level; }
    QString uiString() const;
};

class EventData
{
public:
    DVRServer *server;
    QString location, type;
    QDateTime date;
    int duration;
    EventLevel level;
};

Q_DECLARE_METATYPE(EventData*)

class EventsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum
    {
        EventDataPtr = Qt::UserRole
    };

    explicit EventsModel(QObject *parent = 0);

    void setFilterCameras(const QSet<DVRCamera*> &cameras);
    void setFilterDates(const QDateTime &begin, const QDateTime &end);
    void setFilterLevel(EventLevel minimum);

    QString filterDescription() const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

public slots:
    void setFilterBeginDate(const QDateTime &begin) { setFilterDates(begin, filterDateEnd); }
    void setFilterEndDate(const QDateTime &end) { setFilterDates(filterDateBegin, end); }

signals:
    void filtersChanged();

private slots:
    void serverRemoved(DVRServer *server);

private:
    QList<EventData*> items, cachedEvents;

    /* Filters */
    QSet<DVRCamera*> filterCameras;
    QDateTime filterDateBegin, filterDateEnd;
    EventLevel filterLevel;

    void applyFilters(bool fromCache = true);
    bool testFilter(EventData *data);
};

inline QString EventLevel::uiString() const
{
    switch (level)
    {
    case Info: return EventsModel::tr("Info");
    case Warning: return EventsModel::tr("Warning");
    case Alarm: return EventsModel::tr("Alarm");
    case Critical: return EventsModel::tr("Critical");
    default: return EventsModel::tr("Unknown");
    }
}

#endif // EVENTSMODEL_H
