#include "MainWindow.h"
#include "CameraAreaWidget.h"
#include "CameraAreaControls.h"
#include "DVRServersView.h"
#include "OptionsDialog.h"
#include <QBoxLayout>
#include <QTreeView>
#include <QGroupBox>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{   
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
    QBoxLayout *middleLayout = new QVBoxLayout;
    middleLayout->setMargin(0);

    middleLayout->addWidget(createCameraArea());
    middleLayout->addWidget(createCameraControls());
    mainLayout->addLayout(middleLayout, 1);

    /* Set center widget */
    setCentralWidget(centerWidget);
}

MainWindow::~MainWindow()
{

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
    QGroupBox *box = new QGroupBox(tr("PTZ Controls"));

    /* Placeholder */
    box->setMinimumHeight(200);

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

void MainWindow::showOptionsDialog()
{
    OptionsDialog *dlg = new OptionsDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
