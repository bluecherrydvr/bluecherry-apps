#ifndef CAMERAAREACONTROLS_H
#define CAMERAAREACONTROLS_H

#include <QWidget>

class CameraAreaWidget;
class QComboBox;
class QPushButton;

class CameraAreaControls : public QWidget
{
    Q_OBJECT

public:
    CameraAreaWidget * const cameraArea;

    explicit CameraAreaControls(CameraAreaWidget *cameraArea, QWidget *parent = 0);

public slots:
    void saveLayout();

private slots:
    void savedLayoutChanged(int index);

private:
    QComboBox *m_savedLayouts;
    int m_lastLayoutIndex;
};

#endif // CAMERAAREACONTROLS_H
