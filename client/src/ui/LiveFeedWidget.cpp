#include "LiveFeedWidget.h"
#include "core/DVRCamera.h"
#include <QPainter>
#include <QPaintEvent>

LiveFeedWidget::LiveFeedWidget(QWidget *parent)
    : QWidget(parent), m_camera(0)
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
}

void LiveFeedWidget::setCamera(DVRCamera *camera)
{
    if (camera == m_camera)
        return;

    m_camera = camera;
    updateGeometry();
    update();
}

QSize LiveFeedWidget::sizeHint() const
{
    if (!camera())
        return QSize();

    return QSize(320, 240);
}

void LiveFeedWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect r = rect();
    p.eraseRect(r);

    if (!m_camera)
        return;

    p.drawText(r, Qt::AlignTop | Qt::AlignRight, m_camera->displayName());
}

void LiveFeedWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QLatin1String("application/x-bluecherry-dvrcamera")))
        event->acceptProposedAction();
}

void LiveFeedWidget::dropEvent(QDropEvent *event)
{
    QByteArray data = event->mimeData()->data(QLatin1String("application/x-bluecherry-dvrcamera"));
    QDataStream stream(&data, QIODevice::ReadOnly);

    /* Ignore everything except the first camera dropped */
    int serverid, cameraid;
    stream >> serverid >> cameraid;

    if (stream.status() != QDataStream::Ok)
        return;

    DVRCamera *camera = DVRCamera::findByID(serverid, cameraid);
    if (camera)
    {
        setCamera(camera);
        event->acceptProposedAction();
    }
}
