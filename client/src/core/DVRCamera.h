#ifndef DVRCAMERA_H
#define DVRCAMERA_H

#include <QObject>
#include <QSharedPointer>
#include <QWeakPointer>
#include <QHash>

class DVRServer;
class MJpegStream;
class QXmlStreamReader;

/* There is one DVRCameraData per server+ID; it is shared among many instances of
 * DVRCamera by reference count. This may be created before we've actually queried
 * the server for cameras (for example, with saved camera layouts in the config).
 * Once real data is available, this object will be updated and dataUpdated will be
 * emitted. The DVRServer holds a reference to every camera that currently exists
 * according to the server. */
class DVRCameraData : public QObject, public QSharedData
{
    Q_OBJECT

    friend class DVRCamera;

public:
    DVRServer * const server;
    const int uniqueID;
    QString displayName;
    QByteArray streamUrl;
    QWeakPointer<MJpegStream> mjpegStream;
    bool isLoaded;

    DVRCameraData(DVRServer *server, int uniqueID);
    virtual ~DVRCameraData();

signals:
    void dataUpdated();

private:
    static QHash<QPair<int,int>,DVRCameraData*> instances;
};

class DVRCamera : public QObject
{
    Q_OBJECT

public:
    static DVRCamera getCamera(int serverID, int cameraID);
    static DVRCamera getCamera(DVRServer *server, int cameraID);

    DVRCamera() { }
    DVRCamera(const DVRCamera &o) : d(o.d) { }

    DVRCamera &operator=(const DVRCamera &o)
    {
        d = o.d;
        return *this;
    }

    bool operator==(const DVRCamera &o) const
    {
        return (d.data() == o.d.data());
    }

    bool isValid() const { return d; }
    operator bool() const { return isValid(); }
    operator QObject*() { return d ? d.data() : 0; }

    DVRServer *server() const { return d ? d->server : 0; }
    int uniqueId() const { return d ? d->uniqueID : -1; }
    QString displayName() const { return d ? d->displayName : QString(); }
    QByteArray streamUrl() const { return d ? d->streamUrl : QByteArray(); }
    bool canStream() const { return d && !d->streamUrl.isEmpty(); }
    QSharedPointer<MJpegStream> mjpegStream();

    bool parseXML(QXmlStreamReader &xml);

private:
    QExplicitlySharedDataPointer<DVRCameraData> d;

    DVRCamera(DVRCameraData *dt) : d(dt) { }
};

#endif // DVRCAMERA_H
