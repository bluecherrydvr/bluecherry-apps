#include "DVRCamera.h"
#include "DVRServer.h"

DVRCamera::DVRCamera(DVRServer *s, int id)
    : QObject(s), server(s), uniqueID(id)
{
}

QString DVRCamera::displayName() const
{
    /* This will eventually be known via the server */
    return QString::fromLatin1("%1 No.%2").arg(server->displayName()).arg(uniqueID);
}
