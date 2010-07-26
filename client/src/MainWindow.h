#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QTreeView;
class CameraAreaWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QTreeView *m_sourcesList;
    CameraAreaWidget *m_cameraArea;

    QWidget *createSourcesList();
    QWidget *createPtzBox();
    QWidget *createServerBox();

    QWidget *createCameraArea();
    QWidget *createCameraControls();
};

#endif // MAINWINDOW_H
