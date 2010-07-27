#ifndef BLUECHERRYAPP_H
#define BLUECHERRYAPP_H

#include <QObject>
#include <QList>

class DVRServer;

class BluecherryApp : public QObject
{
    Q_OBJECT

public:
    explicit BluecherryApp();

    QList<DVRServer*> servers() const { return m_servers; }

private:
    QList<DVRServer*> m_servers;
    int m_maxServerId;

    void loadServers();
};

extern BluecherryApp *bcApp;

#endif // BLUECHERRYAPP_H
