#ifndef DVRCAMERA_H
#define DVRCAMERA_H

#include <QObject>

class DVRServer;

class DVRCamera : public QObject
{
    Q_OBJECT

public:
    DVRServer * const server;
    const int uniqueID;

    static DVRCamera *findByID(int serverID, int cameraID);

    explicit DVRCamera(DVRServer *server, int uniqueID);

    QString displayName() const;
};

#endif // DVRCAMERA_H
