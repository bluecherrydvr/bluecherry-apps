#include "BluecherryApp.h"
#include "DVRServer.h"
#include <QSettings>
#include <QStringList>

BluecherryApp *bcApp = 0;

BluecherryApp::BluecherryApp()
    : m_maxServerId(-1)
{
    Q_ASSERT(!bcApp);
    bcApp = this;

    loadServers();
}

void BluecherryApp::loadServers()
{
    Q_ASSERT(m_servers.isEmpty());

    QSettings settings;
    settings.beginGroup(QLatin1String("servers"));
    QStringList groups = settings.childGroups();

    foreach (QString group, groups)
    {
        bool ok = false;
        int id = (int)group.toUInt(&ok);
        if (!ok)
        {
            qWarning("Ignoring invalid server ID from configuration");
            continue;
        }

        DVRServer *server = new DVRServer(id, this);
        m_servers.append(server);
        qDebug("Loaded server %d: %s", id, qPrintable(server->displayName()));

        m_maxServerId = qMax(m_maxServerId, id);
    }
}

DVRServer *BluecherryApp::addNewServer(const QString &name)
{
    int id = ++m_maxServerId;

    QSettings settings;
    settings.setValue(QString::fromLatin1("servers/%1/displayName").arg(id), name);

    DVRServer *server = new DVRServer(id, this);
    emit serverAdded(server);
    return server;
}
