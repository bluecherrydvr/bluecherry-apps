#ifndef VIDEOPLAYERBACKEND_GST_H
#define VIDEOPLAYERBACKEND_GST_H

#include <QObject>
#include <QWidget>
#include <gst/gst.h>
#include "VideoSurface.h"

class QUrl;
class VideoHttpBuffer;

typedef struct _GstDecodeBin GstDecodeBin;

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
    ~VideoPlayerBackend();

    VideoSurface *createSurface();

    void start(const QUrl &url);
    void clear();

    qint64 duration() const;
    qint64 position() const;
    bool isSeekable() const;
    bool atEnd() const { return m_state == Done; }

    VideoState state() const { return m_state; }
    VideoHttpBuffer *videoBuffer() const { return m_videoBuffer; }

    /* Internal */
    GstBusSyncReply busSyncHandler(GstBus *bus, GstMessage *msg);
    void decodePadReady(GstDecodeBin *bin, GstPad *pad, gboolean islast);

public slots:
    void play();
    void pause();
    void seek(qint64 position);
    void restart();

signals:
    void stateChanged(int newState, int oldState);
    void durationChanged(qint64 duration);
    void endOfStream();

private:
    GstElement *m_pipeline, *m_videoLink;
    VideoSurface *m_surface;
    VideoHttpBuffer *m_videoBuffer;
    VideoState m_state;

    bool loadPlugins();
    bool updateVideoSize();
};

#endif // VIDEOPLAYERBACKEND_GST_H
