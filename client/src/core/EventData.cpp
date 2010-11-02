#include "EventData.h"
#include "core/DVRServer.h"
#include "core/DVRCamera.h"
#include <QApplication>
#include <QXmlStreamReader>
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
    else if (str == QLatin1String("warn"))
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

DVRCamera EventData::locationCamera(DVRServer *server, int locationId)
{
    if (locationId >= 0)
        return DVRCamera::getCamera(server, locationId);
    return DVRCamera();
}

QString EventData::uiLocation(DVRServer *server, int locationId)
{
    const DVRCamera &camera = locationCamera(server, locationId);
    if (camera)
        return camera.displayName();
    else if (locationId < 0)
        return QApplication::translate("EventData", "System");
    else
        return QString::fromLatin1("camera-%1").arg(locationId);
}

static EventData *parseEntry(DVRServer *server, QXmlStreamReader &reader);

QList<EventData*> EventData::parseEvents(DVRServer *server, const QByteArray &input)
{
    QXmlStreamReader reader(input);
    QList<EventData*> re;

    if (!reader.hasError() && reader.readNextStartElement())
    {
        if (reader.name() == QLatin1String("feed"))
        {
            while (reader.readNext() != QXmlStreamReader::Invalid)
            {
                if (reader.tokenType() == QXmlStreamReader::EndDocument)
                    break;
                if (reader.tokenType() != QXmlStreamReader::StartElement)
                    continue;

                if (reader.name() == QLatin1String("entry"))
                {
                    EventData *ev = parseEntry(server, reader);
                    if (ev)
                        re.append(ev);
                }
            }
        }
        else
            reader.raiseError(QLatin1String("Invalid feed format"));
    }

    if (reader.hasError())
    {
        qWarning() << "EventData::parseEvents error:" << reader.errorString();
    }

    return re;
}

static EventData *parseEntry(DVRServer *server, QXmlStreamReader &reader)
{
    Q_ASSERT(reader.isStartElement() && reader.name() == QLatin1String("entry"));

    EventData *data = new EventData(server);
    data->duration = 0;

    while (reader.readNext() != QXmlStreamReader::Invalid)
    {
        if (reader.tokenType() == QXmlStreamReader::EndElement)
        {
            if (reader.name() == QLatin1String("entry"))
                break;
        }
        if (reader.tokenType() != QXmlStreamReader::StartElement)
            continue;

        if (reader.name() == QLatin1String("id"))
        {
            bool ok = false;
            qint64 id = reader.attributes().value(QLatin1String("raw")).toString().toLongLong(&ok);
            if (!ok || id < 0)
            {
                reader.raiseError(QLatin1String("Invalid format for id element"));
                continue;
            }

            data->eventId = id;
        }
        else if (reader.name() == QLatin1String("published"))
        {
            data->date = QDateTime::fromString(reader.readElementText(), Qt::ISODate);
        }
        else if (reader.name() == QLatin1String("updated"))
        {
            QString d = reader.readElementText();
            if (d.isEmpty())
                data->duration = -1;
            else
                data->duration = data->date.secsTo(QDateTime::fromString(d, Qt::ISODate));
        }
        else if (reader.name() == QLatin1String("content"))
        {
            bool ok = false;
            data->mediaId = reader.attributes().value(QLatin1String("media_id")).toString().toLongLong(&ok);
            if (!ok)
                data->mediaId = -1;
        }
        else if (reader.name() == QLatin1String("category"))
        {
            QXmlStreamAttributes attrib = reader.attributes();
            if (attrib.value(QLatin1String("scheme")) == QLatin1String("http://www.bluecherrydvr.com/atom.html"))
            {
                QStringRef category = attrib.value(QLatin1String("term"));
                QStringList cd = category.toString().split(QLatin1Char('/'));
                if (cd.size() != 3)
                {
                    reader.raiseError(QLatin1String("Invalid format for category element"));
                    continue;
                }

                data->locationId = cd[0].toInt();
                data->level = cd[1];
                data->type = cd[2];
            }
        }
        else if (reader.name() == QLatin1String("entry"))
            reader.raiseError(QLatin1String("Unexpected <entry> element"));
    }

    if (!reader.hasError() && (data->eventId < 0 || !data->date.isValid()))
        reader.raiseError(QLatin1String("Missing required elements for entry"));

    return data;
}
