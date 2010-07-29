#include "DVRCamera.h"
#include "DVRServer.h"
#include "BluecherryApp.h"

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
