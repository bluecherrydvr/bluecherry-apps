#include "VideoPlayerBackend_gst.h"
#include "VideoHttpBuffer.h"
#include <QUrl>
#include <QDebug>
#include <QApplication>
#include <QDir>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/video/video.h>
#include <gst/interfaces/xoverlay.h>
#include <glib.h>

#if defined(Q_OS_MAC) && !defined(QT_MAC_USE_COCOA)
/* GStreamer's osxvideosink and glimagesink both require Cocoa views (NSView)
 * to render. As a result, we can't have a Carbon build of Qt and render the video
 * properly. */
#error GStreamer requires a Cocoa-based build of Qt
#endif

VideoPlayerBackend::VideoPlayerBackend(QObject *parent)
    : QObject(parent), m_pipeline(0), m_videoLink(0), m_surface(0), m_videoBuffer(0), m_state(Stopped), m_sinkReady(false)
{
    GError *err;
    if (gst_init_check(0, 0, &err) == FALSE)
    {
        qWarning() << "GStreamer initialization failed:" << err->message;
        setError(true, tr("Failed to initialize video playback: %1").arg(QString::fromLatin1(err->message)));
        return;
    }

#if !defined(Q_OS_LINUX) || defined(GSTREAMER_PLUGINS)
    if (!loadPlugins())
        setError(true, tr("Failed to load video plugins"));
#endif
}

VideoPlayerBackend::~VideoPlayerBackend()
{
    if (m_videoBuffer)
        m_videoBuffer->clearPlayback();
    clear();
}

bool VideoPlayerBackend::loadPlugins()
{
#ifdef Q_OS_WIN
#define EXT ".dll"
#else
#define EXT ".so"
#endif

    const char *plugins[] =
    {
        "libgsttypefindfunctions"EXT, "libgstapp"EXT, "libgstdecodebin"EXT, "libgstmatroska"EXT,
        "libgstvideoscale"EXT, "libgstffmpegcolorspace"EXT, "libgstcoreelements"EXT,
#ifndef Q_OS_WIN
        "libgstffmpeg"EXT,
#endif
#ifdef Q_OS_WIN
        "libgstffmpeg-lgpl"EXT, "libgstautodetect"EXT, "libgstdshowvideosink"EXT, "libgstdirectsound"EXT,
#elif defined(Q_OS_MAC)
        "libgstosxaudio"EXT, "libgstosxvideosink"EXT,
#else
        "libgstxvimagesink"EXT,
#endif
        0
    };

#undef EXT
#if defined(Q_OS_MAC)
    QByteArray pluginPath = QFile::encodeName(QApplication::applicationDirPath() + QLatin1String("/../PlugIns/gstreamer/"));
#else
    QByteArray pluginPath = QFile::encodeName(QDir::toNativeSeparators(QApplication::applicationDirPath() +
                                                                       QLatin1String("/")));
#endif

#ifdef GSTREAMER_PLUGINS
    QString ppx = QDir::toNativeSeparators(QString::fromLatin1(GSTREAMER_PLUGINS "/"));
    if (QDir::isAbsolutePath(ppx))
        pluginPath = QFile::encodeName(ppx);
    else
        pluginPath = pluginPath + QFile::encodeName(QString(QDir::separator())) + QFile::encodeName(ppx);
#endif

    if (!QFile::exists(QFile::decodeName(pluginPath)))
    {
        qWarning() << "gstreamer: Plugin path" << pluginPath << "does not exist";
        return false;
    }

    bool success = true;

    for (const char **p = plugins; *p; ++p)
    {
        GError *err = 0;
        GstPlugin *plugin = gst_plugin_load_file(pluginPath + QByteArray::fromRawData(*p, qstrlen(*p)), &err);
        if (!plugin)
        {
            Q_ASSERT(err);
            qWarning() << "gstreamer: Failed to load plugin" << *p << ":" << err->message;
            g_error_free(err);
            success = false;
        }
        else
        {
            Q_ASSERT(!err);
            gst_object_unref(plugin);
        }
    }

    return success;
}

static GstBusSyncReply bus_handler(GstBus *bus, GstMessage *msg, gpointer data)
{
    Q_ASSERT(data);
    return ((VideoPlayerBackend*)data)->busHandler(bus, msg, true);
}

static gboolean async_bus_handler(GstBus *bus, GstMessage *msg, gpointer data)
{
    Q_ASSERT(data);
    ((VideoPlayerBackend*)data)->busHandler(bus, msg, false);
    return TRUE;
}

static void decodePadReadyWrap(GstDecodeBin *bin, GstPad *pad, gboolean islast, gpointer user_data)
{
    Q_ASSERT(user_data);
    static_cast<VideoPlayerBackend*>(user_data)->decodePadReady(bin, pad, islast);
}

