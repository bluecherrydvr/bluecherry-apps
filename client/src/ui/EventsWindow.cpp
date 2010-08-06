#include "EventsWindow.h"
#include "DVRServersView.h"
#include "CameraSourcesModel.h"
#include "EventResultsView.h"
#include <QBoxLayout>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QTreeView>
#include <QSettings>

EventsWindow::EventsWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Bluecherry DVR - Event Viewer"));
    resize(QSize(900, 650));

    QBoxLayout *layout = new QHBoxLayout(this);
    QBoxLayout *filtersLayout = new QVBoxLayout;
    layout->addLayout(filtersLayout);

    /* Filters */
    m_sourcesView = new DVRServersView;
    m_sourcesView->setModel(new CameraSourcesModel(m_sourcesView));
    m_sourcesView->setMaximumWidth(180);
    filtersLayout->addWidget(m_sourcesView);

    createDateFilter(filtersLayout);

    QLabel *label = new QLabel(tr("Minimum Level"));
    label->setStyleSheet(QLatin1String("font-weight:bold;"));
    filtersLayout->addWidget(label);
    filtersLayout->addWidget(createLevelFilter());

    label = new QLabel(tr("Type"));
    label->setStyleSheet(QLatin1String("font-weight:bold;"));
    filtersLayout->addWidget(label);
    filtersLayout->addWidget(createTypeFilter());

    /* Results */
    QBoxLayout *resultLayout = new QVBoxLayout;
    layout->addLayout(resultLayout, 1);
    resultLayout->addWidget(createResultTitle());
    resultLayout->addWidget(createResultsView());
    resultLayout->addWidget(createTimeline());

    /* Settings */
    QSettings settings;
    restoreGeometry(settings.value(QLatin1String("ui/events/geometry")).toByteArray());
}

void EventsWindow::createDateFilter(QBoxLayout *layout)
{
    QCheckBox *title = new QCheckBox(tr("Starting Date"));
    title->setStyleSheet(QLatin1String("font-weight:bold;"));
    layout->addWidget(title);

    QDateTimeEdit *edit = new QDateTimeEdit(QDateTime::currentDateTime());
    edit->setCalendarPopup(true);
    layout->addWidget(edit);

    connect(title, SIGNAL(clicked(bool)), edit, SLOT(setEnabled(bool)));
    title->setChecked(false);
    edit->setEnabled(false);
}

QWidget *EventsWindow::createLevelFilter()
{
    QComboBox *levelFilter = new QComboBox;
    levelFilter->addItem(tr("Any"));
    levelFilter->addItem(tr("Info"));
    levelFilter->addItem(tr("Warning"));
    levelFilter->addItem(tr("Alarm"));
    levelFilter->addItem(tr("Critical"));
    return levelFilter;
}

QWidget *EventsWindow::createTypeFilter()
{
    /* Temporary */
    QTreeView *tmp = new QTreeView;
    tmp->setMaximumWidth(180);
    return tmp;
}

QWidget *EventsWindow::createResultTitle()
{
    m_resultTitle = new QLabel(tr("Most recent events on all cameras"));
    QFont font = m_resultTitle->font();
    font.setPointSize(font.pointSize()+4);
    m_resultTitle->setFont(font);

    return m_resultTitle;
}

QWidget *EventsWindow::createResultsView()
{
    return new EventResultsView;
}

QWidget *EventsWindow::createTimeline()
{
    QWidget *placeholder = new QWidget;
    placeholder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    return placeholder;
}

void EventsWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QLatin1String("ui/events/geometry"), saveGeometry());
    QWidget::closeEvent(event);
}
