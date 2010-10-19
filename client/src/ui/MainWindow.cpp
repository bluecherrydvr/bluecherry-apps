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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Bluecherry DVR"));
    resize(1100, 750);
    createMenu();

    QWidget *centerWidget = new QWidget;
    QBoxLayout *mainLayout = new QHBoxLayout(centerWidget);

    /* Create left side */
    QBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->setMargin(0);

    leftLayout->addWidget(createSourcesList());

    leftLayout->addWidget(createPtzBox());
    leftLayout->addWidget(createServerBox());

    mainLayout->addLayout(leftLayout);

    /* Middle */
    m_centerSplit = new QSplitter(Qt::Vertical);
    mainLayout->addWidget(m_centerSplit, 1);

    QWidget *cameraContainer = new QWidget;
    m_centerSplit->addWidget(cameraContainer);

    QBoxLayout *middleLayout = new QVBoxLayout(cameraContainer);
    middleLayout->setMargin(0);

    middleLayout->addWidget(createCameraArea(), 1);

    /* Controls area */
    QBoxLayout *controlLayout = new QHBoxLayout;
    middleLayout->addLayout(controlLayout);

    controlLayout->addWidget(createCameraControls(), 1);

    QPushButton *eventsBtn = new QPushButton(tr("Search Events"));
    connect(eventsBtn, SIGNAL(clicked()), SLOT(showEventsWindow()));
    controlLayout->addWidget(eventsBtn);

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
    serverMenu->addAction(tr("Configuration"), this, SLOT(openServerConfig()));
    serverMenu->addSeparator();
    serverMenu->addAction(tr("Edit Server"), this, SLOT(editCurrentServer()));

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

QWidget *MainWindow::createPtzBox()
{
    QGroupBox *box = new QGroupBox(tr("Controls"));
    QGridLayout *layout = new QGridLayout(box);
    layout->setColumnStretch(1, 1);

    int row = 0;

    const QString labels[] = { tr("Brightness:"), tr("Contrast:"), tr("Saturation:"), tr("Hue:") };
    for (int i = 0; i < 4; ++i)
    {
        QLabel *label = new QLabel(labels[i]);
        label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
        layout->addWidget(label, row+i, 0);
    }

    NumericOffsetWidget *brightness = new NumericOffsetWidget;
    layout->addWidget(brightness, row++, 1);

    NumericOffsetWidget *contrast = new NumericOffsetWidget;
    layout->addWidget(contrast, row++, 1);

    NumericOffsetWidget *saturation = new NumericOffsetWidget;
    layout->addWidget(saturation, row++, 1);

    NumericOffsetWidget *hue = new NumericOffsetWidget;
    layout->addWidget(hue, row++, 1);

    QCheckBox *allChk = new QCheckBox(tr("Apply to all cameras"));
    layout->addWidget(allChk, row++, 0, 1, 2, Qt::AlignCenter);

    return box;
}

QWidget *MainWindow::createServerBox()
{
    QGroupBox *box = new QGroupBox(tr("DVR Server Stats"));

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
