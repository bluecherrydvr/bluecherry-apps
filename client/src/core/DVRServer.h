#ifndef DVRSERVER_H
#define DVRSERVER_H

#include <QObject>
#include <QVariant>

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

    QVariant readSetting(const QString &key) const;
    QVariant readSetting(const char *key) const { return readSetting(QLatin1String(key)); }

    void writeSetting(const char *key, const QVariant &value) { writeSetting(QLatin1String(key), value); }
    void writeSetting(const QString &key, const QVariant &value);

    void clearSetting(const char *key) { clearSetting(QLatin1String(key)); }
    void clearSetting(const QString &key);

public slots:
    void setDisplayName(const QString &displayName);

signals:
    void changed();

private:
    QString m_displayName;
};

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
