#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DVRServersView;
class CameraAreaWidget;
class RecentEventsView;
class QSplitter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showOptionsDialog();
    void showEventsWindow();

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    DVRServersView *m_sourcesList;
    CameraAreaWidget *m_cameraArea;
    RecentEventsView *m_eventsView;
    QSplitter *m_centerSplit;

    void createMenu();

    QWidget *createSourcesList();
    QWidget *createPtzBox();
    QWidget *createServerBox();

    QWidget *createCameraArea();
    QWidget *createCameraControls();
    QWidget *createRecentEvents();
};

#endif // MAINWINDOW_H
