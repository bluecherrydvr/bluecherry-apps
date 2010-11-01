#include "BluecherryApp.h"
#include "MJpegStream.h"
#include "utils/ImageDecodeTask.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QImage>
#include <QThreadPool>
#include <QTimer>
#include <QDateTime>

MJpegStream::MJpegStream(QObject *parent)
    : QObject(parent), m_httpReply(0), m_decodeTask(0), m_lastActivity(0), m_httpBodyLength(0), m_state(NotConnected),
      m_parserState(ParserBoundary)
{
    connect(&m_activityTimer, SIGNAL(timeout()), SLOT(checkActivity()));
}

MJpegStream::MJpegStream(const QUrl &url, QObject *parent)
    : QObject(parent), m_httpReply(0), m_decodeTask(0), m_lastActivity(0), m_httpBodyLength(0), m_state(NotConnected),
      m_parserState(ParserBoundary)
{
    setUrl(url);
    connect(&m_activityTimer, SIGNAL(timeout()), SLOT(checkActivity()));
}

MJpegStream::~MJpegStream()
{
    if (m_httpReply)
        m_httpReply->deleteLater();
}

void MJpegStream::setState(State newState)
{
    if (m_state == newState)
        return;

    State oldState = m_state;
    m_state = newState;

    emit stateChanged(newState);

    if (newState == Streaming)
    {
        emit streamRunning();
        updateScaleSizes();
    }
    else if (oldState == Streaming)
        emit streamStopped();
}

void MJpegStream::setError(const QString &message)
{
    qDebug() << "mjpeg: error:" << message;
    setState(Error);
    stop();

    QTimer::singleShot(15000, this, SLOT(start()));
}

void MJpegStream::setUrl(const QUrl &url)
{
    m_url = url;
}

void MJpegStream::start()
{
    if (state() >= Connecting)
        return;

    if (!url().isValid())
    {
        setError(QLatin1String("Internal Error"));
        return;
    }

    setState(Connecting);

    m_httpReply = bcApp->nam->get(QNetworkRequest(url()));
    connect(m_httpReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError()));
    connect(m_httpReply, SIGNAL(readyRead()), SLOT(readable()));

    m_activityTimer.start(30000);
}

void MJpegStream::stop()
{
    if (m_httpReply)
    {
        m_httpReply->disconnect(this);
        m_httpReply->abort();
        m_httpReply->deleteLater();
        m_httpReply = 0;
    }

    m_httpBuffer.clear();
    m_httpBoundary.clear();

    if (state() > NotConnected)
        setState(NotConnected);

    m_activityTimer.stop();
}

bool MJpegStream::processHeaders()
{
    Q_ASSERT(m_httpReply);

    QByteArray data = m_httpReply->header(QNetworkRequest::ContentTypeHeader).toByteArray();
    QByteArray dataL = data.toLower();

    /* Get the MIME type */
    QByteArray mimeType;

    int sep = dataL.indexOf(';');
    if (sep > 0)
        mimeType = dataL.left(sep).trimmed();

    m_httpBoundary.clear();
    sep = dataL.indexOf("boundary=", sep);
    if (sep > 0)
        m_httpBoundary = data.mid(sep+9);

    if (mimeType != "multipart/x-mixed-replace" || m_httpBoundary.isEmpty())
    {
        setError(QLatin1String("Invalid content type"));
        return false;
    }

    m_httpBoundary.prepend("\n--");
    return true;
}

void MJpegStream::readable()
{
    if (!m_httpReply)
        return;

    m_lastActivity = QDateTime::currentDateTime().toTime_t();

    if (m_httpBoundary.isNull())
    {
        if (!processHeaders())
            return;
        Q_ASSERT(!m_httpBoundary.isNull());

        m_parserState = ParserBoundary;
        setState(Streaming);
    }

    /* Don't allow the buffer to exceed 2MB */
    static const int maxBuffer = 2*1024*1024;

    for (;;)
    {
        qint64 avail = m_httpReply->bytesAvailable();
        if (avail < 1)
            break;

        if (m_httpBuffer.size() >= maxBuffer)
        {
            setError(QLatin1String("Exceeded maximum buffer size"));
            return;
        }

        int maxRead = qMin((int)avail, maxBuffer - m_httpBuffer.size());
        int readPos = m_httpBuffer.size();
        m_httpBuffer.resize(m_httpBuffer.size()+maxRead);

        int rd = m_httpReply->read(m_httpBuffer.data()+readPos, maxRead);
        if (rd < 0)
        {
            setError(QLatin1String("Read error"));
            return;
        }

        if (rd < maxRead)
            m_httpBuffer.truncate(readPos+rd);

        if (!parseBuffer() || !m_httpReply)
            return;
    }
}

