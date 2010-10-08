#include "DVRCamera.h"
#include "DVRServer.h"
#include "BluecherryApp.h"
#include "MJpegStream.h"
#include <QXmlStreamReader>

DVRCamera::DVRCamera(DVRServer *s, int id)
    : QObject(s), server(s), uniqueID(id)
{
}

DVRCamera *DVRCamera::findByID(int serverID, int cameraID)
{
    DVRServer *server = bcApp->findServerID(serverID);
    if (!server)
        return 0;

    return server->findCamera(cameraID);
}

DVRCamera *DVRCamera::parseFromXML(DVRServer *server, QXmlStreamReader &xml)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("device"));

    bool ok = false;
    int deviceId = (int)xml.attributes().value(QLatin1String("id")).toString().toUInt(&ok);

    if (!ok)
    {
        xml.raiseError(QLatin1String("Invalid device ID"));
        return 0;
    }

    DVRCamera *camera = new DVRCamera(server, deviceId);

    while (xml.readNext() != QXmlStreamReader::Invalid)
    {
        if (xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == QLatin1String("device"))
            break;
        else if (xml.tokenType() != QXmlStreamReader::StartElement)
            continue;

        if (xml.name() == QLatin1String("name"))
        {
            camera->m_displayName = xml.readElementText();
        }
        else if (xml.name() == QLatin1String("streamUrl"))
        {
            if (xml.attributes().value(QLatin1String("type")) != QLatin1String("mjpeg"))
            {
                xml.skipCurrentElement();
                continue;
            }

            camera->m_streamUrl = server->api->serverUrl().resolved(QUrl(xml.readElementText()))
                                  .toString().toLatin1();
        }
        else
            xml.skipCurrentElement();
    }

    if (camera->m_displayName.isEmpty())
        camera->m_displayName = QString::fromLatin1("#%2").arg(deviceId);

    return camera;
}

QSharedPointer<MJpegStream> DVRCamera::mjpegStream()
{
    QSharedPointer<MJpegStream> re;

    if (m_mjpegStream.isNull())
    {
        if (!m_streamUrl.isEmpty())
        {
            re = QSharedPointer<MJpegStream>(new MJpegStream(QUrl(QString::fromLatin1(m_streamUrl))));
            m_mjpegStream = re;
        }
    }
    else
        re = m_mjpegStream.toStrongRef();

    return re;
}
