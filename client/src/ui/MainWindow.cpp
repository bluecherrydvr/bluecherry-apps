#include "MainWindow.h"
#include "CameraAreaWidget.h"
#include "CameraAreaControls.h"
#include "DVRServersView.h"
#include "OptionsDialog.h"
#include "EventsWindow.h"
#include "NumericOffsetWidget.h"
#include "RecentEventsView.h"
#include "EventsModel.h"
#include "AboutDialog.h"
#include "OptionsServerPage.h"
#include "ServerConfigWindow.h"
#include "core/DVRServer.h"
#include "core/BluecherryApp.h"
#include <QBoxLayout>
#include <QTreeView>
#include <QGroupBox>
#include <QMenuBar>
#include <QLabel>
#include <QCheckBox>
#include <QSettings>
#include <QShortcut>
#include <QSplitter>
#include <QPushButton>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QMacStyle>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QTextDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Bluecherry"));
    resize(1100, 750);
    createMenu();

    QWidget *centerWidget = new QWidget;
    QBoxLayout *mainLayout = new QHBoxLayout(centerWidget);
    mainLayout->setSpacing(5);

    /* Create left side */
    QBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setMargin(0);

    leftLayout->addWidget(createSourcesList());

    //leftLayout->addWidget(createServerBox());

    mainLayout->addLayout(leftLayout);

    /* Middle */
    m_centerSplit = new QSplitter(Qt::Vertical);
    mainLayout->addWidget(m_centerSplit, 1);

    QFrame *cameraContainer = new QFrame;
    m_centerSplit->addWidget(cameraContainer);

    QBoxLayout *middleLayout = new QVBoxLayout(cameraContainer);
    middleLayout->setSpacing(0);
    middleLayout->setMargin(0);

    createCameraArea();

    /* Controls area */
    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->setSpacing(0);
    middleLayout->addLayout(controlLayout);
    middleLayout->addWidget(m_cameraArea, 1);

    QWidget *controls = createCameraControls();
    controlLayout->addWidget(controls, 1);

#ifdef Q_OS_MAC
    if (qobject_cast<QMacStyle*>(style()))
    {
        m_cameraArea->setFrameStyle(QFrame::NoFrame);
        cameraContainer->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    }
#else
    controls->setStyleSheet(QLatin1String("QToolBar { border: none; }"));
