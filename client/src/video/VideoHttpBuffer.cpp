#include "VideoHttpBuffer.h"
#include "core/BluecherryApp.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

VideoHttpBuffer::VideoHttpBuffer(QObject *parent)
    : QObject(parent), m_networkReply(0), m_fileSize(0)
{
    m_bufferFile.setFileTemplate(QLatin1String("bc_vbuf_XXXXXX.mkv"));
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
}

void VideoHttpBuffer::networkRead()
{
    Q_ASSERT(m_bufferFile.isOpen());

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

        if (r < sizeof(data))
            break;
    }

    if (r < 0)
    {
        emit streamError(tr("Failed to read video stream: %1").arg(m_networkReply->errorString()));
        return;
    }

    m_fileSize = qMax(m_fileSize, m_bufferFile.pos());
    emit readyRead();
}

void VideoHttpBuffer::networkFinished()
{
    emit endOfFile();
}
