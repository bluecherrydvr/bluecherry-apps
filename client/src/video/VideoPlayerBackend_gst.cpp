#include "VideoPlayerBackend_gst.h"
#include "VideoHttpBuffer.h"
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/interfaces/xoverlay.h>
#include <glib.h>

#ifdef Q_WS_MAC
#include <QMacCocoaViewContainer>
#endif

VideoPlayerBackend::VideoPlayerBackend(QObject *parent)
    : QObject(parent), m_pipeline(0), m_videoLink(0), m_surface(0), m_state(Stopped)
{
    GError *err;
    if (gst_init_check(0, 0, &err) == FALSE)
    {
        qWarning() << "GStreamer initialization failed:" << err->message;
    }
}

static GstBusSyncReply bus_handler(GstBus *bus, GstMessage *msg, gpointer data)
{
    Q_ASSERT(data);
    return ((VideoPlayerBackend*)data)->busSyncHandler(bus, msg);
}

static void decodePadReadyWrap(GstDecodeBin *bin, GstPad *pad, gboolean islast, gpointer user_data)
{
    Q_ASSERT(user_data);
    static_cast<VideoPlayerBackend*>(user_data)->decodePadReady(bin, pad, islast);
}

class VideoSurface : public QWidget
{
public:
    VideoSurface(QWidget *parent = 0) : QWidget(parent)
    {
    }

    virtual QPaintEngine *paintEngine() const
    {
        return 0;
    }
};

QWidget *VideoPlayerBackend::createSurface()
{
    Q_ASSERT(!m_surface);

#ifdef Q_WS_MAC
    m_surface = new QMacCocoaViewContainer(0);
#else
    m_surface = new VideoSurface;
#endif
    QPalette p = m_surface->palette();
    p.setColor(QPalette::Window, Qt::red);
    m_surface->setPalette(p);
    m_surface->setAutoFillBackground(false);
    m_surface->setAttribute(Qt::WA_NativeWindow);
    m_surface->setAttribute(Qt::WA_PaintOnScreen);
    m_surface->setAttribute(Qt::WA_NoSystemBackground);
    m_surface->setAttribute(Qt::WA_OpaquePaintEvent);

    return m_surface;
}

void VideoPlayerBackend::start(const QUrl &url)
{
    if (m_pipeline)
        clear();

    Q_ASSERT(m_surface);

    m_pipeline = gst_pipeline_new("stream");
    Q_ASSERT(m_pipeline);

    /* Source */
    GstElement *source = gst_element_factory_make("appsrc", "source");
    Q_ASSERT(source);

    VideoHttpBuffer *vhb = new VideoHttpBuffer(GST_APP_SRC(source), m_pipeline, this);
    vhb->start(url);

    /* Decoder */
    GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    g_signal_connect(decoder, "new-decoded-pad", G_CALLBACK(decodePadReadyWrap), this);

    /* Create videoscale and ffmpegcolorspace elements to handle conversion if it's necessary */
    GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", "colorspace");
    Q_ASSERT(colorspace);
    GstElement *scale = gst_element_factory_make("videoscale", "scale");
    Q_ASSERT(scale);

#ifdef Q_WS_X11
    GstElement *sink = gst_element_factory_make("xvimagesink", "sink");
    g_object_set(G_OBJECT(sink), "force-aspect-ratio", TRUE, NULL);
#elif defined(Q_WS_MAC)
    GstElement *sink = gst_element_factory_make("osxvideosink", "sink");
    g_object_set(G_OBJECT(sink), "embed", TRUE, NULL);
#else
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");
#endif
    Q_ASSERT(sink);

    gst_bin_add_many(GST_BIN(m_pipeline), source, decoder, colorspace, scale, sink, NULL);
    gboolean ok = gst_element_link(source, decoder);
    if (!ok)
    {
        qWarning() << "gstreamer: Failed to link source to decodebin";
        return;
    }

    ok = gst_element_link_many(colorspace, scale, sink, NULL);
    if (!ok)
    {
        qWarning() << "gstreamer: Failed to link video output chain";
        return;
    }

    /* This is the element that is linked to the decoder for video output; it will be linked when decodePadReady
     * gives us the video pad. */
    m_videoLink = colorspace;

    /* We handle all messages in the sync handler, because we can't run a glib event loop.
     * Although linux does use glib's loop (and we can take advantage of that), it's better
     * to handle everything this way for windows and mac support. */
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    Q_ASSERT(bus);
    gst_bus_enable_sync_message_emission(bus);
    gst_bus_set_sync_handler(bus, bus_handler, this);
    gst_object_unref(bus);
}

void VideoPlayerBackend::clear()
{
    if (!m_pipeline)
        return;

    gst_element_set_state(m_pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(m_pipeline));

    m_pipeline = m_videoLink = 0;
}