VideoSurface *VideoPlayerBackend::createSurface()
{
    Q_ASSERT(!m_surface);

    m_surface = new VideoSurface;
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

bool VideoPlayerBackend::start(const QUrl &url)
{
    if (m_pipeline)
        clear();

    Q_ASSERT(m_surface);

    /* Pipeline */
    m_pipeline = gst_pipeline_new("stream");
    if (!m_pipeline)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("stream")));
        return false;
    }

    /* Source element */
    GstElement *source = gst_element_factory_make("appsrc", "source");
    if (!source)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("source")));
        return false;
    }

    /* Buffered HTTP source (using source) */
    m_videoBuffer = new VideoHttpBuffer(GST_APP_SRC(source), m_pipeline, this);
    m_videoBuffer->start(url);

    /* Decoder */
    GstElement *decoder = gst_element_factory_make("decodebin", "decoder");
    if (!decoder)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("decoder")));
        return false;
    }
    g_signal_connect(decoder, "new-decoded-pad", G_CALLBACK(decodePadReadyWrap), this);

    /* Colorspace conversion (no-op if unnecessary) */
    GstElement *colorspace = gst_element_factory_make("ffmpegcolorspace", "colorspace");
    if (!colorspace)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("colorspace")));
        return false;
    }

    /* Video scaling */
    GstElement *scale = gst_element_factory_make("videoscale", "scale");
    if (!scale)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("scale")));
        return false;
    }

#ifdef Q_WS_MAC
    GstElement *sink = gst_element_factory_make("osxvideosink", "sink");
    if (sink)
        g_object_set(G_OBJECT(sink), "embed", TRUE, NULL);
#else
    GstElement *sink = gst_element_factory_make("autovideosink", "sink");
#endif
    if (!sink)
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("sink")));
        return false;
    }

    m_sinkReady = false;

    gst_bin_add_many(GST_BIN(m_pipeline), source, decoder, colorspace, scale, sink, NULL);
    if (!gst_element_link_many(source, decoder, NULL))
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("link decoder")));
        return false;
    }

    if (!gst_element_link_many(colorspace, scale, sink, NULL))
    {
        setError(true, tr("Failed to create video pipeline (%1)").arg(QLatin1String("link sink")));
        return false;
    }

    /* This is the element that is linked to the decoder for video output; it will be linked when decodePadReady
     * gives us the video pad. */
    m_videoLink = colorspace;

    /* We handle all messages in the sync handler, because we can't run a glib event loop.
     * Although linux does use glib's loop (and we could take advantage of that), it's better
     * to handle everything this way for windows and mac support. */
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(m_pipeline));
    Q_ASSERT(bus);
    gst_bus_enable_sync_message_emission(bus);
    gst_bus_set_sync_handler(bus, bus_handler, this);
#ifdef Q_OS_LINUX
    gst_bus_add_watch(bus, async_bus_handler, this);
#endif
    gst_object_unref(bus);

    /* When VideoHttpBuffer has buffered a reasonable amount of data to facilitate detection and such, it will
     * move the pipeline into the PAUSED state, which should set everything else up. */
    return true;
}

void VideoPlayerBackend::clear()
{
    if (m_pipeline)
    {
        gst_element_set_state(m_pipeline, GST_STATE_NULL);
        /* Ensure the transition to NULL completes */
        gst_element_get_state(m_pipeline, 0, 0, GST_CLOCK_TIME_NONE);
        gst_object_unref(GST_OBJECT(m_pipeline));
    }

    m_pipeline = m_videoLink = 0;

    if (m_videoBuffer)
    {
        if (m_videoBuffer->parent() == this)
            m_videoBuffer->deleteLater();
        m_videoBuffer = 0;
    }

    m_state = Stopped;
    m_errorMessage.clear();
    m_sinkReady = false;
}

void VideoPlayerBackend::setError(bool permanent, const QString &message)
{
    VideoState old = m_state;
    m_state = permanent ? PermanentError : Error;
    m_errorMessage = message;
    emit stateChanged(m_state, old);
}

void VideoPlayerBackend::play()
{
    if (!m_pipeline)
        return;
    gst_element_set_state(m_pipeline, GST_STATE_PLAYING);
}

void VideoPlayerBackend::pause()
{
    if (!m_pipeline)
        return;
    gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
}

void VideoPlayerBackend::restart()
{
    if (!m_pipeline)
        return;
    gst_element_set_state(m_pipeline, GST_STATE_READY);

    VideoState old = m_state;
    m_state = Stopped;
    emit stateChanged(m_state, old);
}

qint64 VideoPlayerBackend::duration() const
{
    if (m_pipeline)
    {
        GstFormat fmt = GST_FORMAT_TIME;
        gint64 re = 0;
        if (gst_element_query_duration(m_pipeline, &fmt, &re))
            return re;
    }

    return -1;
}

qint64 VideoPlayerBackend::position() const
{
    if (!m_pipeline)
        return -1;
    else if (m_state == Stopped)
        return 0;
    else if (m_state == Done)
        return duration();

    GstFormat fmt = GST_FORMAT_TIME;
    gint64 re = 0;
    if (!gst_element_query_position(m_pipeline, &fmt, &re))
        re = -1;
    return re;
}

