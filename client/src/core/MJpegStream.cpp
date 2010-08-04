#include "BluecherryApp.h"
#include "MJpegStream.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDebug>
#include <QImageReader>
#include <QBuffer>
#include <QImage>

MJpegStream::MJpegStream(QObject *parent)
    : QObject(parent), m_httpReply(0), m_state(NotConnected), m_parserState(ParserBoundary), m_httpBodyLength(0)
{
}

MJpegStream::MJpegStream(const QUrl &url, QObject *parent)
    : QObject(parent), m_httpReply(0), m_state(NotConnected), m_parserState(ParserBoundary), m_httpBodyLength(0)
{
    setUrl(url);
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
        emit streamRunning();
    else if (oldState == Streaming)
        emit streamStopped();
}

void MJpegStream::setError(const QString &message)
{
    qDebug() << "mjpeg: error:" << message;
    setState(Error);
    stop();
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
        setError(tr("Internal Error"));
        return;
    }

    setState(Connecting);

    m_httpReply = bcApp->nam->get(QNetworkRequest(url()));
    connect(m_httpReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(requestError()));
    connect(m_httpReply, SIGNAL(readyRead()), SLOT(readable()));
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
}

bool MJpegStream::processHeaders()
{
    Q_ASSERT(m_httpReply);

    QByteArray data = m_httpReply->header(QNetworkRequest::ContentTypeHeader).toByteArray().toLower();

    /* Get the MIME type */
    QByteArray mimeType;

    int sep = data.indexOf(';');
    if (sep > 0)
        mimeType = data.left(sep).trimmed();

    m_httpBoundary.clear();
    sep = data.indexOf("boundary=", sep);
    if (sep > 0)
        m_httpBoundary = data.mid(sep+9);

    if (mimeType != "multipart/x-mixed-replace" || m_httpBoundary.isEmpty())
    {
        setError(tr("Invalid content type"));
        return false;
    }

    m_httpBoundary.prepend("\r\n--");
    return true;
}

void MJpegStream::readable()
{
    if (!m_httpReply)
        return;

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
            setError(tr("Exceeded maximum buffer size"));
            return;
        }

        int maxRead = qMin((int)avail, maxBuffer - m_httpBuffer.size());
        int readPos = m_httpBuffer.size();
        m_httpBuffer.resize(m_httpBuffer.size()+maxRead);

        int rd = m_httpReply->read(m_httpBuffer.data()+readPos, maxRead);
        if (rd < 0)
        {
            setError(tr("Read error"));
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
            /* Everything up to the last m_httpBoundary-1 bytes may be safely discarded */
            m_httpBuffer.remove(0, m_httpBuffer.size() - (m_httpBoundary.size()-1));
            return true;
        }

        int boundaryStart = boundary;
        boundary += m_httpBoundary.size();

testBoundary:
        if (m_httpBuffer.size() - boundary < 2)
        {
            /* Not enough to finish the boundary yet; wait and try again */
            m_httpBuffer.remove(0, boundaryStart);
            return true;
        }

        /* The -- suffix would indicate the last part, but in a neverending stream, that won't
         * happen. Allow it, but ignore it. */
        if (m_httpBuffer[boundary] == '-' && m_httpBuffer[boundary+1] == '-')
        {
            boundary += 2;
            goto testBoundary;
        }
        else if (m_httpBuffer[boundary] == '\r' && m_httpBuffer[boundary+1] == '\n')
            boundary += 2;

        /* Reached the end of the boundary; headers follow */
        m_httpBuffer.remove(0, boundary);
        m_parserState = ParserHeaders;
        m_httpBodyLength = 0;
    }

    if (m_parserState == ParserHeaders)
    {
        int lnStart = 0, lnEnd = 0;
        for (; (lnEnd = m_httpBuffer.indexOf("\r\n", lnStart)) >= 0; lnStart = lnEnd + 2)
        {
            if (lnStart == lnEnd)
            {
                m_parserState = ParserBody;
                /* Skip the final \r\n */
                lnStart += 2;
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
        {
            QByteArray body = QByteArray::fromRawData(m_httpBuffer.data(), m_httpBodyLength);
            decodeFrame(body);
        }

        m_httpBuffer.remove(0, m_httpBodyLength);
        m_parserState = ParserBoundary;

        /* Parse again; recursion isn't a problem because this should be a very rare case */
        if (!m_httpBuffer.isEmpty())
            parseBuffer();
    }

    return true;
}

void MJpegStream::requestError()
{
    setError(tr("HTTP error: %1").arg(m_httpReply->errorString()));
}

void MJpegStream::decodeFrame(QByteArray &data)
{
    QBuffer buffer(&data);
    QImageReader reader(&buffer, "jpeg");
    reader.setAutoDetectImageFormat(false);

#if QT_VERSION >= 0040700
    QPixmap newFrame = QPixmap::fromImageReader(&reader);
#else
    QPixmap newFrame = reader.read().toPixmap();
#endif

    if (newFrame.isNull())
    {
        qDebug() << "mjpeg: error while decoding frame:" << reader.errorString();
        return;
    }

    m_currentFrame = newFrame;
    emit updateFrame(m_currentFrame);
}
