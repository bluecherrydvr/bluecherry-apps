#include "DVRCamera.h"
#include "DVRServer.h"
#include "BluecherryApp.h"
#include "MJpegStream.h"
#include <QXmlStreamReader>
#include <QMimeData>
#include <QDebug>

QHash<QPair<int,int>,DVRCameraData*> DVRCameraData::instances;

DVRCamera DVRCamera::getCamera(int serverID, int cameraID)
{
    DVRServer *server = bcApp->findServerID(serverID);
    if (!server)
        return 0;

    return getCamera(server, cameraID);
}

DVRCamera DVRCamera::getCamera(DVRServer *server, int cameraID)
{
    DVRCameraData *data = DVRCameraData::instances.value(qMakePair(server->configId, cameraID), 0);
    if (!data)
        data = new DVRCameraData(server, cameraID);

    return DVRCamera(data);
}

bool DVRCamera::parseXML(QXmlStreamReader &xml)
{
    if (!isValid())
        return false;

    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("device"));

    QString name, streamUrl;

    while (xml.readNext() != QXmlStreamReader::Invalid)
    {
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("device"))
            break;
        else if (xml.tokenType() != QXmlStreamReader::StartElement)
            continue;

        if (xml.name() == QLatin1String("device_name"))
        {
            name = xml.readElementText();
        }
        else
            xml.skipCurrentElement();
    }

    if (name.isEmpty())
        name = QString::fromLatin1("#%2").arg(uniqueId());

    d->displayName = name;
    QUrl url(QLatin1String("/media/mjpeg.php?multipart"));
    url.addQueryItem(QLatin1String("id"), QString::number(d->uniqueID));
    d->streamUrl = server()->api->serverUrl().resolved(url).toString().toLatin1();
    d->isLoaded = true;

    emit d->dataUpdated();
    return true;
}

QSharedPointer<MJpegStream> DVRCamera::mjpegStream()
{
    QSharedPointer<MJpegStream> re;

    if (!d || d->mjpegStream.isNull())
    {
        if (d && !d->streamUrl.isEmpty())
        {
            re = QSharedPointer<MJpegStream>(new MJpegStream(QUrl(QString::fromLatin1(d->streamUrl))));
            d->mjpegStream = re;
        }
    }
    else
        re = d->mjpegStream.toStrongRef();

    return re;
}

DVRCameraData::DVRCameraData(DVRServer *s, int i)
    : server(s), uniqueID(i), isLoaded(false)
{
    Q_ASSERT(instances.find(qMakePair(s->configId, i)) == instances.end());
    instances.insert(qMakePair(server->configId, uniqueID), this);
}

DVRCameraData::~DVRCameraData()
{
    instances.remove(qMakePair(server->configId, uniqueID));
}

QDataStream &operator<<(QDataStream &s, const DVRCamera &camera)
{
    if (!camera.isValid())
        s << -1;
    else
        s << camera.server()->configId << camera.uniqueId();
    return s;
}

QDataStream &operator>>(QDataStream &s, DVRCamera &camera)
{
    int serverId = -1, cameraId = -1;
    s >> serverId;

    if (s.status() != QDataStream::Ok || serverId < 0)
    {
        camera = DVRCamera();
        return s;
    }

    s >> cameraId;
    camera = DVRCamera::getCamera(serverId, cameraId);
    return s;
}

QList<DVRCamera> DVRCamera::fromMimeData(const QMimeData *mimeData)
{
    QByteArray data = mimeData->data(QLatin1String("application/x-bluecherry-dvrcamera"));
    QDataStream stream(&data, QIODevice::ReadOnly);

    QList<DVRCamera> re;
    while (!stream.atEnd() && stream.status() == QDataStream::Ok)
    {
        DVRCamera c;
        stream >> c;
        if (c)
            re.append(c);
    }

    return re;
}
