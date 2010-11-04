#ifndef DVRSERVER_H
#define DVRSERVER_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "ServerRequestManager.h"
#include "DVRCamera.h"

class QNetworkRequest;
class QUrl;
class QSslCertificate;

class DVRServer : public QObject
{
    Q_OBJECT

public:
    ServerRequestManager *api;
    const int configId;

    explicit DVRServer(int configId, QObject *parent = 0);

    const QString &displayName() const { return m_displayName; }

    QString hostname() const;
    int serverPort() const;
    QString username() const;
    QString password() const;

    QList<DVRCamera> cameras() const { return m_cameras; }
    DVRCamera findCamera(int id) { return DVRCamera::getCamera(this, id); }

    /* Settings */
    QVariant readSetting(const QString &key) const;
    QVariant readSetting(const char *key) const { return readSetting(QLatin1String(key)); }

    void writeSetting(const char *key, const QVariant &value) { writeSetting(QLatin1String(key), value); }
    void writeSetting(const QString &key, const QVariant &value);

    void clearSetting(const char *key) { clearSetting(QLatin1String(key)); }
    void clearSetting(const QString &key);

    /* SSL */
    bool isKnownCertificate(const QSslCertificate &certificate) const;
    void setKnownCertificate(const QSslCertificate &certificate);

public slots:
    void setDisplayName(const QString &displayName);
    /* Permanently remove from config and delete */
    void removeServer();

    void login();
    void updateCameras();

signals:
    void changed();
    void serverRemoved(DVRServer *server);

    void cameraAdded(const DVRCamera &camera);
    void cameraRemoved(const DVRCamera &camera);

private slots:
    void updateCamerasReply();
    void disconnected();

private:
    QList<DVRCamera> m_cameras;
    QString m_displayName;
    QTimer m_refreshTimer;
};

Q_DECLARE_METATYPE(DVRServer*)

inline QString DVRServer::hostname() const
{
    return readSetting("hostname").toString();
}

inline int DVRServer::serverPort() const
{
    bool ok = false;
    int r = readSetting("port").toInt(&ok);
    return ok ? r : 7001;
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
