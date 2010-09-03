#include "EventData.h"
#include "core/DVRServer.h"
#include "core/DVRCamera.h"
#include <QApplication>
#include <QDebug>

QString EventLevel::uiString() const
{
    switch (level)
    {
    case Info: return QApplication::translate("EventLevel", "Info");
    case Warning: return QApplication::translate("EventLevel", "Warning");
    case Alarm: return QApplication::translate("EventLevel", "Alarm");
    case Critical: return QApplication::translate("EventLevel", "Critical");
    default: return QApplication::translate("EventLevel", "Unknown");
    }
}

QColor EventLevel::uiColor() const
{
    switch (level)
    {
    case Info: return QColor(122, 122, 122);
    case Warning: return QColor(62, 107, 199);
    case Alarm: return QColor(204, 120, 10);
    case Critical: return QColor(175, 0, 0);
    default: return QColor(Qt::black);
    }
}

EventLevel &EventLevel::operator=(const QString &str)
{
    if (str == QLatin1String("info"))
        level = Info;
    else if (str == QLatin1String("warning"))
        level = Warning;
    else if (str == QLatin1String("alarm"))
        level = Alarm;
    else if (str == QLatin1String("critical"))
        level = Critical;
    else
        level = Info;

    return *this;
}

QString EventType::uiString() const
{
    switch (type)
    {
    case CameraMotion: return QApplication::translate("EventType", "Motion");
    case CameraNotFound: return QApplication::translate("EventType", "Not Found");
    case CameraVideoLost: return QApplication::translate("EventType", "Video Lost");
    case CameraAudioLost: return QApplication::translate("EventType", "Audio Lost");
    case SystemDiskSpace: return QApplication::translate("EventType", "Disk Space");
    case SystemCrash: return QApplication::translate("EventType", "Crash");
    case SystemBoot: return QApplication::translate("EventType", "Startup");
    case SystemShutdown: return QApplication::translate("EventType", "Shutdown");
    case SystemReboot: return QApplication::translate("EventType", "Reboot");
    case SystemPowerOutage: return QApplication::translate("EventType", "Power Lost");
    case UnknownType:
    default:
        return QApplication::translate("EventType", "Unknown");
    }
}

EventType &EventType::operator=(const QString &str)
{
    if (str == QLatin1String("motion"))
        type = CameraMotion;
    else if (str == QLatin1String("not found"))
        type = CameraNotFound;
    else if (str == QLatin1String("video signal loss"))
        type = CameraVideoLost;
    else if (str == QLatin1String("audio signal loss"))
        type = CameraAudioLost;
    else if (str == QLatin1String("disk-space"))
        type = SystemDiskSpace;
    else if (str == QLatin1String("crash"))
        type = SystemCrash;
    else if (str == QLatin1String("boot"))
        type = SystemBoot;
    else if (str == QLatin1String("shutdown"))
        type = SystemShutdown;
    else if (str == QLatin1String("reboot"))
        type = SystemReboot;
    else if (str == QLatin1String("power-outage"))
        type = SystemPowerOutage;
    else
        type = UnknownType;

    return *this;
}

void EventData::setLocation(const QString &location)
{
    if (location.startsWith(QLatin1String("camera-")))
    {
        bool ok = false;
        locationId = location.mid(7).toUInt(&ok);

        if (!ok)
        {
            qWarning() << "Invalid event location" << location;
            locationId = -1;
        }
    }
    else if (location != QLatin1String("system"))
    {
        qWarning() << "Invalid event location" << location;
        locationId = -1;
    }
    else
        locationId = -1;
}

QString EventData::uiServer() const
{
    return server->displayName();
}

DVRCamera *EventData::locationCamera(DVRServer *server, int locationId)
{
    if (locationId >= 0)
        return server->findCamera(locationId);
    return 0;
}

QString EventData::uiLocation(DVRServer *server, int locationId)
{
    DVRCamera *camera = locationCamera(server, locationId);
    if (camera)
        return camera->displayName();
    else if (locationId < 0)
        return QApplication::translate("EventData", "System");
    else
        return QString::fromLatin1("camera-%1").arg(locationId);
}
