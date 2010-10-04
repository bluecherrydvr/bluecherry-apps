#ifndef LIVEFEEDWIDGET_H
#define LIVEFEEDWIDGET_H

#include <QWidget>
#include <QSharedPointer>

class DVRCamera;
class MJpegStream;
class QMimeData;

class LiveFeedWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(DVRCamera* camera READ camera WRITE setCamera)

public:
    explicit LiveFeedWidget(QWidget *parent = 0);
    virtual ~LiveFeedWidget();

    DVRCamera *camera() const { return m_camera; }
    QString statusMessage() const { return m_statusMsg; }

    virtual QSize sizeHint() const;

public slots:
    void setCamera(DVRCamera *camera);

    QWidget *openWindow();
    void closeCamera();
    void setFullScreen(bool on = true);
    void toggleFullScreen() { setFullScreen(!isFullScreen()); }

    void saveSnapshot(const QString &file = QString());

private slots:
    void updateFrame(const QPixmap &frame, const QVector<QImage> &scaledFrames);
    void addScaleSize(QVector<QSize> &sizes);
    void mjpegStateChanged(int state);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragLeaveEvent(QDragLeaveEvent *event);
    virtual void dropEvent(QDropEvent *event);
    virtual void paintEvent(QPaintEvent *event);
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    DVRCamera *m_camera, *m_dragCamera;
    QSharedPointer<MJpegStream> m_stream;
    QPixmap m_currentFrame;
    QString m_statusMsg;

    DVRCamera *cameraFromMime(const QMimeData *mimeData);

    void setStatusMessage(const QString &message);
    void clearStatusMessage() { setStatusMessage(QString()); }

    void clone(LiveFeedWidget *other);
};

class QDataStream;

QDataStream &operator<<(QDataStream &stream, const LiveFeedWidget &widget);
QDataStream &operator>>(QDataStream &stream, LiveFeedWidget &widget);

#endif // LIVEFEEDWIDGET_H
