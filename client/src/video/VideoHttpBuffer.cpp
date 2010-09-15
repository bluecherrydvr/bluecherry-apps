#include "VideoHttpBuffer.h"
#include "core/BluecherryApp.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>
#include <QDir>

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>

void VideoHttpBuffer::needDataWrap(GstAppSrc *src, guint length, gpointer user_data)
{
    static_cast<VideoHttpBuffer*>(user_data)->needData(length);
}

gboolean VideoHttpBuffer::seekDataWrap(GstAppSrc *src, guint64 offset, gpointer user_data)
{
    return static_cast<VideoHttpBuffer*>(user_data)->seekData(offset) ? TRUE : FALSE;
}

VideoHttpBuffer::VideoHttpBuffer(GstAppSrc *element, GstElement *pipeline, QObject *parent)
    : QObject(parent), m_networkReply(0), m_fileSize(0), m_readPos(0), m_writePos(0), m_element(element), m_bufferBlocked(false),
      m_pipeline(pipeline)
{
    m_bufferFile.setFileTemplate(QDir::tempPath() + QLatin1String("/bc_vbuf_XXXXXX.mkv"));

    gst_app_src_set_stream_type(m_element, GST_APP_STREAM_TYPE_SEEKABLE);

    GstAppSrcCallbacks callbacks = { needDataWrap, 0, seekDataWrap };
    /* Should we grab the DestroyNotify as well? */
    gst_app_src_set_callbacks(m_element, &callbacks, this, 0);
}

VideoHttpBuffer::~VideoHttpBuffer()
{
    /* Cleanup callbacks on m_element? */
    /* Deref m_element? */

    m_readFile.close();
}

bool VideoHttpBuffer::start(const QUrl &url)
{
    Q_ASSERT(!m_networkReply);
    if (m_networkReply)
        delete m_networkReply;

    m_networkReply = bcApp->nam->get(QNetworkRequest(url));
    connect(m_networkReply, SIGNAL(readyRead()), SLOT(networkRead()));
    connect(m_networkReply, SIGNAL(finished()), SLOT(networkFinished()));
    connect(m_networkReply, SIGNAL(metaDataChanged()), SLOT(networkMetaData()));

    if (!m_bufferFile.open())
    {
        qWarning() << "Failed to open buffer file for video:" << m_bufferFile.errorString();
        return false;
    }

    qDebug() << m_bufferFile.fileName();
    m_readFile.setFileName(m_bufferFile.fileName());
    if (!m_readFile.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open read buffer for video:" << m_readFile.errorString();
        return false;
    }

    m_readPos = m_writePos = 0;

    qDebug("VideoHttpBuffer: started");
    return true;
}

void VideoHttpBuffer::networkMetaData()
{
    if (m_fileSize)
        return;

    bool ok = false;
    m_fileSize = m_networkReply->header(QNetworkRequest::ContentLengthHeader).toLongLong(&ok);

    if (!ok)
    {
        qDebug() << "VideoHttpBuffer: No content-length for video download";
        m_fileSize = 0;
        return;
    }

    qDebug() << "VideoHttpBuffer: File size is" << m_fileSize;
    Q_ASSERT(m_bufferFile.isOpen());
    if (!m_bufferFile.resize(m_fileSize))
        qDebug() << "VideoHttpBuffer: Failed to resize buffer file:" << m_bufferFile.errorString();

    gst_app_src_set_size(m_element, m_fileSize);
}

void VideoHttpBuffer::networkRead()
{
    Q_ASSERT(m_bufferFile.isOpen());

    char data[5120];
    qint64 wrt = 0;
    qint64 r;
    while ((r = m_networkReply->read(data, sizeof(data))) > 0)
    {
        qint64 wr = m_bufferFile.write(data, r);
        if (wr < 0)
        {
            emit streamError(tr("Write to buffer file failed: %1").arg(m_bufferFile.errorString()));
            return;
        }
        else if (wr < r)
        {
            emit streamError(tr("Write to buffer file failed: %1").arg(QLatin1String("Data written to buffer is incomplete")));
            return;
        }

        wrt += wr;

        if (r < sizeof(data))
            break;
    }

    if (r < 0)
    {
        emit streamError(tr("Failed to read video stream: %1").arg(m_networkReply->errorString()));
        return;
    }

    /* TODO: This probably needs some better synchronization to prevent issues with the read thread */
    m_bufferFile.flush();
    m_writePos += wrt;
    m_fileSize = qMax(m_fileSize, m_writePos);
}

void VideoHttpBuffer::networkFinished()
{
    qDebug("VideoHttpBuffer: Finished download");
    if (m_fileSize != m_writePos)
    {
        qDebug() << "VideoHttpBuffer: Adjusting filesize to match actual downloaded amount";
        m_fileSize = m_writePos;
        gst_app_src_set_size(m_element, m_fileSize);
    }
}

void VideoHttpBuffer::needData(unsigned size)
{
    /* Stop the stream if there is less than twice the amount requested, unless that is the end of the stream.
     * This gives the pipeline enough buffer to resume from the paused state properly.
     *
     * TODO: Calculate this buffer using time rather than bytes */

    if (m_writePos < m_fileSize && unsigned(m_writePos - m_readPos) < size*2)
    {
        qDebug() << "VideoHttpBuffer: buffer is exhausted with" << (m_fileSize - m_readPos) << "bytes remaining in stream";
        m_bufferBlocked = true;
        GstStateChangeReturn re = gst_element_set_state(m_pipeline, GST_STATE_PAUSED);

        /* gst_element_get_state will block until the (asynchronous) state change completes */
        GstState stateNow, statePending;
        re = gst_element_get_state(m_pipeline, &stateNow, &statePending, GST_CLOCK_TIME_NONE);
    }

    size = qMin(size, unsigned(m_writePos - m_readPos));

    /* Refactor to use gst_pad_alloc_buffer? Probably wouldn't provide any benefit. */
    GstBuffer *buffer = gst_buffer_new_and_alloc(size);

    Q_ASSERT(m_readFile.pos() == m_readPos);
    GST_BUFFER_SIZE(buffer) = m_readFile.read((char*)GST_BUFFER_DATA(buffer), size);

    if (GST_BUFFER_SIZE(buffer) < 1)
    {
        gst_buffer_unref(buffer);

        if (m_readFile.atEnd())
        {
            qDebug() << "VideoHttpBuffer: end of stream";
            gst_app_src_end_of_stream(m_element);
            return;
        }

        qDebug() << "VideoHttpBuffer: read error:" << m_readFile.errorString();
        /* TODO error */
        return;
    }

    m_readPos += size;

    GstFlowReturn flow = gst_app_src_push_buffer(m_element, buffer);
    if (flow != GST_FLOW_OK)
        qDebug() << "VideoHttpBuffer: Push result is" << flow;
}

bool VideoHttpBuffer::seekData(qint64 offset)
{
    qDebug() << "VideoHttpBuffer: seek to" << offset;
    if (!m_readFile.seek(offset))
    {
        qDebug() << "VideoHttpBuffer: seek failed";
        return false;
    }

    m_readPos = offset;
    return true;
}
