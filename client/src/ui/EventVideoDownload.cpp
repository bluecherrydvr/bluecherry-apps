#include "EventVideoDownload.h"
#include <QProgressDialog>
#include <QtConcurrentRun>
#include <QFutureWatcher>
#include <QProgressBar>

EventVideoDownload::EventVideoDownload(QObject *parent)
    : QObject(parent), m_dialog(0), m_futureWatch(0)
{
}

EventVideoDownload::~EventVideoDownload()
{
    if (m_videoBuffer)
    {
        m_videoBuffer->setAutoDelete(true);
    }
    else
    {
        if (QFile::exists(m_tempFilePath))
        {
            if (!QFile::remove(m_tempFilePath))
                qDebug() << "EventVideoDownload: Failed to remove temporary video file";
        }
    }
}

void EventVideoDownload::setVideoBuffer(VideoHttpBuffer *buffer)
{
    Q_ASSERT(!m_dialog);
    m_videoBuffer = buffer;

    if (m_videoBuffer)
    {
        m_tempFilePath = m_videoBuffer->bufferFileName();
        m_videoBuffer->setAutoDelete(false);
    }
    else
        m_tempFilePath.clear();
}

void EventVideoDownload::setFilePath(const QString &path)
{
    m_finalPath = path;
}

void EventVideoDownload::start(QWidget *parentWindow)
{
    Q_ASSERT(m_videoBuffer && !m_finalPath.isEmpty());

    m_dialog = new QProgressDialog(parentWindow);
    m_dialog->setLabelText(tr("Downloading event video..."));
    m_dialog->setAutoReset(false);
    m_dialog->setAutoClose(false);
    QProgressBar *pb = new QProgressBar(m_dialog);
    pb->setTextVisible(false);
    pb->show();
    m_dialog->setBar(pb);

    m_dialog->show();

    if (m_videoBuffer->isBufferingFinished())
    {
        startCopy();
    }
    else
    {
        connect(m_videoBuffer.data(), SIGNAL(bufferingFinished()), SLOT(startCopy()));
        connect(m_videoBuffer.data(), SIGNAL(bufferUpdated()), SLOT(updateBufferProgress()));
    }
}

void EventVideoDownload::updateBufferProgress()
{
    Q_ASSERT(m_dialog && m_videoBuffer);

    m_dialog->setRange(0, (int)m_videoBuffer->fileSize());
    m_dialog->setValue((int)m_videoBuffer->bufferedSize());
}

void EventVideoDownload::startCopy()
{
    if (m_tempFilePath.isEmpty() || m_finalPath.isEmpty())
    {
        qWarning() << "EventVideoDownload::startCopy: Invalid parameters";
        return;
    }

    if (QFile::exists(m_finalPath))
    {
        if (!QFile::remove(m_finalPath))
        {
            qDebug() << "EventVideoDownload: Failed to replace video file";
            /* TODO: proper error */
            return;
        }
    }

    QFuture<bool> re = QtConcurrent::run(&QFile::copy, m_tempFilePath, m_finalPath);
    m_futureWatch = new QFutureWatcher<bool>(this);
    m_futureWatch->setFuture(re);

    connect(m_futureWatch, SIGNAL(finished()), SLOT(copyFinished()));

    m_dialog->setLabelText(tr("Copying video..."));
    m_dialog->setRange(0, 0);
}

void EventVideoDownload::copyFinished()
{
    Q_ASSERT(m_futureWatch);

    bool success = m_futureWatch->result();
    if (!success)
    {
        m_dialog->setLabelText(tr("Copy failed!"));
        m_dialog->setRange(0, 100);
        m_dialog->setValue(0);
    }
    else
    {
        m_dialog->setLabelText(tr("Video downloaded successfully"));
        /* This actually is necessary. QProgressDialog is terrible. */
        m_dialog->setRange(0, 101);
        m_dialog->setValue(100);
        m_dialog->setMaximum(100);
    }

    m_dialog->setCancelButtonText(tr("Close"));

    m_futureWatch->deleteLater();
    m_futureWatch = 0;
}
