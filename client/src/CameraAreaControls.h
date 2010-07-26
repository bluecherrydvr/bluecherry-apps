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

private:
    QComboBox *m_savedLayouts;
    QPushButton *m_gridButtons[6];
};

#endif // CAMERAAREACONTROLS_H
