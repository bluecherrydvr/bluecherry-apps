#ifndef SERVERCONFIGWINDOW_H
#define SERVERCONFIGWINDOW_H

#include <QWidget>

class DVRServer;
class QWebView;

class ServerConfigWindow : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(DVRServer* server READ server WRITE setServer NOTIFY serverChanged)

public:
    explicit ServerConfigWindow(QWidget *parent = 0);
    ~ServerConfigWindow();

    static ServerConfigWindow *instance();

    DVRServer *server() const { return m_server; }

public slots:
    void setServer(DVRServer *server);

signals:
    void serverChanged(DVRServer *server);

private:
    static ServerConfigWindow *m_instance;

    DVRServer *m_server;
    QWebView *m_webView;
};

#endif // SERVERCONFIGWINDOW_H