#endif

    QPushButton *eventsBtn = new QPushButton(tr("Search Events"));
    connect(eventsBtn, SIGNAL(clicked()), SLOT(showEventsWindow()));
    leftLayout->addWidget(eventsBtn, 0, Qt::AlignCenter);

    /* Recent events */
    QWidget *eventsWidget = createRecentEvents();
    m_centerSplit->addWidget(eventsWidget);
    m_centerSplit->setStretchFactor(0, 1);

    /* Set center widget */
    setCentralWidget(centerWidget);

    QSettings settings;
    restoreGeometry(settings.value(QLatin1String("ui/main/geometry")).toByteArray());
    if (!m_centerSplit->restoreState(settings.value(QLatin1String("ui/main/centerSplit")).toByteArray()))
    {
        m_centerSplit->setSizes(QList<int>() << 1000 << 100);
    }

    new QShortcut(QKeySequence(Qt::Key_F11), m_cameraArea, SLOT(toggleFullScreen()));

    connect(bcApp, SIGNAL(sslConfirmRequired(DVRServer*,QList<QSslError>,QSslConfiguration)),
            SLOT(sslConfirmRequired(DVRServer*,QList<QSslError>,QSslConfiguration)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit closing();

    QSettings settings;
    settings.setValue(QLatin1String("ui/main/geometry"), saveGeometry());
    settings.setValue(QLatin1String("ui/main/centerSplit"), m_centerSplit->saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::createMenu()
{
    QMenu *appMenu = menuBar()->addMenu(tr("&Application"));
    appMenu->addAction(tr("Add new &server..."), this, SLOT(addServer()));
    appMenu->addAction(tr("&Options"), this, SLOT(showOptionsDialog()));
    appMenu->addSeparator();
    appMenu->addAction(tr("&Quit"), this, SLOT(close()));

    QMenu *serverMenu = menuBar()->addMenu(tr("&Server"));
    menuServerName = serverMenu->addAction(QString());
    QFont f;
    f.setBold(true);
    menuServerName->setFont(f);
    serverMenu->addSeparator();
    serverMenu->addAction(tr("&Configuration"), this, SLOT(openServerConfig()));
    serverMenu->addAction(tr("&Refresh devices"), this, SLOT(refreshServerDevices()));
    serverMenu->addSeparator();
    serverMenu->addAction(tr("&Edit Server"), this, SLOT(editCurrentServer()));

    connect(serverMenu, SIGNAL(aboutToShow()), SLOT(showServersMenu()));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Documentation"), this, SLOT(openDocumentation()));
    helpMenu->addAction(tr("Bluecherry &support"), this, SLOT(openSupport()));
    helpMenu->addAction(tr("Suggest a &feature"), this, SLOT(openIdeas()));
    helpMenu->addSeparator();
    helpMenu->addAction(tr("&About Bluecherry DVR"), this, SLOT(openAbout()));
}

void MainWindow::showServersMenu()
{
    DVRServer *server = m_sourcesList->currentServer();
    menuServerName->setText(server ? server->displayName() : tr("No Server"));
}

QWidget *MainWindow::createSourcesList()
{
    m_sourcesList = new DVRServersView;
    m_sourcesList->setMinimumHeight(220);
    m_sourcesList->setFixedWidth(170);

    return m_sourcesList;
}

QWidget *MainWindow::createServerBox()
{
    QGroupBox *box = new QGroupBox(tr("Server Information"));

    /* Placeholder */
    box->setMinimumHeight(200);

    return box;
}

QWidget *MainWindow::createCameraArea()
{
    m_cameraArea = new CameraAreaWidget;
    return m_cameraArea;
}

QWidget *MainWindow::createCameraControls()
{
    CameraAreaControls *controls = new CameraAreaControls(m_cameraArea);
    connect(this, SIGNAL(closing()), controls, SLOT(saveLayout()));
    return controls;
}

QWidget *MainWindow::createRecentEvents()
{
    m_eventsView = new RecentEventsView;
    m_eventsView->setModel(new EventsModel(m_eventsView));
    return m_eventsView;
}

void MainWindow::showOptionsDialog()
{
    OptionsDialog *dlg = new OptionsDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void MainWindow::showEventsWindow()
{
    EventsWindow *window = EventsWindow::instance();
    window->show();
    window->raise();
}

void MainWindow::openAbout()
{
    foreach (QWidget *w, QApplication::topLevelWidgets())
    {
        if (qobject_cast<AboutDialog*>(w))
        {
            w->raise();
            return;
        }
    }

    AboutDialog *dlg = new AboutDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

void MainWindow::openDocumentation()
{
    /* TODO: Get a link for this */
}

void MainWindow::openIdeas()
{
    QUrl url(QLatin1String("http://ideas.bluecherrydvr.com/"));
    bool ok = QDesktopServices::openUrl(url);
    if (!ok)
        QMessageBox::critical(this, tr("Error"), tr("An error occurred while opening %1").arg(url.toString()));
}

void MainWindow::openSupport()
{
    QUrl url(QLatin1String("http://support.bluecherrydvr.com/tickets/new"));
    bool ok = QDesktopServices::openUrl(url);
    if (!ok)
        QMessageBox::critical(this, tr("Error"), tr("An error occurred while opening %1").arg(url.toString()));
}

void MainWindow::addServer()
{
    OptionsDialog *dlg = new OptionsDialog(this);
    dlg->showPage(OptionsDialog::ServerPage);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    OptionsServerPage *pg = static_cast<OptionsServerPage*>(dlg->pageWidget(OptionsDialog::ServerPage));
    pg->addNewServer();

    dlg->show();
}

void MainWindow::editCurrentServer()
{
    DVRServer *server = m_sourcesList->currentServer();
    if (!server)
        return;

    OptionsDialog *dlg = new OptionsDialog(this);
    dlg->showPage(OptionsDialog::ServerPage);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    OptionsServerPage *pg = static_cast<OptionsServerPage*>(dlg->pageWidget(OptionsDialog::ServerPage));
    pg->setCurrentServer(server);

    dlg->show();
}

void MainWindow::openServerConfig()
{
    DVRServer *server = m_sourcesList->currentServer();
    if (!server)
        return;

    ServerConfigWindow::instance()->setServer(server);
    ServerConfigWindow::instance()->show();
    ServerConfigWindow::instance()->raise();
}

void MainWindow::refreshServerDevices()
{
    DVRServer *server = m_sourcesList->currentServer();
    if (!server)
        return;

    server->updateCameras();
}

void MainWindow::sslConfirmRequired(DVRServer *server, const QList<QSslError> &errors, const QSslConfiguration &config)
{
    Q_UNUSED(errors);
    Q_ASSERT(server);
    Q_ASSERT(!config.peerCertificate().isNull());

    QByteArray digest = config.peerCertificate().digest(QCryptographicHash::Sha1);
    QString fingerprint = QString::fromLatin1(digest.toHex()).toUpper();
    for (int i = 4; i < fingerprint.size(); i += 5)
        fingerprint.insert(i, QLatin1Char(' '));

    QMessageBox *dlg = new QMessageBox(QMessageBox::Warning, tr("Security warning"),
                                       tr("The SSL certificate for <b>%1</b> has changed! This could indicate an "
                                          "attack on the secure connection, or that the server has recently been "
                                          "reinstalled.<br><br><b>Server:</b> %1<br><b>URL:</b> %2<br>"
                                          "<b>Fingerprint:</b> %3<br><br>Do you want to connect anyway, and trust "
                                          "this certificate in the future?")
                                       .arg(Qt::escape(server->displayName()), server->api->serverUrl().toString(),
                                            fingerprint));
    QPushButton *ab = dlg->addButton(tr("Accept Certificate"), QMessageBox::AcceptRole);
    dlg->setDefaultButton(dlg->addButton(QMessageBox::Cancel));
    dlg->setParent(this);
    dlg->setWindowModality(Qt::WindowModal);

    dlg->exec();
    if (dlg->clickedButton() != ab)
        return;

    server->setKnownCertificate(config.peerCertificate());
}
