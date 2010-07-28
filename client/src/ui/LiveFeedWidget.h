#ifndef LIVEFEEDWIDGET_H
#define LIVEFEEDWIDGET_H

#include <QWidget>

class DVRCamera;

class LiveFeedWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(DVRCamera* camera READ camera WRITE setCamera NOTIFY cameraChanged)

public:
    explicit LiveFeedWidget(QWidget *parent = 0);

    DVRCamera *camera() const { return m_camera; }

    virtual QSize sizeHint() const;

public slots:
    void setCamera(DVRCamera *camera);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    DVRCamera *m_camera;
};

#endif // LIVEFEEDWIDGET_H
