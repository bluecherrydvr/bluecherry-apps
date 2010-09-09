#include "EventVideoPlayer.h"
#include "core/BluecherryApp.h"
#include <QBoxLayout>
#include <QSlider>
#include <QToolButton>

EventVideoPlayer::EventVideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);

    m_videoWidget = new QWidget;
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_videoWidget->setMinimumSize(320, 240);
    layout->addWidget(m_videoWidget);

    QSlider *videoSlider = new QSlider(Qt::Horizontal);
    videoSlider->setEnabled(false);
    layout->addWidget(videoSlider);

    QBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setSpacing(3);
    layout->addLayout(btnLayout);

    QToolButton *playBtn = new QToolButton;
    playBtn->setText(QString(QChar(0x25BA)));
    btnLayout->addWidget(playBtn);
    connect(playBtn, SIGNAL(clicked()), SLOT(play()));

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

    btnLayout->addStretch();
}

void EventVideoPlayer::setVideo(const QUrl &url)
{
    backend.start(url, m_videoWidget);
}

void EventVideoPlayer::clearVideo()
{
    backend.clear();
}

void EventVideoPlayer::play()
{
    backend.play();
}
