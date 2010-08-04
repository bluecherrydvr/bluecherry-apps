#include "DVRCamera.h"
#include "DVRServer.h"
#include "BluecherryApp.h"
#include "MJpegStream.h"

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

QString DVRCamera::displayName() const
{
    /* This will eventually be known via the server */
    return QString::fromLatin1("%1 No.%2").arg(server->displayName()).arg(uniqueID);
}

QSharedPointer<MJpegStream> DVRCamera::mjpegStream()
{
    QSharedPointer<MJpegStream> re;

    if (m_mjpegStream.isNull())
    {
         /* Test feeds; will be replaced by real camera feeds someday.. */
         QString mjpegTest = server->readSetting("mjpegTest").toString();
         if (!mjpegTest.isEmpty())
         {
             re = QSharedPointer<MJpegStream>(new MJpegStream(QUrl(mjpegTest)));
             m_mjpegStream = re;
         }
    }
    else
        re = m_mjpegStream.toStrongRef();

    return re;
}
