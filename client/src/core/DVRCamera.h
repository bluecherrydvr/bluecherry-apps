#ifndef DVRCAMERA_H
#define DVRCAMERA_H

#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>

class DVRServer;
class MJpegStream;
class QXmlStreamReader;

class DVRCamera : public QObject
{
    Q_OBJECT

public:
    DVRServer * const server;
    const int uniqueID;

    static DVRCamera *findByID(int serverID, int cameraID);
    static DVRCamera *parseFromXML(DVRServer *server, QXmlStreamReader &xml);

    explicit DVRCamera(DVRServer *server, int uniqueID);

    QString displayName() const { return m_displayName; }
    QByteArray streamUrl() const { return m_streamUrl; }

    bool canStream() const { return !m_streamUrl.isEmpty(); }

    QSharedPointer<MJpegStream> mjpegStream();

private:
    QString m_displayName;
    QByteArray m_streamUrl;
    QWeakPointer<MJpegStream> m_mjpegStream;
};

#endif // DVRCAMERA_H
