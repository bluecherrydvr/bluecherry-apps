#include "VideoPlayerBackend_gst.h"
#include <QUrl>
#include <QDebug>
#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>

VideoPlayerBackend::VideoPlayerBackend(QObject *parent)
    : QObject(parent), m_play(0), m_sink(0), m_bus(0)
{
    GError *err;
    if (gst_init_check(0, 0, &err) == FALSE)
    {
        qWarning() << "GStreamer initialization failed:" << err->message;
    }
}

static gboolean bus_callback(GstBus *bus, GstMessage *msg, gpointer data)
{
    return ((VideoPlayerBackend*)data)->busEvent(bus, msg);
}

void VideoPlayerBackend::start(const QUrl &url, QWidget *surface)
{
    if (m_play || m_sink || m_bus)
        clear();

    m_play = gst_element_factory_make("playbin", "play");
    g_object_set(G_OBJECT(m_play), "uri", url.toString().toLocal8Bit().constData(), NULL);

    m_sink = gst_element_factory_make("xvimagesink", "sink");
    g_object_set(G_OBJECT(m_play), "video-sink", m_sink, NULL);

    surface->setAttribute(Qt::WA_NativeWindow);
    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(m_sink), surface->winId());

    m_bus = gst_pipeline_get_bus(GST_PIPELINE(m_play));
    gst_bus_add_watch(m_bus, bus_callback, this);
    gst_object_unref(m_bus);
}

void VideoPlayerBackend::clear()
{
    if (!m_play)
        return;

    gst_element_set_state(m_play, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(m_play));

    m_play = m_sink = 0;
    m_bus = 0;
}

void VideoPlayerBackend::play()
{
    Q_ASSERT(m_play);
    gst_element_set_state(m_play, GST_STATE_PLAYING);
}

gboolean VideoPlayerBackend::busEvent(GstBus *bus, GstMessage *msg)
{
    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_EOS:
        qDebug("end of stream");
        break;

    case GST_MESSAGE_ERROR:
        {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            g_free(debug);

            g_printerr("Error: %s\n", error->message);
            g_error_free(error);
            break;
        }

    default:
        break;
    }

    return TRUE;
}
