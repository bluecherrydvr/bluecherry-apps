#ifndef VIDEOSURFACE_H
#define VIDEOSURFACE_H

#include <QFrame>
#include <QDebug>

class VideoSurface : public
#ifdef Q_WS_MAC
        QMacCocoaViewContainer
#else
        QWidget
#endif
{
    Q_OBJECT
    Q_PROPERTY(QSize videoSize READ videoSize WRITE setVideoSize NOTIFY videoSizeChanged)

public:
    VideoSurface(QWidget *parent = 0)
#ifdef Q_WS_MAC
        : QMacCocoaViewContainer(parent)
#else
        : QWidget(parent)
#endif
    {
    }

#ifndef Q_WS_MAC
    virtual QPaintEngine *paintEngine() const
    {
        return 0;
    }
#endif

    QSize videoSize() const { return m_videoSize; }

    virtual QSize sizeHint() const
    {
        return m_videoSize;
    }

    QSize scaled(const QSize &area) const
    {
        QSize sz = m_videoSize;
        sz.scale(area, Qt::KeepAspectRatio);
        return sz;
    }

public slots:
    void setVideoSize(const QSize &size)
    {
        m_videoSize = size;
        updateGeometry();
        emit videoSizeChanged(size);
    }

signals:
    void videoSizeChanged(const QSize &size);

private:
    QSize m_videoSize;
};

class VideoContainer : public QFrame
{
    Q_OBJECT

public:
    VideoSurface * const videoSurface;

    VideoContainer(VideoSurface *surface, QWidget *parent = 0)
        : QFrame(parent), videoSurface(surface)
    {
        connect(surface, SIGNAL(videoSizeChanged(QSize)), SLOT(updateSurface()));

        surface->setParent(this);
        setMinimumSize(320, 240);
        setFrameStyle(QFrame::Sunken | QFrame::Panel);
        setAutoFillBackground(true);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QPalette p = palette();
        p.setColor(QPalette::Window, Qt::black);
        p.setColor(QPalette::WindowText, Qt::white);
        setPalette(p);
    }

public slots:
    void updateSurface()
    {
        QSize pSz = contentsRect().size();
        QSize sz = videoSurface->scaled(pSz);
        QPoint pos(qRound((pSz.width()-sz.width())/2.0), qRound((pSz.height()-sz.height())/2.0));
        videoSurface->setGeometry(QRect(pos + contentsRect().topLeft(), sz));
    }

protected:
    virtual void resizeEvent(QResizeEvent *ev)
    {
        updateSurface();
        QFrame::resizeEvent(ev);
    }
};


#endif // VIDEOSURFACE_H
