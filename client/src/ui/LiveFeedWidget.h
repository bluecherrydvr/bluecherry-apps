#ifndef LIVEFEEDWIDGET_H
#define LIVEFEEDWIDGET_H

#include <QWidget>

class DVRCamera;
class QMimeData;

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

private slots:
    void updateFrame(const QPixmap &frame);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void paintEvent(QPaintEvent *event);

private:
    DVRCamera *m_camera, *m_dragCamera;
    QPixmap m_currentFrame;

    DVRCamera *cameraFromMime(const QMimeData *mimeData);
};

#endif // LIVEFEEDWIDGET_H
