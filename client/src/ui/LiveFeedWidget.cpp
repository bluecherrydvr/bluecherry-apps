#include "LiveFeedWidget.h"
#include "core/DVRCamera.h"
#include "core/DVRServer.h"
#include "core/MJpegStream.h"
#include <QSettings>
#include <QPainter>
#include <QPaintEvent>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QToolTip>
#include <QDataStream>

LiveFeedWidget::LiveFeedWidget(QWidget *parent)
    : QWidget(parent), m_stream(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setFocusPolicy(Qt::ClickFocus);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    QPalette p = palette();
    p.setColor(QPalette::Window, Qt::black);
    p.setColor(QPalette::WindowText, Qt::yellow);
    setPalette(p);

    QFont f = font();
    f.setBold(true);
    setFont(f);

    setStatusMessage(tr("No\nCamera"));
}

LiveFeedWidget::~LiveFeedWidget()
{
    if (m_stream)
    {
        m_stream->disconnect(this);
        m_stream->updateScaleSizes();
    }
}

void LiveFeedWidget::clone(LiveFeedWidget *other)
{
    setCamera(other->camera());
    m_statusMsg = other->statusMessage();
    m_currentFrame = other->m_currentFrame;
}

void LiveFeedWidget::setCamera(const DVRCamera &camera)
{
    if (camera == m_camera)
        return;

    if (m_stream)
    {
        m_stream->disconnect(this);
        m_stream->updateScaleSizes();
        m_stream.clear();
    }

    m_camera = camera;
    m_currentFrame = QPixmap();
    m_statusMsg.clear();

    updateGeometry();
    update();

    if (!m_camera)
    {
        setStatusMessage(tr("No\nCamera"));
    }
    else
    {
        m_stream = m_camera.mjpegStream();
        if (m_stream)
        {
            m_currentFrame = m_stream->currentFrame();
            connect(m_stream.data(), SIGNAL(updateFrame(QPixmap,QVector<QImage>)),
                    SLOT(updateFrame(QPixmap,QVector<QImage>)));
            connect(m_stream.data(), SIGNAL(buildScaleSizes(QVector<QSize>&)), SLOT(addScaleSize(QVector<QSize>&)));
            connect(m_stream.data(), SIGNAL(stateChanged(int)), SLOT(mjpegStateChanged(int)));
            m_stream->start();
        }
        else
            setStatusMessage(tr("No\nVideo"));
    }

    emit cameraChanged(m_camera);
}

void LiveFeedWidget::setStatusMessage(const QString &message)
{
    m_statusMsg = message;
    update();
}

QWidget *LiveFeedWidget::openWindow()
{
    LiveFeedWidget *widget = new LiveFeedWidget(window());
    widget->setWindowFlags(Qt::Window);
    widget->setAttribute(Qt::WA_DeleteOnClose);
    widget->clone(this);
    widget->show();
    return widget;
}

void LiveFeedWidget::closeCamera()
{
    if (isWindow())
        close();
    else
        clearCamera();
}

void LiveFeedWidget::setFullScreen(bool on)
{
    if (on)
    {
        if (isWindow())
            showFullScreen();
        else
            openWindow()->showFullScreen();
    }
    else if (isWindow())
        close();
}

void LiveFeedWidget::addScaleSize(QVector<QSize> &sizes)
{
    sizes.append(size());
}

