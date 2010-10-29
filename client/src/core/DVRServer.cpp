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

    connect(&m_refreshTimer, SIGNAL(timeout()), SLOT(updateCameras()));
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
    if (!api->isOnline())
    {
        m_refreshTimer.stop();
        return;
    }

    if (!m_refreshTimer.isActive())
        m_refreshTimer.start(60000);

    qDebug() << "DVRServer: Requesting cameras list";
    QNetworkReply *reply = api->sendRequest(QUrl(QLatin1String("/ajax/devices.php?XML=1")));
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

    QSet<int> idSet;

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
                    bool ok = false;
                    QString idv = xml.attributes().value(QLatin1String("id")).toString();
                    if (idv.isNull())
                        continue;
                    int deviceId = (int)idv.toUInt(&ok);
                    if (!ok)
                    {
                        xml.raiseError(QLatin1String("Invalid device ID"));
                        continue;
                    }

                    idSet.insert(deviceId);
                    DVRCamera camera = DVRCamera::getCamera(this, deviceId);
                    if (!camera.parseXML(xml))
                    {
                        if (!xml.hasError())
                            xml.raiseError(QLatin1String("Device parsing failed"));
                        continue;
                    }

                    if (!m_cameras.contains(camera))
                    {
                        m_cameras.append(camera);
                        emit cameraAdded(camera);
                    }
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
        return;
    }

    for (int i = 0; i < m_cameras.size(); ++i)
    {
        if (!idSet.contains(m_cameras[i].uniqueId()))
        {
            DVRCamera c = m_cameras[i];
            m_cameras.removeAt(i);
            qDebug("DVRServer: camera %d removed", c.uniqueId());
            emit cameraRemoved(c);
            c.removed();
            --i;
        }
    }
}
