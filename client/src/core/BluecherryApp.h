#ifndef BLUECHERRYAPP_H
#define BLUECHERRYAPP_H

#include <QObject>
#include <QList>

class DVRServer;
class QNetworkAccessManager;
class MainWindow;
class QNetworkReply;
class QSslError;

class BluecherryApp : public QObject
{
    Q_OBJECT

public:
    QNetworkAccessManager * const nam;
    MainWindow *mainWindow;

    explicit BluecherryApp();

    QList<DVRServer*> servers() const { return m_servers; }
    DVRServer *addNewServer(const QString &name);
    DVRServer *findServerID(int id);
    bool serverExists(DVRServer *server) { return m_servers.contains(server); }

signals:
    void serverAdded(DVRServer *server);
    void serverRemoved(DVRServer *server);

private slots:
    void onServerRemoved(DVRServer *server);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private:
    QList<DVRServer*> m_servers;
    int m_maxServerId;

    void loadServers();
};

extern BluecherryApp *bcApp;

#endif // BLUECHERRYAPP_H
