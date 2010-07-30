#include "LiveFeedWidget.h"
#include "core/DVRCamera.h"
#include "core/DVRServer.h"
#include "core/MJpegStream.h"
#include <QSettings>
#include <QPainter>
#include <QPaintEvent>

LiveFeedWidget::LiveFeedWidget(QWidget *parent)
    : QWidget(parent), m_camera(0), m_dragCamera(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAcceptDrops(true);

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    p.setColor(QPalette::WindowText, Qt::yellow);
    setPalette(p);

    QFont f = font();
    f.setBold(true);
    setFont(f);

    setStatusMessage(tr("No\nCamera"));
}

void LiveFeedWidget::setCamera(DVRCamera *camera)
{
    if (camera == m_camera)
        return;

    m_camera = camera;
    m_currentFrame = QPixmap();
    m_statusMsg.clear();

    updateGeometry();
    update();

    if (!m_camera)
    {
        setStatusMessage(tr("No\nCamera"));
        return;
    }

    /* Test feeds; will be replaced by real camera feeds someday.. */
    QString mjpegTest = camera->server->readSetting("mjpegTest").toString();
    if (!mjpegTest.isEmpty())
    {
        MJpegStream *stream = new MJpegStream(QUrl(mjpegTest), this);
        connect(stream, SIGNAL(updateFrame(QPixmap)), SLOT(updateFrame(QPixmap)));
        connect(stream, SIGNAL(stateChanged(int)), SLOT(mjpegStateChanged(int)));
        stream->start();
    }
}

void LiveFeedWidget::setStatusMessage(const QString &message)
{
    m_statusMsg = message;
    update();
}

void LiveFeedWidget::updateFrame(const QPixmap &frame)
{
    m_currentFrame = frame;
    m_statusMsg.clear();
    update();
}

void LiveFeedWidget::mjpegStateChanged(int state)
{
    switch (state)
    {
    case MJpegStream::Error:
        setStatusMessage(tr("Stream Error"));
        break;
    case MJpegStream::NotConnected:
        setStatusMessage(tr("Disconnected"));
        break;
    case MJpegStream::Connecting:
        setStatusMessage(tr("Connecting..."));
        break;
    case MJpegStream::Streaming:
        setStatusMessage(tr("Buffering..."));
        break;
    }
}

QSize LiveFeedWidget::sizeHint() const
{
    return QSize();
}

void LiveFeedWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect r = rect();
    p.eraseRect(r);

    if (m_dragCamera)
    {
        p.save();
        p.setPen(QPen(QBrush(Qt::white), 2));
        p.setRenderHint(QPainter::Antialiasing, true);
        p.drawRoundedRect(r.adjusted(2, 2, -2, -2), 3, 3);
        p.restore();

        p.drawText(r.adjusted(6, 6, -6, -6), Qt::AlignTop | Qt::AlignRight, m_dragCamera->displayName());
        return;
    }

    if (!m_currentFrame.isNull())
    {
        QSize renderSize = m_currentFrame.size();
        renderSize.scale(r.size(), Qt::KeepAspectRatio);

        if (renderSize != m_currentFrame.size())
            m_currentFrame = m_currentFrame.scaled(renderSize, Qt::KeepAspectRatio, Qt::FastTransformation);

        QPoint topLeft((r.width() - renderSize.width())/2, (r.height() - renderSize.height())/2);
        p.drawPixmap(topLeft, m_currentFrame);
    }

    if (!m_statusMsg.isEmpty())
    {
        /* If the frame was painted, darken it, because status indicates it's not operating normally */
        p.fillRect(r, QColor(0, 0, 0, 190));

        QFont font(p.font());
        font.setPointSize(14);
        font.setBold(false);
        p.save();
        p.setFont(font);
        p.setPen(m_currentFrame.isNull() ? QColor(60, 60, 60) : QColor(Qt::white));

        p.drawText(r, Qt::AlignCenter, m_statusMsg);
        p.restore();
    }

    if (m_camera)
        p.drawText(r.adjusted(4, 4, -4, -4), Qt::AlignTop | Qt::AlignRight, m_camera->displayName());
}

DVRCamera *LiveFeedWidget::cameraFromMime(const QMimeData *mimeData)
{
    QByteArray data = mimeData->data(QLatin1String("application/x-bluecherry-dvrcamera"));
    QDataStream stream(&data, QIODevice::ReadOnly);

    /* Ignore everything except the first camera dropped */
    int serverid, cameraid;
    stream >> serverid >> cameraid;

    if (stream.status() != QDataStream::Ok)
        return 0;

    return DVRCamera::findByID(serverid, cameraid);
}

void LiveFeedWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QLatin1String("application/x-bluecherry-dvrcamera")))
    {
        /* This could also be done via an event filter or overlay widget, if it later becomes necessary. */
        m_dragCamera = cameraFromMime(event->mimeData());
        event->acceptProposedAction();
        update();
    }
}

void LiveFeedWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    m_dragCamera = 0;
    update();
}

void LiveFeedWidget::dropEvent(QDropEvent *event)
{
    m_dragCamera = 0;

    DVRCamera *camera = cameraFromMime(event->mimeData());
    if (camera)
    {
        setCamera(camera);
        event->acceptProposedAction();
    }
}
