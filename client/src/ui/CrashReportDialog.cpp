#include "CrashReportDialog.h"
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QCommandLinkButton>
#include <QProgressBar>
#include <QStyle>
#include <QFile>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QApplication>
#include <QStackedLayout>
#include <QNetworkReply>
#include <QDialogButtonBox>
#include <QPropertyAnimation>
#include <QShowEvent>

static const char * const reportUrl = "???";

CrashReportDialog::CrashReportDialog(const QString &dumpFile, QWidget *parent)
    : QDialog(parent), m_dumpFile(dumpFile), m_uploadReply(0), m_finalResult(QDialog::Accepted)
{
    setWindowTitle(tr("Bluecherry DVR - Report Crash"));
    setFixedSize(430, 270);

    QBoxLayout *mainLayout = new QVBoxLayout(this);

    QBoxLayout *topLayout = new QHBoxLayout;
    mainLayout->addLayout(topLayout);

    QLabel *icon = new QLabel;
    icon->setPixmap(style()->standardPixmap(QStyle::SP_MessageBoxCritical));
    topLayout->addWidget(icon, 0, Qt::AlignLeft | Qt::AlignVCenter);

    QLabel *title = new QLabel(tr("The Bluecherry DVR client crashed!"));
    title->setStyleSheet(QLatin1String("font-size:16px;color:#003399;"));
    title->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(title, 1, Qt::AlignCenter);

    m_stackLayout = new QStackedLayout(mainLayout);

    QWidget *container = new QWidget;
    m_stackLayout->addWidget(container);
    QBoxLayout *layout = new QVBoxLayout(container);
    layout->setMargin(0);

    layout->addStretch();

    m_allowReport = new QCheckBox(tr("Send an automatic crash report"));
    m_allowReport->setChecked(true);
    layout->addWidget(m_allowReport, 0, Qt::AlignCenter);

    QBoxLayout *emailLayout = new QHBoxLayout;
    layout->addLayout(emailLayout);

    QLabel *label = new QLabel(tr("Email:"));
    emailLayout->addWidget(label);

    m_emailInput = new QLineEdit;
    emailLayout->addWidget(m_emailInput);

    label = new QLabel(tr("If you enter your email address, we will update you on our progress with this bug"));
    label->setStyleSheet(QLatin1String("color:#606060;font-size:9px"));
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignRight | Qt::AlignTop);
    layout->addWidget(label);

    layout->addStretch();

    QCommandLinkButton *restartBtn = new QCommandLinkButton(tr("Restart Client"));
    restartBtn->setDefault(true);
    connect(restartBtn, SIGNAL(clicked()), SLOT(uploadAndRestart()));
    layout->addWidget(restartBtn);

    QCommandLinkButton *exitBtn = new QCommandLinkButton(tr("Exit"));
    connect(exitBtn, SIGNAL(clicked()), SLOT(uploadAndExit()));
    layout->addWidget(exitBtn);

    /* Upload progress widgets */
    QWidget *uploadContainer = new QWidget;
    m_stackLayout->addWidget(uploadContainer);

    QBoxLayout *uploadLayout = new QVBoxLayout(uploadContainer);
    uploadLayout->setMargin(0);
    uploadLayout->addStretch();

    label = new QLabel(tr("Sending crash report..."));
    uploadLayout->addWidget(label);

    m_uploadProgress = new QProgressBar;
    m_uploadProgress->setTextVisible(false);
    uploadLayout->addWidget(m_uploadProgress);

    uploadLayout->addStretch();

    QDialogButtonBox *btn = new QDialogButtonBox(QDialogButtonBox::Cancel, Qt::Horizontal);
    uploadLayout->addWidget(btn);
}

CrashReportDialog::~CrashReportDialog()
{
    if (m_uploadReply)
    {
        m_uploadReply->abort();
        delete m_uploadReply;
    }
}

void CrashReportDialog::showEvent(QShowEvent *event)
{
    if (!event->spontaneous())
    {
        activateWindow();
        raise();
    }

    QDialog::showEvent(event);
}

void CrashReportDialog::uploadAndRestart()
{
    m_finalResult = QDialog::Accepted;
    sendReport();
}

void CrashReportDialog::uploadAndExit()
{
    m_finalResult = QDialog::Rejected;
    sendReport();
}

void CrashReportDialog::sendReport()
{
    if (m_uploadReply)
        return;

    QString platform =
#if defined(Q_OS_WIN)
                       QLatin1String("windows");
#elif defined(Q_OS_MAC)
                       QLatin1String("mac");
#elif defined(Q_OS_LINUX)
                       QLatin1String("linux");
#else
                       QLatin1String("unknown");
#endif

    /* NAM will be freed during destruct */
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);

    QUrl url(QString::fromLatin1(reportUrl));
    url.addQueryItem(QLatin1String("version"), qApp->applicationVersion());
    url.addQueryItem(QLatin1String("platform"), platform);
    if (!m_emailInput->text().isEmpty())
        url.addQueryItem(QLatin1String("email"), m_emailInput->text());

    if (!m_dumpFile.exists() || !m_dumpFile.open(QIODevice::ReadOnly))
    {
        url.addQueryItem(QLatin1String("dump"), QLatin1String("0"));
        m_uploadReply = nam->get(QNetworkRequest(url));
    }
    else
        m_uploadReply = nam->post(QNetworkRequest(url), &m_dumpFile);

    connect(m_uploadReply, SIGNAL(finished()), SLOT(uploadFinished()));
    bool ok = connect(m_uploadReply, SIGNAL(uploadProgress(qint64,qint64)), SLOT(setUploadProgress(qint64)));
    Q_ASSERT(ok);
    m_uploadProgress->setMaximum((int)m_dumpFile.size());

    m_stackLayout->setCurrentIndex(1);

    setFixedHeight(QWIDGETSIZE_MAX);
    QPropertyAnimation *resizeAnimation = new QPropertyAnimation(this, "size");
    resizeAnimation->setEndValue(QSize(width(), minimumSizeHint().height()));
    resizeAnimation->setDuration(150);
    resizeAnimation->setEasingCurve(QEasingCurve::InQuad);
    resizeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void CrashReportDialog::setUploadProgress(qint64 done)
{
    m_uploadProgress->setValue((int)done);
}

void CrashReportDialog::uploadFinished()
{
    m_uploadReply->deleteLater();
    m_uploadReply = 0;

    done(m_finalResult);
}