void LiveFeedWidget::updateFrame(const QPixmap &frame, const QVector<QImage> &scaledFrames)
{
    QSize desired = frame.size();
    desired.scale(size(), Qt::KeepAspectRatio);

    m_currentFrame = frame;
    for (QVector<QImage>::ConstIterator it = scaledFrames.begin(); it != scaledFrames.end(); ++it)
    {
        if (it->size() == desired)
        {
            m_currentFrame = QPixmap::fromImage(*it);
            break;
        }
    }

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

        p.drawText(r.adjusted(6, 6, -6, -6), Qt::AlignTop | Qt::AlignRight, m_dragCamera.displayName());
        return;
    }

    if (!m_currentFrame.isNull())
    {
        QSize renderSize = m_currentFrame.size();
        renderSize.scale(r.size(), Qt::KeepAspectRatio);

        if (renderSize != m_currentFrame.size())
        {
            if (m_stream)
                m_currentFrame = m_stream->currentFrame();
            m_currentFrame = m_currentFrame.scaled(renderSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        }

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
        p.drawText(r.adjusted(4, 4, -4, -4), Qt::AlignTop | Qt::AlignRight, m_camera.displayName());
}

DVRCamera LiveFeedWidget::cameraFromMime(const QMimeData *mimeData)
{
    QByteArray data = mimeData->data(QLatin1String("application/x-bluecherry-dvrcamera"));
    QDataStream stream(&data, QIODevice::ReadOnly);

    /* Ignore everything except the first camera dropped */
    int serverid, cameraid;
    stream >> serverid >> cameraid;

    if (stream.status() != QDataStream::Ok)
        return DVRCamera();

    return DVRCamera::getCamera(serverid, cameraid);
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
    m_dragCamera = DVRCamera();
    update();
}

void LiveFeedWidget::dropEvent(QDropEvent *event)
{
    m_dragCamera = DVRCamera();

    DVRCamera camera = cameraFromMime(event->mimeData());
    if (camera)
    {
        setCamera(camera);
        event->acceptProposedAction();
    }
}

void LiveFeedWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if (m_stream)
        m_stream->updateScaleSizes();
}

void LiveFeedWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    menu.addAction(tr("Snapshot"), this, SLOT(saveSnapshot()))->setEnabled(m_camera);
    menu.addAction(tr("Pause"))->setEnabled(m_camera);
    menu.addSeparator();
    menu.addAction(tr("Open in window"), this, SLOT(openWindow()));
    menu.addAction(!isFullScreen() ? tr("Open as fullscreen") : tr("Exit fullscreen"), this, SLOT(toggleFullScreen()));
    menu.addSeparator();

    QActionGroup *group = new QActionGroup(&menu);
    group->setExclusive(true);
    group->setEnabled(!isFullScreen());

    QAction *actSizeFit = group->addAction(tr("Size to fit"));
    actSizeFit->setCheckable(true);
    actSizeFit->setChecked(true);

    QAction *actSizeFull = group->addAction(tr("Full size"));
    actSizeFull->setCheckable(true);
    actSizeFull->setEnabled(m_camera);

    menu.addActions(group->actions());
    menu.addSeparator();

    QAction *actClose = menu.addAction(tr("Close camera"), this, SLOT(closeCamera()));
    actClose->setEnabled(m_camera);

    menu.exec(event->globalPos());
}

void LiveFeedWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Escape:
        if (isWindow())
            close();
        break;
    case Qt::Key_F11:
        toggleFullScreen();
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }

    event->accept();
}

void LiveFeedWidget::saveSnapshot(const QString &ifile)
{
    if (m_currentFrame.isNull())
        return;

    QString file = ifile;

    if (file.isEmpty())
    {
        file = QFileDialog::getSaveFileName(this, tr("Save Camera Snapshot"), QString(), tr("Image (*.jpg)"));
        if (file.isEmpty())
            return;
    }

    if (!m_currentFrame.save(file, "jpeg"))
    {
        QMessageBox::critical(this, tr("Snapshot Error"), tr("An error occurred while saving the snapshot image."),
                              QMessageBox::Ok);
        return;
    }

    QToolTip::showText(mapToGlobal(QPoint(0,0)), tr("Snapshot Saved"), this);
}

QDataStream &operator<<(QDataStream &stream, const LiveFeedWidget &widget)
{
    const DVRCamera &camera = widget.camera();
    if (!camera)
        stream << -1;
    else
        stream << camera.server()->configId << camera.uniqueId();
    return stream;
}

QDataStream &operator>>(QDataStream &stream, LiveFeedWidget &widget)
{
    int serverId = -1, cameraId = -1;
    stream >> serverId;

    if (stream.status() != QDataStream::Ok || serverId < 0)
    {
        widget.clearCamera();
        return stream;
    }

    stream >> cameraId;
    widget.setCamera(DVRCamera::getCamera(serverId, cameraId));
    return stream;
}
