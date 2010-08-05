#include "EventsWindow.h"
#include "DVRServersView.h"
#include "CameraSourcesModel.h"
#include <QBoxLayout>

EventsWindow::EventsWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Bluecherry DVR - Event Viewer"));

    QBoxLayout *layout = new QHBoxLayout(this);

    m_sourcesView = new DVRServersView;
    m_sourcesView->setModel(new CameraSourcesModel(m_sourcesView));
    layout->addWidget(m_sourcesView);

    layout->addStretch();
}
