#ifndef EVENTSMODEL_H
#define EVENTSMODEL_H

#include <QAbstractItemModel>
#include <QDateTime>
#include <QList>

class DVRServer;

class EventsModel : public QAbstractItemModel
{
    Q_OBJECT

    class EventData
    {
    public:
        DVRServer *server;
        QString location, type;
        QDateTime date;
        int duration;
        enum Level
        {
            Info,
            Warning,
            Alarm,
            Critical
        } level;
    };

public:
    explicit EventsModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private slots:
    void serverRemoved(DVRServer *server);

private:
    QList<EventData*> events;
};

#endif // EVENTSMODEL_H
