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
    explicit VideoPlayerBackend(QObject *parent = 0);

    void start(const QUrl &url, QWidget *surface);
    void clear();

    void play();

    gboolean busEvent(GstBus *bus, GstMessage *msg);

private:
    GstElement *m_play;
    GstElement *m_sink;
    GstBus *m_bus;
};

#endif // VIDEOPLAYERBACKEND_GST_H
