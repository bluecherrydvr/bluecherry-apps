#ifndef CAMERAAREACONTROLS_H
#define CAMERAAREACONTROLS_H

#include <QToolBar>

class CameraAreaWidget;
class QComboBox;
class QPushButton;

class CameraAreaControls : public QToolBar
{
    Q_OBJECT

public:
    CameraAreaWidget * const cameraArea;

    explicit CameraAreaControls(CameraAreaWidget *cameraArea, QWidget *parent = 0);

public slots:
    void saveLayout();

private slots:
    void savedLayoutChanged(int index);
    void showLayoutMenu(const QPoint &pos, int index = -1);

private:
    QComboBox *m_savedLayouts;
    int m_lastLayoutIndex;
};

#endif // CAMERAAREACONTROLS_H
