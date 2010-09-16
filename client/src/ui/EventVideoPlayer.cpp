#include "EventVideoPlayer.h"
#include "core/BluecherryApp.h"
#include <QBoxLayout>
#include <QSlider>
#include <QToolButton>
#include <QApplication>
#include <QThread>
#include <QDebug>

EventVideoPlayer::EventVideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    connect(&backend, SIGNAL(stateChanged(int,int)),
            SLOT(stateChanged(int)));
    connect(&backend, SIGNAL(durationChanged(qint64)), SLOT(durationChanged(qint64)));
    connect(&backend, SIGNAL(endOfStream()), SLOT(durationChanged()));

    connect(&m_posTimer, SIGNAL(timeout()), SLOT(updatePosition()));

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    m_videoWidget = backend.createSurface();
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_videoWidget->setMinimumSize(320, 240);
    layout->addWidget(m_videoWidget);

    m_seekSlider = new QSlider(Qt::Horizontal);
    connect(m_seekSlider, SIGNAL(valueChanged(int)), SLOT(seek(int)));
    //m_seekSlider->setEnabled(false);
    layout->addWidget(m_seekSlider);

    QBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(3);
    layout->addLayout(btnLayout);

    m_playBtn = new QToolButton;
    m_playBtn->setText(QString(QChar(0x25BA)));
    btnLayout->addWidget(m_playBtn);
    connect(m_playBtn, SIGNAL(clicked()), SLOT(playPause()));

    btnLayout->addSpacing(9);

    QToolButton *slowBtn = new QToolButton;
    slowBtn->setText(QString(2, QChar(0x25C4)));
    btnLayout->addWidget(slowBtn);

    QToolButton *fastBtn = new QToolButton;
    fastBtn->setText(QString(2, QChar(0x25BA)));
    btnLayout->addWidget(fastBtn);

    btnLayout->addSpacing(9);

    QToolButton *restartBtn = new QToolButton;
    restartBtn->setText(QString::fromUtf8("|\xe2\x97\x84"));
    btnLayout->addWidget(restartBtn);
    connect(restartBtn, SIGNAL(clicked()), SLOT(restart()));

    btnLayout->addStretch();
}

void EventVideoPlayer::setVideo(const QUrl &url)
{
    backend.start(url);
}

void EventVideoPlayer::clearVideo()
{
    backend.clear();
}

void EventVideoPlayer::playPause()
{
    if (backend.state() == VideoPlayerBackend::Playing)
    {
        backend.pause();
    }
    else
    {
        if (backend.atEnd())
            backend.restart();
        backend.play();
    }
}

void EventVideoPlayer::restart()
{
    backend.restart();
    backend.play();
}

void EventVideoPlayer::seek(int position)
{
    backend.seek(qint64(position) * 1000000);
}

void EventVideoPlayer::stateChanged(int state)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    qDebug("state change %d", state);
    if (state == VideoPlayerBackend::Playing)
    {
        m_playBtn->setText(QLatin1String("||"));
        m_posTimer.start(30);
    }
    else
    {
        m_playBtn->setText(QString(QChar(0x25BA)));
        m_posTimer.stop();
        updatePosition();
    }
}

void EventVideoPlayer::durationChanged(qint64 nsDuration)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    if (nsDuration == -1)
        nsDuration = backend.duration();

    /* Time is assumed to be nanoseconds; convert to milliseconds */
    int duration = int(nsDuration / 1000000);
    /* BUG: Shouldn't mindlessly chop to int */
    m_seekSlider->blockSignals(true);
    m_seekSlider->setMaximum(duration);
    m_seekSlider->blockSignals(false);
    updatePosition();
}

void EventVideoPlayer::updatePosition()
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    if (!m_seekSlider->maximum())
    {
        qint64 nsDuration = backend.duration();
        if (nsDuration && int(nsDuration / 1000000))
        {
            durationChanged(nsDuration);
            return;
        }
    }

    qint64 nsPosition = backend.position();
    int position = int(nsPosition / 1000000);
    m_seekSlider->blockSignals(true);
    m_seekSlider->setValue(position);
    m_seekSlider->blockSignals(false);
}
