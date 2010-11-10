#ifndef EVENTSMODEL_H
#define EVENTSMODEL_H

#include "core/EventData.h"
#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>
#include <QSet>
#include <QColor>
#include <QBitArray>
#include <QHash>
#include <QTimer>

class DVRServer;
class DVRCamera;

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

    void setFilterDates(const QDateTime &begin, const QDateTime &end);

    QString filterDescription() const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

public slots:
    void setFilterSources(const QMap<DVRServer*,QList<int> > &sources);
    void setFilterTypes(const QBitArray &typemap);
    void setFilterLevel(EventLevel minimum);

    void setFilterBeginDate(const QDateTime &begin) { setFilterDates(begin, filterDateEnd); }
    void setFilterEndDate(const QDateTime &end) { setFilterDates(filterDateBegin, end); }

    /* Request the most recent events from the given server, the DVRServer* source, or the
     * DVRServer represented by the ServerRequestManager* source */
    void updateServer(DVRServer *server = 0);
    void updateServers();

    void setUpdateInterval(int ms);
    void stopUpdates() { setUpdateInterval(-1); }

signals:
    void filtersChanged();

private slots:
    void serverAdded(DVRServer *server);
    void serverRemoved(DVRServer *server);
    void requestFinished();

private:
    QList<EventData*> items;
    QHash<DVRServer*,QList<EventData*> > cachedEvents;
    QTimer updateTimer;

    /* Filters */
    QHash<DVRServer*, QSet<int> > filterSources;
    QDateTime filterDateBegin, filterDateEnd;
    QBitArray filterTypes;
    EventLevel filterLevel;

    /* Sorting */
    int sortColumn;
    Qt::SortOrder sortOrder;

    void resort() { sort(sortColumn, sortOrder); }

    void applyFilters(bool fromCache = true);
    bool testFilter(EventData *data);

    void createTestData();
};

#endif // EVENTSMODEL_H
