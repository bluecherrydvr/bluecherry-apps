#include "core/BluecherryApp.h"
#include "DVRServersModel.h"
#include "core/DVRServer.h"
#include "core/DVRCamera.h"
#include <QTextDocument>
#include <QApplication>
#include <QStyle>
#include <QMimeData>
#include <QDataStream>

DVRServersModel::DVRServersModel(QObject *parent)
    : QAbstractItemModel(parent), m_offlineDisabled(false)
{
    statusIcon = QIcon(QLatin1String(":/icons/status.png"));
    statusIcon.addFile(QLatin1String(":/icons/status-offline.png"), QSize(), QIcon::Disabled, QIcon::Off);

    connect(bcApp, SIGNAL(serverAdded(DVRServer*)), SLOT(serverAdded(DVRServer*)));
    connect(bcApp, SIGNAL(serverRemoved(DVRServer*)), SLOT(serverRemoved(DVRServer*)));

    servers = bcApp->servers();

    foreach (DVRServer *server, servers)
    {
        connect(server, SIGNAL(changed()), SLOT(serverDataChanged()));
        connect(server->api, SIGNAL(statusChanged(int)), SLOT(serverDataChanged()));
    }
}

DVRServer *DVRServersModel::serverForRow(const QModelIndex &index) const
{
    DVRServer *server = qobject_cast<DVRServer*>((QObject*)index.internalPointer());
    if (server)
        return server;

    return 0;
}

DVRCamera *DVRServersModel::cameraForRow(const QModelIndex &index) const
{
    DVRCamera *camera = qobject_cast<DVRCamera*>((QObject*)index.internalPointer());
    if (camera)
        return camera;

    return 0;
}

QModelIndex DVRServersModel::indexForServer(DVRServer *server) const
{
    int row = servers.indexOf(server);
    if (row < 0)
        return QModelIndex();

    return index(row, 0);
}

void DVRServersModel::setOfflineDisabled(bool offlineDisabled)
{
    if (m_offlineDisabled == offlineDisabled)
        return;

    m_offlineDisabled = offlineDisabled;
    emit dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
}

int DVRServersModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return servers.size();

    DVRServer *server = serverForRow(parent);
    if (server)
        return server->cameras().size();

    return 0;
}

int DVRServersModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 3;
    else
        return 3;
}

QModelIndex DVRServersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        if (row < 0 || column < 0 || row >= servers.size() || column >= columnCount())
            return QModelIndex();

        return createIndex(row, column, (QObject*)servers[row]);
    }
    else
    {
        DVRServer *server = serverForRow(parent);
        if (!server || row < 0 || column < 0 || column >= columnCount(parent))
            return QModelIndex();

        QList<DVRCamera*> cameras = server->cameras();
        if (row >= cameras.size())
            return QModelIndex();

        return createIndex(row, column, (QObject*)cameras[row]);
    }
}

QModelIndex DVRServersModel::parent(const QModelIndex &child) const
{
    DVRCamera *camera = cameraForRow(child);
    if (!camera)
        return QModelIndex();

    return indexForServer(camera->server);
}

Qt::ItemFlags DVRServersModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags re = Qt::ItemIsSelectable;
    DVRServer *s;
    DVRCamera *c;

    if ((c = cameraForRow(index)))
        s = c->server;
    else
        s = serverForRow(index);

    if (!m_offlineDisabled || s && s->api->isOnline())
        re |= Qt::ItemIsEnabled;
    else
        return re;

    if (!index.parent().isValid())
        re |= Qt::ItemIsEditable;
    else
        re |= Qt::ItemIsDragEnabled;

    return re;
}

QVariant DVRServersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    DVRServer *server = serverForRow(index);
    if (server)
    {
        if (role == Qt::ToolTipRole)
        {
            return tr("<span style='white-space:nowrap'><b>%1</b><br>%3 @ %2</span>", "tooltip")
                    .arg(Qt::escape(server->displayName()))
                    .arg(Qt::escape(server->hostname()))
                    .arg(Qt::escape(server->username()));
        }
        else if (role == ServerPtrRole)
            return QVariant::fromValue(server);

        switch (index.column())
        {
        case 0:
            if (role == Qt::DisplayRole || role == Qt::EditRole)
                return server->displayName();
            else if (role == Qt::DecorationRole)
            {
                if (m_offlineDisabled)
                    return statusIcon;
                else
                    return statusIcon.pixmap(16, server->api->isOnline() ? QIcon::Normal : QIcon::Disabled);
            }
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
    }

    DVRCamera *camera = cameraForRow(index);
    if (camera)
    {
        switch (role)
        {
        case Qt::DisplayRole:
            return camera->displayName();
        }
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
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    DVRServer *server = serverForRow(index);
    if (!server)
        return false;

    switch (index.column())
    {
    case 0:
        {
            QString name = value.toString().trimmed();
            if (name.isEmpty())
                return false;

            /* dataChanged will be emitted in response to the DVRServer::changed() signal */
            server->setDisplayName(name);
        }
        break;
    case 1:
        server->writeSetting("hostname", value.toString());
        break;
    case 2:
        server->writeSetting("username", value.toString());
        break;
    case 3:
        server->writeSetting("password", value.toString());
        break;
    default:
        return false;
    }

    return true;
}

QStringList DVRServersModel::mimeTypes() const
{
    return QStringList() << QLatin1String("application/x-bluecherry-dvrcamera");
}

/* application/x-bluecherry-dvrcamera is a list of int,int where each pair has the
 * IDs of the server and the camera respectively. This is only used internally. */

QMimeData *DVRServersModel::mimeData(const QModelIndexList &indexes) const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes)
    {
        DVRCamera *camera;
        if (!index.isValid() || !(camera = cameraForRow(index)))
            continue;

        stream << camera->server->configId << camera->uniqueID;
    }

    QMimeData *mime = new QMimeData;
    mime->setData(QLatin1String("application/x-bluecherry-dvrcamera"), data);
    return mime;
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

void DVRServersModel::serverAdded(DVRServer *server)
{
    beginInsertRows(QModelIndex(), servers.size(), servers.size());
    servers.append(server);
    endInsertRows();

    connect(server, SIGNAL(changed()), SLOT(serverDataChanged()));
}

void DVRServersModel::serverRemoved(DVRServer *server)
{
    int row = servers.indexOf(server);
    if (row < 0)
        return;

    beginRemoveRows(QModelIndex(), row, row);
    servers.removeAt(row);
    endRemoveRows();
}