void VideoPlayerBackend::play()
{
    Q_ASSERT(m_pipeline);
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

void VideoPlayerBackend::pause()
{
    Q_ASSERT(m_pipeline);
    gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
}

void VideoPlayerBackend::restart()
{
    Q_ASSERT(m_pipeline);
    gst_element_set_state(m_pipeline, GST_STATE_READY);

    VideoState old = m_state;
    m_state = Stopped;
    emit stateChanged(m_state, old);
}

qint64 VideoPlayerBackend::duration() const
{    
    GstFormat fmt = GST_FORMAT_TIME;
    gint64 re = 0;
    if (gst_element_query_duration(m_pipeline, &fmt, &re))
        return re;

    return -1;
}

qint64 VideoPlayerBackend::position() const
{
    if (m_state == Stopped)
        return 0;
    else if (m_state == Done)
        return duration();

    GstFormat fmt = GST_FORMAT_TIME;
    gint64 re = 0;
    gst_element_query_position(m_pipeline, &fmt, &re);
    return re;
}

void VideoPlayerBackend::seek(qint64 position)
{
    gboolean re = gst_element_seek_simple(m_pipeline, GST_FORMAT_TIME,
                            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                            position);

    if (!re)
        qDebug() << "seek to position" << position << "failed";
}

void VideoPlayerBackend::decodePadReady(GstDecodeBin *bin, GstPad *pad, gboolean islast)
{
    /* TODO: Better cap detection */
    GstCaps *caps = gst_pad_get_caps_reffed(pad);
    Q_ASSERT(caps);
    gchar *capsstr = gst_caps_to_string(caps);
    gst_caps_unref(caps);

    if (QByteArray(capsstr).contains("video"))
    {
        qDebug("creating video");
        gboolean ok = gst_element_link(GST_ELEMENT(bin), m_videoLink);
        if (!ok)
        {
            qWarning() << "gstreamer: Failed to link decodebin to video chain";
            return;
        }
    }

    /* TODO: Audio */

    //free(capsstr);
}

/* Caution: This function is executed on all sorts of strange threads, which should
 * not be excessively delayed, deadlocked, or used for anything GUI-related. Primarily,
 * we want to emit signals (which will result in queued slot calls) or do queued method
 * invocation to handle GUI updates. */
GstBusSyncReply VideoPlayerBackend::busSyncHandler(GstBus *bus, GstMessage *msg)
{
    Q_UNUSED(bus);

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_BUFFERING:
        {
            gint percent = 0;
            gst_message_parse_buffering(msg, &percent);
            qDebug() << "buffering" << percent << "%";
        }
        break;

    case GST_MESSAGE_STATE_CHANGED:
        {
            GstState newState;
            gst_message_parse_state_changed(msg, 0, &newState, 0);
            VideoState vpState;

            switch (newState)
            {
            case GST_STATE_VOID_PENDING:
            case GST_STATE_NULL:
            case GST_STATE_READY:
                vpState = Stopped;
                break;
            case GST_STATE_PAUSED:
                vpState = Paused;
                break;
            case GST_STATE_PLAYING:
                vpState = Playing;
            }

            if (vpState != m_state)
            {
                VideoState old = m_state;
                m_state = vpState;
                emit stateChanged(m_state, old);
            }
        }
        break;

    case GST_MESSAGE_DURATION:
        qDebug("duration");
        emit durationChanged(duration());
        break;

    case GST_MESSAGE_EOS:
        {
            qDebug("end of stream");
            VideoState old = m_state;
            m_state = Done;
            emit stateChanged(m_state, old);
            emit endOfStream();
        }
        break;

    case GST_MESSAGE_ERROR:
        {
            gchar *debug;
            GError *error;

            gst_message_parse_error(msg, &error, &debug);
            qDebug() << "gstreamer: Error:" << error->message;
            qDebug() << "gstreamer: Debug:" << debug;

            g_free(debug);
            g_error_free(error);
        }
        break;

    case GST_MESSAGE_WARNING:
        {
            gchar *debug;
            GError *error;

            gst_message_parse_warning(msg, &error, &debug);
            qDebug() << "gstreamer: Warning:" << error->message;
            qDebug() << "gstreamer:   Debug:" << debug;

            g_free(debug);
            g_error_free(error);
        }
        break;

    case GST_MESSAGE_ELEMENT:
        if (gst_structure_has_name(msg->structure, "prepare-xwindow-id"))
        {
            qDebug("gstreamer: Setting X overlay");
            GstElement *sink = GST_ELEMENT(GST_MESSAGE_SRC(msg));
            gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(sink), (gulong)m_surface->winId());
            gst_x_overlay_expose(GST_X_OVERLAY(sink));
            gst_message_unref(msg);
            return GST_BUS_DROP;
        }
#ifdef Q_WS_MAC
        else if (gst_structure_has_name(msg->structure, "have-ns-view"))
        {
            const GstStructure *structure = gst_message_get_structure(msg);
            void *nsview = g_value_get_pointer(gst_structure_get_value(structure, "nsview"));

            qDebug("gstreamer: Received have-ns-view: %p", nsview);
            bool ok = QMetaObject::invokeMethod(this, "setVideoView", Qt::QueuedConnection,
                                                Q_ARG(void*, nsview));
            Q_ASSERT(ok);
            Q_UNUSED(ok);

            gst_message_unref(msg);
            return GST_BUS_DROP;
        }
#endif
        break;

    default:
        break;
    }

    return GST_BUS_PASS;
}

#ifdef Q_WS_MAC
#include <QApplication>
#include <QResizeEvent>

void VideoPlayerBackend::setVideoView(void *nsview)
{
    qDebug("setting video view");
    Q_ASSERT(qobject_cast<QMacCocoaViewContainer*>(m_surface));
    static_cast<QMacCocoaViewContainer*>(m_surface)->setCocoaView(nsview);

    /* Hack: This is necessary to get QMacCocoaViewContainer to resize the nsview
     * properly when it's initially set. Bug exists as of Qt 4.7.0 */
    QSize sz = m_surface->size();
    m_surface->resize(sz - QSize(1,1));
    m_surface->resize(sz);
}
#endif
