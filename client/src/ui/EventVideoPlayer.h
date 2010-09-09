#ifndef EVENTVIDEOPLAYER_H
#define EVENTVIDEOPLAYER_H

#include <QWidget>
#include <QUrl>

#ifdef USE_GSTREAMER
#include "video/VideoPlayerBackend_gst.h"
#endif

class EventVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit EventVideoPlayer(QWidget *parent = 0);

public slots:
    void setVideo(const QUrl &url);
    void clearVideo();

    void play();

private:
    VideoPlayerBackend backend;
    QWidget *m_videoWidget;
};

#endif // EVENTVIDEOPLAYER_H
