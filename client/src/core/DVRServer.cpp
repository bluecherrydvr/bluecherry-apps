#include "DVRServer.h"
#include "DVRCamera.h"
#include <QSettings>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>
#include <QXmlStreamReader>
#include <QDebug>

DVRServer::DVRServer(int id, QObject *parent)
    : QObject(parent), configId(id)
{
    m_displayName = readSetting("displayName").toString();
    api = new ServerRequestManager(this);

    connect(api, SIGNAL(loginSuccessful()), SLOT(updateCameras()));

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

void DVRServer::updateCameras()
{
    qDebug() << "DVRServer: Requesting cameras list";
    QNetworkReply *reply = api->sendRequest(QUrl(QLatin1String("/ajax/list-devices.php")));
    connect(reply, SIGNAL(finished()), SLOT(updateCamerasReply()));
}

void DVRServer::updateCamerasReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    qDebug() << "DVRServer: Received cameras list reply";

    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError)
    {
        /* TODO: Handle this well */
        qWarning() << "DVRServer: Error from updating cameras:" << reply->errorString();
        return;
    }

    QByteArray data = reply->readAll();
    QXmlStreamReader xml(data);
    QList<DVRCamera*> cameras;

    while (xml.readNextStartElement())
    {
        if (xml.name() == QLatin1String("devices"))
        {
            while (xml.readNext() != QXmlStreamReader::Invalid)
            {
                if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("devices"))
                    break;
                else if (xml.tokenType() != QXmlStreamReader::StartElement)
                    continue;

                if (xml.name() == QLatin1String("device"))
                {
                    DVRCamera *camera = DVRCamera::parseFromXML(this, xml);
                    if (!camera)
                    {
                        qDebug() << "DVRServer: Parsing <device> tag failed:" << xml.errorString();
                        xml.skipCurrentElement();
                        continue;
                    }

                    cameras.append(camera);
                }
            }
            break;
        }
        else
            xml.skipCurrentElement();
    }

    if (xml.hasError())
    {
        qWarning() << "DVRServer: Error while parsing camera list:" << xml.errorString();
        qDeleteAll(cameras);
        return;
    }

    foreach (DVRCamera *c, cameras)
    {
        m_cameras.append(c);
        emit cameraAdded(c);
    }
}
