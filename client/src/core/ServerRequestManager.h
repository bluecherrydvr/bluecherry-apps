#ifndef SERVERREQUESTMANAGER_H
#define SERVERREQUESTMANAGER_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>

class DVRServer;

class ServerRequestManager : public QObject
{
    Q_OBJECT

public:
    enum Status
    {
        LoginError = -2,
        ServerError = -1,
        Offline,
        Online
    };

    DVRServer * const server;

    explicit ServerRequestManager(DVRServer *server);

    bool isOnline() const { return m_status == Online; }
    Status status() const { return m_status; }
    QString errorMessage() const { return m_errorMessage; }

    void login(const QString &username, const QString &password);

    QUrl serverUrl() const;
    QNetworkRequest buildRequest(const QUrl &relativeUrl);
    QNetworkRequest buildRequest(const QString &relativeUrl) { return buildRequest(QUrl(relativeUrl)); }
    QNetworkReply *sendRequest(const QUrl &relativeUrl);

signals:
    void loginSuccessful();
    void serverError(const QString &message);
    void loginError(const QString &message);
    void statusChanged(int status);

private slots:
    void loginReplyReady();

private:
    QString m_errorMessage;
    QNetworkReply *m_loginReply;
    Status m_status;

    void setStatus(Status status, const QString &errorMessage = QString());
};

#endif // SERVERREQUESTMANAGER_H
