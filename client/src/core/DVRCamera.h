#ifndef DVRCAMERA_H
#define DVRCAMERA_H

#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>

class DVRServer;
class MJpegStream;

class DVRCamera : public QObject
{
    Q_OBJECT

public:
    DVRServer * const server;
    const int uniqueID;

    static DVRCamera *findByID(int serverID, int cameraID);

    explicit DVRCamera(DVRServer *server, int uniqueID);

    QString displayName() const;
    QSharedPointer<MJpegStream> mjpegStream();

private:
    QWeakPointer<MJpegStream> m_mjpegStream;
};

#endif // DVRCAMERA_H
