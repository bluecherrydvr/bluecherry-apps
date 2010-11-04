#include "BluecherryApp.h"
#include "DVRServer.h"
#include <QSettings>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QHostAddress>
#include <QTimer>
#include <QFile>
#include <QSslError>
#include <QSslConfiguration>
#include <QSslCertificate>

BluecherryApp *bcApp = 0;

BluecherryApp::BluecherryApp()
    : nam(new QNetworkAccessManager(this)), m_maxServerId(-1)
{
    Q_ASSERT(!bcApp);
    bcApp = this;

    connect(nam, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(sslErrors(QNetworkReply*,QList<QSslError>)));

    /* Don't use the system CAs to verify certificates */
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setCaCertificates(QList<QSslCertificate>());
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    loadServers();
}

void BluecherryApp::loadServers()
{
    Q_ASSERT(m_servers.isEmpty());

    QSettings settings;
    settings.beginGroup(QLatin1String("servers"));
    QStringList groups = settings.childGroups();

    foreach (QString group, groups)
    {
        bool ok = false;
        int id = (int)group.toUInt(&ok);
        if (!ok)
        {
            qWarning("Ignoring invalid server ID from configuration");
            continue;
        }

        DVRServer *server = new DVRServer(id, this);
        connect(server, SIGNAL(serverRemoved(DVRServer*)), SLOT(onServerRemoved(DVRServer*)));

        m_servers.append(server);
        m_maxServerId = qMax(m_maxServerId, id);
    }

#ifdef Q_OS_LINUX
    /* If there are no servers configured, and the server application is installed here, automatically
     * configure a local server. */
    if (groups.isEmpty() && QFile::exists(QLatin1String("/etc/bluecherry.conf")))
    {
        DVRServer *s = addNewServer(tr("Local"));
        s->writeSetting("hostname", QHostAddress(QHostAddress::LocalHost).toString());
        /* This must match the default username and password for the server */
        s->writeSetting("username", QLatin1String("admin"));
        s->writeSetting("password", QLatin1String("bluecherry"));
        QTimer::singleShot(0, s, SLOT(login()));
    }
#endif
}

DVRServer *BluecherryApp::addNewServer(const QString &name)
{
    int id = ++m_maxServerId;

    QSettings settings;
    settings.setValue(QString::fromLatin1("servers/%1/displayName").arg(id), name);

    DVRServer *server = new DVRServer(id, this);
    m_servers.append(server);
    connect(server, SIGNAL(serverRemoved(DVRServer*)), SLOT(onServerRemoved(DVRServer*)));

    emit serverAdded(server);
    return server;
}

DVRServer *BluecherryApp::findServerID(int id)
{
    for (QList<DVRServer*>::Iterator it = m_servers.begin(); it != m_servers.end(); ++it)
    {
        if ((*it)->configId == id)
            return *it;
    }

    return 0;
}

void BluecherryApp::onServerRemoved(DVRServer *server)
{
    if (m_servers.removeOne(server))
        emit serverRemoved(server);
}

void BluecherryApp::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    foreach (const QSslError &err, errors)
    {
        switch (err.error())
        {
        case QSslError::CertificateNotYetValid:
        case QSslError::CertificateExpired:
            qDebug() << "BluecherryApp: Ignoring expired SSL certificate";
            break;
        case QSslError::SelfSignedCertificate:
        case QSslError::SelfSignedCertificateInChain:
        case QSslError::HostNameMismatch:
            break;
        default:
            qWarning() << "SSL Error:" << err.errorString() <<(int) err.error();
            return;
        }
    }

    QSslCertificate cert = reply->sslConfiguration().peerCertificate();
    if (cert.isNull())
    {
        qWarning() << "SSL Error: No certificate from peer";
        return;
    }

    /* Figure out which server is associated with this request */
    DVRServer *server = reply->property("DVRServer").value<DVRServer*>();
    if (!server)
    {
        server = qobject_cast<DVRServer*>(reply->request().originatingObject());
        if (!server)
        {
            QUrl requestUrl = reply->request().url();
            foreach (DVRServer *s, servers())
            {
                QUrl serverUrl = s->api->serverUrl();
                if (QString::compare(serverUrl.authority(), requestUrl.authority(), Qt::CaseInsensitive) == 0
                    && QString::compare(serverUrl.scheme(), requestUrl.scheme(), Qt::CaseInsensitive) == 0)
                {
                    server = s;
                    break;
                }
            }

            if (!server)
            {
                qWarning() << "BluecherryApp: Unable to determine server associated with request for" << requestUrl;
                return;
            }
        }
    }

    if (!server->isKnownCertificate(cert))
    {
        qDebug("BluecherryApp: Prompting user to accept different SSL certificate");
        emit sslConfirmRequired(server, errors, reply->sslConfiguration());
        /* If the user accepted, this should now be a known certificate */
        if (!server->isKnownCertificate(reply->sslConfiguration().peerCertificate()))
            return;
    }

    reply->ignoreSslErrors();
}
