#ifndef DVRSERVER_H
#define DVRSERVER_H

#include <QObject>
#include <QVariant>

class DVRCamera;
class QNetworkRequest;
class QUrl;

class DVRServer : public QObject
{
    Q_OBJECT

public:
    const int configId;

    explicit DVRServer(int configId, QObject *parent = 0);

    const QString &displayName() const { return m_displayName; }

    QString hostname() const;
    QString username() const;
    QString password() const;

    QList<DVRCamera*> cameras() const { return m_cameras; }
    DVRCamera *findCamera(int id) const;

    /* Settings */
    QVariant readSetting(const QString &key) const;
    QVariant readSetting(const char *key) const { return readSetting(QLatin1String(key)); }

    void writeSetting(const char *key, const QVariant &value) { writeSetting(QLatin1String(key), value); }
    void writeSetting(const QString &key, const QVariant &value);

    void clearSetting(const char *key) { clearSetting(QLatin1String(key)); }
    void clearSetting(const QString &key);

    /* Network */
    QNetworkRequest createRequest(const QUrl &url);

public slots:
    void setDisplayName(const QString &displayName);
    /* Permanently remove from config and delete */
    void removeServer();

signals:
    void changed();
    void serverRemoved(DVRServer *server);

private:
    QList<DVRCamera*> m_cameras;
    QString m_displayName;
};

Q_DECLARE_METATYPE(DVRServer*)

inline QString DVRServer::hostname() const
{
    return readSetting("hostname").toString();
}

inline QString DVRServer::username() const
{
    return readSetting("username").toString();
}

inline QString DVRServer::password() const
{
    return readSetting("password").toString();
}

#endif // DVRSERVER_H
