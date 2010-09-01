#ifndef EVENTSOURCESMODEL_H
#define EVENTSOURCESMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include <QBitArray>

class DVRServer;
class DVRCamera;

class EventSourcesModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit EventSourcesModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    struct ServerData
    {
        DVRServer *server;
        QVector<DVRCamera*> cameras;
        /* Note that the indexes are +1 from cameras */
        QBitArray checkState;
    };

    QVector<ServerData> servers;
};

#endif // EVENTSOURCESMODEL_H
