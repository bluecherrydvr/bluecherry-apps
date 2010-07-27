#include "core/BluecherryApp.h"
#include "DVRServersModel.h"
#include "core/DVRServer.h"
#include <QTextDocument>

DVRServersModel::DVRServersModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    servers = bcApp->servers();

    foreach (DVRServer *server, servers)
    {
        connect(server, SIGNAL(changed()), SLOT(serverDataChanged()));
    }
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

Qt::ItemFlags DVRServersModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags re = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    if (index.isValid() && index.column() == 0)
        re |= Qt::ItemIsEditable;

    return re;
}

QVariant DVRServersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DVRServer *server = reinterpret_cast<DVRServer*>(index.internalPointer());
    if (!server)
        return QVariant();

    if (role == Qt::ToolTipRole)
    {
        return tr("<span style='white-space:nowrap'><b>%1</b><br>%3 @ %2</span>", "tooltip")
                .arg(Qt::escape(server->displayName()))
                .arg(Qt::escape(server->hostname()))
                .arg(Qt::escape(server->username()));
    }

    switch (index.column())
    {
    case 0:
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return server->displayName();
        break;
    case 1:
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return server->hostname();
        break;
    case 2:
        if (role == Qt::DisplayRole || role == Qt::EditRole)
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

bool DVRServersModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 0 || role != Qt::EditRole)
        return false;

    DVRServer *server = reinterpret_cast<DVRServer*>(index.internalPointer());
    if (!server)
        return false;

    QString name = value.toString().trimmed();
    if (name.isEmpty())
        return false;

    /* dataChanged will be emitted in response to the DVRServer::changed() signal */
    server->setDisplayName(name);
}

void DVRServersModel::serverDataChanged()
{
    DVRServer *server = qobject_cast<DVRServer*>(sender());
    if (!server)
        return;

    int row = servers.indexOf(server);
    if (row < 0)
        return;

    emit dataChanged(index(row, 0), index(row, columnCount()-1));
}
