#include "MainWindow.h"
#include "CameraAreaWidget.h"
#include "CameraAreaControls.h"
#include "DVRServersView.h"
#include "OptionsDialog.h"
#include "EventsWindow.h"
#include "NumericOffsetWidget.h"
#include "RecentEventsView.h"
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Bluecherry DVR"));
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
    QBoxLayout *middleLayout = new QVBoxLayout(cameraContainer);
    middleLayout->setMargin(0);

    middleLayout->addWidget(createCameraArea(), 1);
    middleLayout->addWidget(createCameraControls());
    m_centerSplit->addWidget(cameraContainer);
    m_centerSplit->setStretchFactor(0, 1);

    QWidget *container = new QWidget;
    m_centerSplit->addWidget(container);
    QBoxLayout *containerLayout = new QHBoxLayout(container);
    containerLayout->setMargin(0);

    containerLayout->addWidget(createRecentEvents());
    QPushButton *eventsBtn = new QPushButton(tr("Open Events"));
    connect(eventsBtn, SIGNAL(clicked()), SLOT(showEventsWindow()));
    containerLayout->addWidget(eventsBtn, 0, Qt::AlignTop);

    /* Set center widget */
    setCentralWidget(centerWidget);

    QSettings settings;
    restoreGeometry(settings.value(QLatin1String("ui/main/geometry")).toByteArray());
    m_centerSplit->restoreState(settings.value(QLatin1String("ui/main/centerSplit")).toByteArray());

    new QShortcut(QKeySequence(Qt::Key_F11), m_cameraArea, SLOT(toggleFullScreen()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QLatin1String("ui/main/geometry"), saveGeometry());
    settings.setValue(QLatin1String("ui/main/centerSplit"), m_centerSplit->saveState());
    QMainWindow::closeEvent(event);
}

void MainWindow::createMenu()
{
    QMenu *appMenu = menuBar()->addMenu(tr("&Application"));
    appMenu->addAction(tr("&Options"), this, SLOT(showOptionsDialog()));
    appMenu->addSeparator();
    appMenu->addAction(tr("&Quit"), this, SLOT(close()));
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
    return controls;
}

QWidget *MainWindow::createRecentEvents()
{
    m_eventsView = new RecentEventsView;
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
    EventsWindow *window = new EventsWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}
