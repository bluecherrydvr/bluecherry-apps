#include "DVRServer.h"
#include "DVRCamera.h"
#include <QSettings>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>

DVRServer::DVRServer(int id, QObject *parent)
    : QObject(parent), configId(id)
{
    m_displayName = readSetting("displayName").toString();
    api = new ServerRequestManager(this);

    /* Create some fake cameras for testing */
    for (int i = 0; i < configId+2; ++i)
    {
        DVRCamera *camera = new DVRCamera(this, i);
        m_cameras.append(camera);
    }

    if (!hostname().isEmpty() && !username().isEmpty())
        QTimer::singleShot(0, this, SLOT(login()));
}

QVariant DVRServer::readSetting(const QString &key) const
{
    QSettings settings;
    return settings.value(QString::fromLatin1("servers/%1/").arg(configId).append(key));
}

void DVRServer::writeSetting(const QString &key, const QVariant &value)
{
    QSettings settings;
    settings.setValue(QString::fromLatin1("servers/%1/").arg(configId).append(key), value);

    emit changed();
}

void DVRServer::clearSetting(const QString &key)
{
    QSettings settings;
    settings.remove(QString::fromLatin1("servers/%1/").arg(configId).append(key));

    emit changed();
}

void DVRServer::setDisplayName(const QString &name)
{
    if (m_displayName == name)
        return;

    m_displayName = name;
    writeSetting("displayName", name);
}

DVRCamera *DVRServer::findCamera(int id) const
{
    for (QList<DVRCamera*>::ConstIterator it = m_cameras.begin(); it != m_cameras.end(); ++it)
    {
        if ((*it)->uniqueID == id)
            return *it;
    }

    return 0;
}

void DVRServer::removeServer()
{
    qDebug("Deleting DVR server %d", configId);

    emit serverRemoved(this);

    QSettings settings;
    settings.remove(QString::fromLatin1("servers/%1").arg(configId));

    deleteLater();
}

void DVRServer::login()
{
    api->login(username(), password());
}

QNetworkRequest DVRServer::createRequest(const QUrl &relurl)
{
    QUrl url(relurl);
    url.setScheme(QLatin1String("http"));
    url.setHost(hostname());

    return QNetworkRequest(url);
}
