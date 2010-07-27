#ifndef DVRSERVERSMODEL_H
#define DVRSERVERSMODEL_H

#include <QAbstractItemModel>

class DVRServer;

class DVRServersModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit DVRServersModel(QObject *parent = 0);

    DVRServer *serverForRow(const QModelIndex &index) const { return serverForRow(index.row()); }
    DVRServer *serverForRow(int row) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

private slots:
    void serverDataChanged();

private:
    QList<DVRServer*> servers;
};

#endif // DVRSERVERSMODEL_H