bool VideoPlayerBackend::isSeekable() const
{
    if (!m_pipeline)
        return false;

    GstQuery *query = gst_query_new_seeking(GST_FORMAT_TIME);
    gboolean re = gst_element_query(m_pipeline, query);
    if (re)
    {
        gboolean seekable;
        gst_query_parse_seeking(query, 0, &seekable, 0, 0);
        re = seekable;
    }
    else
        qDebug() << "gstreamer: Failed to query seeking properties of the stream";

    gst_query_unref(query);
    return re;
}

bool VideoPlayerBackend::seek(qint64 position)
{
    if (!m_pipeline)
        return false;

    gboolean re = gst_element_seek_simple(m_pipeline, GST_FORMAT_TIME,
                            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                            position);

    if (!re)
        qDebug() << "gstreamer: seek to position" << position << "failed";

    return re ? true : false;
}

void VideoPlayerBackend::decodePadReady(GstDecodeBin *bin, GstPad *pad, gboolean islast)
{
    Q_UNUSED(islast);

    /* TODO: Better cap detection */
    GstCaps *caps = gst_pad_get_caps_reffed(pad);
    Q_ASSERT(caps);
    gchar *capsstr = gst_caps_to_string(caps);
    gst_caps_unref(caps);

    if (QByteArray(capsstr).contains("video"))
    {
        qDebug("gstreamer: linking video decoder to pipeline");
        if (!gst_element_link(GST_ELEMENT(bin), m_videoLink))
        {
            setError(false, tr("Building video pipeline failed"));
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
GstBusSyncReply VideoPlayerBackend::busHandler(GstBus *bus, GstMessage *msg, bool isSynchronous)
{
    Q_UNUSED(bus);

    switch (GST_MESSAGE_TYPE(msg))
    {
    case GST_MESSAGE_BUFFERING:
        {
            gint percent = 0;
            gst_message_parse_buffering(msg, &percent);
            qDebug() << "gstreamer: buffering" << percent << "%";
        }
        break;

    case GST_MESSAGE_STATE_CHANGED:
        {
            if (m_state == PermanentError)
                break;

            GstState oldState, newState;
            gst_message_parse_state_changed(msg, &oldState, &newState, 0);
            VideoState vpState = m_state;

            switch (newState)
            {
            case GST_STATE_VOID_PENDING:
            case GST_STATE_NULL:
                if (m_state == Error)
                    break;
            case GST_STATE_READY:
                vpState = Stopped;
                break;
            case GST_STATE_PAUSED:
                vpState = Paused;
                emit durationChanged(duration());
                break;
            case GST_STATE_PLAYING:
                vpState = Playing;
                emit durationChanged(duration());
                break;
            }

            if (m_sinkReady && oldState < GST_STATE_PAUSED && newState >= GST_STATE_PAUSED)
                updateVideoSize();

            if (m_sinkReady && newState < GST_STATE_PAUSED)
                m_sinkReady = false;

            if (vpState != m_state)
            {
                VideoState old = m_state;
                m_state = vpState;
                emit stateChanged(m_state, old);
            }
        }
        break;

    case GST_MESSAGE_DURATION:
        emit durationChanged(duration());
        break;

    case GST_MESSAGE_EOS:
        {
            qDebug("gstreamer: end of stream");
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

            setError(false, tr("Playback error: %1").arg(QString::fromLatin1(error->message)));

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
        if (isSynchronous && gst_structure_has_name(msg->structure, "prepare-xwindow-id"))
        {
            qDebug("gstreamer: Setting window ID");
            GstElement *sink = GST_ELEMENT(GST_MESSAGE_SRC(msg));
            gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(sink), (gulong)m_surface->winId());
            gst_x_overlay_expose(GST_X_OVERLAY(sink));
            gst_message_unref(msg);

            m_sinkReady = true;

            return GST_BUS_DROP;
        }
        break;

    default:
        break;
    }

    return GST_BUS_PASS;
}

bool VideoPlayerBackend::updateVideoSize()
{
    Q_ASSERT(m_videoLink);

    GstIterator *padit = gst_element_iterate_src_pads(m_videoLink);
    bool done = false, success = false;
    while (!done)
    {
        GstPad *pad;
        switch (gst_iterator_next(padit, (gpointer*)&pad))
        {
        case GST_ITERATOR_OK:
            {
                int width, height;
                if (gst_video_get_size(pad, &width, &height))
                {
                    qDebug() << "gstreamer: Determined video size to be" << width << height;
                    bool ok = QMetaObject::invokeMethod(m_surface, "setVideoSize",
                                                        Qt::QueuedConnection,
                                                        Q_ARG(QSize, QSize(width, height)));
                    Q_ASSERT(ok);
                    Q_UNUSED(ok);
                    done = success = true;
                }

                gst_object_unref(GST_OBJECT(pad));
            }
            break;
        case GST_ITERATOR_DONE:
            done = true;
            break;
        default:
            break;
        }
    }

    gst_iterator_free(padit);
    return success;
}
