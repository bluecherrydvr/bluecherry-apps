#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DVRServersView;
class CameraAreaWidget;
class RecentEventsView;
class QSplitter;
class DVRServer;
class QSslError;
class QSslConfiguration;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    CameraAreaWidget *cameraArea() const { return m_cameraArea; }

public slots:
    void showOptionsDialog();
    void showEventsWindow();

    void addServer();
    void openServerConfig();
    void editCurrentServer();
    void refreshServerDevices();

    void openDocumentation();
    void openSupport();
    void openIdeas();
    void openAbout();

private slots:
    void showServersMenu();
    void sslConfirmRequired(DVRServer *server, const QList<QSslError> &errors, const QSslConfiguration &config);

signals:
    void closing();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    DVRServersView *m_sourcesList;
    CameraAreaWidget *m_cameraArea;
    RecentEventsView *m_eventsView;
    QSplitter *m_centerSplit;
    QAction *menuServerName;

    void createMenu();

    QWidget *createSourcesList();
    QWidget *createServerBox();

    QWidget *createCameraArea();
    QWidget *createCameraControls();
    QWidget *createRecentEvents();
};

#endif // MAINWINDOW_H
