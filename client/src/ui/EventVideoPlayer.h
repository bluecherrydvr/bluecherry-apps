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

class EventVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit EventVideoPlayer(QWidget *parent = 0);

public slots:
    void setVideo(const QUrl &url);
    void clearVideo();

    void playPause();
    void seek(int position);

private slots:
    void stateChanged(int state);
    void durationChanged(qint64 duration);
    void updatePosition();

private:
    VideoPlayerBackend backend;
    QWidget *m_videoWidget;
    QToolButton *m_playBtn;
    QSlider *m_seekSlider;
    QTimer m_posTimer;
};

#endif // EVENTVIDEOPLAYER_H
