#ifndef EVENTVIDEOPLAYER_H
#define EVENTVIDEOPLAYER_H

#include <QWidget>
#include <QUrl>
#include <QTimer>

#ifdef USE_GSTREAMER
#include "video/VideoPlayerBackend_gst.h"
#endif

class QToolButton;
class QSlider;
class QLabel;

class EventVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit EventVideoPlayer(QWidget *parent = 0);

public slots:
    void setVideo(const QUrl &url);
    void clearVideo();
    void saveVideo(const QString &path = QString());

    void playPause();
    void seek(int position);
    void restart();

private slots:
    void stateChanged(int state);
    void durationChanged(qint64 duration = -1);
    void updatePosition();

private:
    VideoPlayerBackend backend;
    QWidget *m_videoWidget;
    QToolButton *m_playBtn;
    QSlider *m_seekSlider;
    QLabel *m_posText;
    QTimer m_posTimer;
};

#endif // EVENTVIDEOPLAYER_H
