#include "ServerRequestManager.h"
#include "DVRServer.h"
#include "BluecherryApp.h"
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QDebug>

ServerRequestManager::ServerRequestManager(DVRServer *s)
    : QObject(s), server(s), m_loginReply(0), m_status(Offline)
{
}

void ServerRequestManager::setStatus(Status s, const QString &errmsg)
{
    if (s == m_status && (errmsg.isEmpty() || errmsg == m_errorMessage))
        return;

    m_status = s;
    m_errorMessage = errmsg;

    qDebug() << "ServerRequestManager" << server->displayName() << ": Status" << m_status << "message" << m_errorMessage;

    emit statusChanged(m_status);

    switch (m_status)
    {
    case LoginError:
        emit loginError(m_errorMessage);
        break;
    case ServerError:
        emit serverError(m_errorMessage);
        break;
    case Online:
        emit loginSuccessful();
        break;
    default:
        break;
    }
}

QUrl ServerRequestManager::serverUrl() const
{
    QUrl url;
    url.setScheme(QLatin1String("http"));
    url.setHost(server->hostname());
    url.setPort(server->serverPort());
    return url;
}

QNetworkRequest ServerRequestManager::buildRequest(const QUrl &relativeUrl)
{
    Q_ASSERT(relativeUrl.isRelative());
    QUrl url = relativeUrl;
    url.setScheme(QLatin1String("http"));
    url.setHost(server->hostname());
    url.setPort(server->serverPort());

    return QNetworkRequest(url);
}

QNetworkReply *ServerRequestManager::sendRequest(const QUrl &relativeUrl)
{
    return bcApp->nam->get(buildRequest(relativeUrl));
}

void ServerRequestManager::login(const QString &username, const QString &password)
{
    if (m_loginReply)
    {
        m_loginReply->disconnect(this);
        m_loginReply->abort();
        m_loginReply->deleteLater();
        m_loginReply = 0;
    }

    QNetworkRequest req = buildRequest(QLatin1String("/ajax/login.php"));

    QUrl queryData;
    queryData.addQueryItem(QLatin1String("login"), username);
    queryData.addQueryItem(QLatin1String("password"), password);

    emit loginRequestStarted();

    m_loginReply = bcApp->nam->post(req, queryData.encodedQuery());
    connect(m_loginReply, SIGNAL(finished()), SLOT(loginReplyReady()));
}

void ServerRequestManager::loginReplyReady()
{
    if (!m_loginReply || sender() != m_loginReply)
        return;

    QNetworkReply *reply = m_loginReply;
    m_loginReply->deleteLater();
    m_loginReply = 0;

    if (reply->error() != QNetworkReply::NoError)
    {
        setStatus(ServerError, tr("Request failed: %1").arg(reply->errorString()));
        return;
    }

    QByteArray data = reply->readAll();
    if (!data.startsWith("OK"))
    {
        setStatus(LoginError, data.isEmpty() ? tr("Unknown error") : QString::fromUtf8(data));
        return;
    }

    if (bcApp->nam->cookieJar()->cookiesForUrl(reply->url()).isEmpty())
    {
        setStatus(LoginError, QLatin1String("No authentication data provided by server"));
        return;
    }

    setStatus(Online);
}
