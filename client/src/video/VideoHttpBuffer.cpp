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
    : QObject(parent), m_networkReply(0), m_fileSize(0), m_readPos(0), m_writePos(0), m_element(element), m_pipeline(pipeline),
      m_bufferBlocked(false), ratePos(0), rateMax(0)
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

    QMutexLocker lock(&m_lock);
    if (!m_bufferFile.open())
    {
        qWarning() << "Failed to open buffer file for video:" << m_bufferFile.errorString();
        return false;
    }
    qDebug() << m_bufferFile.fileName();
    m_readPos = m_writePos = 0;
    lock.unlock();

    qDebug("VideoHttpBuffer: started");
    return true;
}

void VideoHttpBuffer::networkMetaData()
{
    QMutexLocker lock(&m_lock);
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

    lock.unlock();

    gst_app_src_set_size(m_element, m_fileSize);
}

void VideoHttpBuffer::networkRead()
{
    QMutexLocker lock(&m_lock);
    Q_ASSERT(m_bufferFile.isOpen());

    if (!m_bufferFile.seek(m_writePos))
    {
        emit streamError(tr("Seek in buffer file failed: %1").arg(m_bufferFile.errorString()));
        return;
    }

    char data[5120];
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

        m_writePos += wr;

        if (r < sizeof(data))
            break;
    }

    if (r < 0)
    {
        emit streamError(tr("Failed to read video stream: %1").arg(m_networkReply->errorString()));
        return;
    }

    m_fileSize = qMax(m_fileSize, (qint64)m_writePos);
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

#if 0
    /* Record the request for rate estimation */
    GstClock *clock = gst_system_clock_obtain();
    addRateData(gst_clock_get_time(clock), size);
    gst_object_unref(GST_OBJECT(clock));

    quint64 edur;
    unsigned esize;
    getRateEstimation(&edur, &esize);
#endif

    QMutexLocker lock(&m_lock);

    if (m_writePos < m_fileSize && unsigned(m_writePos - m_readPos) < size*3)
    {
        qDebug() << "VideoHttpBuffer: buffer is exhausted with" << (m_fileSize - m_readPos) << "bytes remaining in stream";
        m_bufferBlocked = true;
        lock.unlock();

        GstStateChangeReturn re = gst_element_set_state(m_pipeline, GST_STATE_PAUSED);
        if (re == GST_STATE_CHANGE_FAILURE)
            qWarning() << "VideoHttpBuffer: FAILED to pause pipeline!";

        if (re == GST_STATE_CHANGE_ASYNC)
        {
            /* gst_element_get_state will block until the asynchronous state change completes */
            GstState stateNow, statePending;
            re = gst_element_get_state(m_pipeline, &stateNow, &statePending, GST_CLOCK_TIME_NONE);
        }

        lock.relock();
    }
    else
        m_bufferBlocked = false;

    size = qMin(size, unsigned(m_writePos - m_readPos));
    if (!size)
        return;

    /* Refactor to use gst_pad_alloc_buffer? Probably wouldn't provide any benefit. */
    GstBuffer *buffer = gst_buffer_new_and_alloc(size);

    if (!m_bufferFile.seek(m_readPos))
    {
        qDebug() << "VideoHttpBuffer: Failed to seek for read:" << m_bufferFile.errorString();
        return;
    }

    GST_BUFFER_SIZE(buffer) = m_bufferFile.read((char*)GST_BUFFER_DATA(buffer), size);

    if (GST_BUFFER_SIZE(buffer) < 1)
    {
        gst_buffer_unref(buffer);

        if (m_bufferFile.atEnd())
        {
            qDebug() << "VideoHttpBuffer: end of stream";
            gst_app_src_end_of_stream(m_element);
            return;
        }

        qDebug() << "VideoHttpBuffer: read error:" << m_bufferFile.errorString();
        /* TODO error */
        return;
    }

    m_readPos += size;
    lock.unlock();

    GstFlowReturn flow = gst_app_src_push_buffer(m_element, buffer);
    if (flow != GST_FLOW_OK)
        qDebug() << "VideoHttpBuffer: Push result is" << flow;
}

bool VideoHttpBuffer::seekData(qint64 offset)
{
    qDebug() << "VideoHttpBuffer: seek to" << offset;
    m_readPos = offset;
    return true;
}

void VideoHttpBuffer::addRateData(quint64 time, unsigned size)
{
    rateData[ratePos].time = time;
    rateData[ratePos].size = size;
    rateMax = qMax(rateMax, ++ratePos);
    if (ratePos == rateCount)
        ratePos = 0;
}

void VideoHttpBuffer::getRateEstimation(quint64 *duration, unsigned *size)
{
    *size = 0;
    if (!rateMax)
    {
        *duration = 0;
        return;
    }

    int end = ratePos ? (ratePos-1) : (rateMax-1);
    int begin = (ratePos == rateMax) ? 0 : ratePos;

    *duration = rateData[end].time - rateData[begin].time;
    for (int i = begin; i != end;)
    {
        *size += rateData[i].size;
        if (++i == rateMax)
            i = 0;
    }
    *size += rateData[end].size;

    qDebug() << "Data in" << *duration << "ns was" << *size;
}
