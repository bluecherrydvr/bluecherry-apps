#ifndef VIDEOPLAYERBACKEND_GST_H
#define VIDEOPLAYERBACKEND_GST_H

#include <QObject>
#include <QWidget>
#include <gst/gst.h>

class QUrl;

class VideoPlayerBackend : public QObject
{
    Q_OBJECT

public:
    enum VideoState
    {
        Stopped,
        Playing,
        Paused,
        Done
    };

    explicit VideoPlayerBackend(QObject *parent = 0);

    void start(const QUrl &url, QWidget *surface);
    void clear();

    void play();
    void pause();
    void stop();
    void seek(qint64 position);
    void restart();

    qint64 duration() const;
    qint64 position() const;
    bool atEnd() const { return m_state == Done; }

    VideoState state() const { return m_state; }

    /* Internal */
    gboolean busEvent(GstBus *bus, GstMessage *msg);
    GstBusSyncReply busSyncHandler(GstBus *bus, GstMessage *msg);

signals:
    void stateChanged(int newState, int oldState);
    void durationChanged(qint64 duration);
    void endOfStream();

private:
    GstElement *m_play;
    GstElement *m_sink;
    GstBus *m_bus;
    QWidget *m_surface;
    VideoState m_state;
};

#endif // VIDEOPLAYERBACKEND_GST_H