bool MJpegStream::parseBuffer()
{
    if (m_parserState == ParserBoundary)
    {
        /* Search for the boundary */
        int boundary = m_httpBuffer.indexOf(m_httpBoundary);
        if (boundary < 0)
        {
            /* Everything up to the last m_httpBoundary-2 bytes may be safely discarded */
            m_httpBuffer.remove(0, m_httpBuffer.size() - (m_httpBoundary.size()-2));
            return true;
        }

        int boundaryStart = boundary;
        if (boundaryStart && m_httpBuffer[boundaryStart-1] == '\r')
            --boundaryStart;

        boundary += m_httpBoundary.size();
        int sz = m_httpBuffer.size() - boundary;

        if (sz >= 2 && m_httpBuffer[boundary] == '-' && m_httpBuffer[boundary+1] == '-')
        {
            boundary += 2;
            sz -= 2;
        }

        if (sz && m_httpBuffer[boundary] == '\n')
        {
            boundary++;
        }
        else if (sz >= 2 && m_httpBuffer[boundary] == '\r' && m_httpBuffer[boundary+1] == '\n')
        {
            boundary += 2;
        }
        else if (sz < 2)
        {
            /* Not enough to finish the boundary; remove everything up to the start and wait */
            m_httpBuffer.remove(0, boundaryStart);
            return true;
        }
        else
        {
            /* Invalid characters mean this isn't a boundary. Remove it. */
            m_httpBuffer.remove(0, boundaryStart + m_httpBoundary.size());
            return true;
        }

        /* Reached the end of the boundary; headers follow */
        m_httpBuffer.remove(0, boundary);
        m_parserState = ParserHeaders;
        m_httpBodyLength = 0;
    }

    if (m_parserState == ParserHeaders)
    {
        int lnStart = 0, lnEnd = 0;
        for (; (lnEnd = m_httpBuffer.indexOf('\n', lnStart)) >= 0; lnStart = lnEnd+1)
        {
            if (lnStart == lnEnd || ((lnEnd-lnStart) == 1 && m_httpBuffer[lnStart] == '\r'))
            {
                m_parserState = ParserBody;
                /* Skip the final \n */
                lnStart = lnEnd+1;
                break;
            }

            /* We only care about Content-Length */
            if ((lnEnd - lnStart) > 15 && qstrnicmp(m_httpBuffer.data()+lnStart, "Content-Length:", 15) == 0)
            {
                bool ok = false;
                m_httpBodyLength = m_httpBuffer.mid(lnStart+15, lnEnd-lnStart-15).trimmed().toUInt(&ok);
                if (!ok)
                    m_httpBodyLength = 0;
            }
        }

        /* All processed headers can be removed */
        m_httpBuffer.remove(0, lnStart);
    }

    if (m_parserState == ParserBody)
    {
        if (m_httpBodyLength)
        {
            /* The easy route; Content-Length tells us where the body ends */
            if (m_httpBuffer.size() < m_httpBodyLength)
            {
                /* Not enough data yet. */
                return true;
            }
        }
        else
        {
            /* Scan for the boundary */
            int boundary = m_httpBuffer.indexOf(m_httpBoundary);
            if (boundary < 0)
                return true;

            m_httpBodyLength = boundary;
        }

        /* End of the body; m_httpBodyLength is its length, at the beginning of m_httpBuffer */
        qDebug() << "mjpeg: read body of" << m_httpBodyLength << "bytes";

        /* Create a QByteArray for just the body, without copying */
        decodeFrame(QByteArray(m_httpBuffer.data(), m_httpBodyLength));

        m_httpBuffer.remove(0, m_httpBodyLength);
        m_parserState = ParserBoundary;

        /* Parse again; recursion isn't a problem because this should be a very rare case */
        if (!m_httpBuffer.isEmpty())
            parseBuffer();
    }

    return true;
}

void MJpegStream::checkActivity()
{
    if (QDateTime::currentDateTime().toTime_t() - m_lastActivity > 30)
        setError(QLatin1String("Stream timeout"));
}

void MJpegStream::requestError()
{
    setError(QString::fromLatin1("HTTP error: %1").arg(m_httpReply->errorString()));
}

void MJpegStream::updateScaleSizes()
{
    /* Remove duplicates and such? */
    m_scaleSizes.clear();
    emit buildScaleSizes(m_scaleSizes);
}

void MJpegStream::decodeFrame(const QByteArray &data)
{
    /* This will cancel the task if it hasn't started yet; in-progress or completed tasks will still
     * deliver a result */
    if (m_decodeTask)
        m_decodeTask->cancel();

    m_decodeTask = new ImageDecodeTask(this, "decodeFrameResult");
    m_decodeTask->setData(data);
    m_decodeTask->setScaleSizes(m_scaleSizes);

    QThreadPool::globalInstance()->start(m_decodeTask);
}

void MJpegStream::decodeFrameResult(ThreadTask *task)
{
    ImageDecodeTask *decodeTask = static_cast<ImageDecodeTask*>(task);
    if (m_decodeTask == decodeTask)
        m_decodeTask = 0;

    if (decodeTask->result().isNull())
        return;

    bool sizeChanged = decodeTask->result().size() != m_currentFrame.size();
    m_currentFrame = QPixmap::fromImage(decodeTask->result());
    if (sizeChanged)
        emit streamSizeChanged(m_currentFrame.size());
    emit updateFrame(m_currentFrame, decodeTask->scaleResults());
}
