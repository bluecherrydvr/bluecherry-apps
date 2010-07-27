#include "core/BluecherryApp.h"
#include "DVRServersModel.h"
#include "core/DVRServer.h"

DVRServersModel::DVRServersModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    servers = bcApp->servers();
}

int DVRServersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return servers.size();
}

int DVRServersModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QModelIndex DVRServersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row < 0 || column < 0 || row >= servers.size() || column >= columnCount())
        return QModelIndex();

    return createIndex(row, column, servers[row]);
}

QModelIndex DVRServersModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

QVariant DVRServersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DVRServer *server = reinterpret_cast<DVRServer*>(index.internalPointer());
    if (!server)
        return QVariant();

    switch (index.column())
    {
    case 0:
        if (role == Qt::DisplayRole)
            return server->displayName();
        break;
    case 1:
        if (role == Qt::DisplayRole)
            return server->hostname();
        break;
    case 2:
        if (role == Qt::DisplayRole)
            return server->username();
        break;
    }

    return QVariant();
}

QVariant DVRServersModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
    case 0:
        return tr("DVR Server");
    case 1:
        return tr("Hostname");
    case 2:
        return tr("Username");
    }

    return QVariant();
}
