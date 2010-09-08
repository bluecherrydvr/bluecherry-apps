#ifndef EVENTDATA_H
#define EVENTDATA_H

#include <QString>
#include <QDateTime>
#include <QColor>

class DVRServer;
class DVRCamera;

class EventLevel
{
public:
    enum Level
    {
        Info = 0,
        Warning,
        Alarm,
        Critical,
        Minimum = Info
    } level : 8;

    EventLevel() : level(Info) { }
    EventLevel(Level l) : level(l) { }
    EventLevel(const QString &l) { *this = l; }

    QString uiString() const;
    QColor uiColor() const;

    EventLevel &operator=(const QString &l);
    operator Level() const { return level; }
};

class EventType
{
public:
    enum Type
    {
        UnknownType = -1,
        CameraMotion,
        MinCamera = CameraMotion,
        CameraNotFound,
        CameraVideoLost,
        CameraAudioLost,
        MaxCamera = CameraAudioLost,
        SystemDiskSpace,
        MinSystem = SystemDiskSpace,
        SystemCrash,
        SystemBoot,
        SystemShutdown,
        SystemReboot,
        SystemPowerOutage,
        MaxSystem = SystemPowerOutage,
        Max = MaxSystem
    } type : 8;

    EventType() : type(UnknownType) { }
    EventType(Type t) : type(t) { }
    EventType(const QString &str) { *this = str; }

    QString uiString() const;

    EventType &operator=(const QString &str);
    operator int() const { return type; }
    operator Type() const { return type; }
};

struct EventData
{
    QDateTime date;
    DVRServer * const server;
    int duration;
    int locationId;
    EventLevel level;
    EventType type;

    EventData(DVRServer *s)
        : server(s), duration(0), locationId(-1)
    {
    }

    void setLocation(const QString &location);

    bool isSystem() const { return locationId < 0; }
    bool isCamera() const { return locationId >= 0; }
    QDateTime endDate() const { return date.addSecs(duration); }

    QColor uiColor() const { return level.uiColor(); }
    QString uiLevel() const { return level.uiString(); }
    QString uiType() const { return type.uiString(); }
    QString uiServer() const;
    QString uiLocation() const { return uiLocation(server, locationId); }

    DVRCamera *locationCamera() const { return locationCamera(server, locationId); }

    static QString uiLocation(DVRServer *server, int locationId);
    static DVRCamera *locationCamera(DVRServer *server, int locationId);

    static QList<EventData*> parseEvents(DVRServer *server, const QByteArray &input);
};

#endif // EVENTDATA_H
