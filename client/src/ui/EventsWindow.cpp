#include "EventsWindow.h"
#include "DVRServersView.h"
#include "EventSourcesModel.h"
#include "EventResultsView.h"
#include "EventTimelineWidget.h"
#include "EventViewWindow.h"
#include <QBoxLayout>
#include <QGridLayout>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QTreeView>
#include <QSettings>
#include <QSlider>
#include <QSplitter>

EventsWindow::EventsWindow(QWidget *parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Bluecherry DVR - Event Viewer"));
    resize(QSize(900, 650));

    QBoxLayout *layout = new QHBoxLayout(this);
    QBoxLayout *filtersLayout = new QVBoxLayout;
    layout->addLayout(filtersLayout);

    createResultsView();

    /* Filters */
    m_sourcesView = new DVRServersView;
    EventSourcesModel *sourcesModel = new EventSourcesModel(m_sourcesView);
    m_sourcesView->setModel(sourcesModel);
    m_sourcesView->setMaximumWidth(180);
    filtersLayout->addWidget(m_sourcesView);

    connect(sourcesModel, SIGNAL(checkedSourcesChanged(QMap<DVRServer*,QList<int>>)),
            m_resultsView->eventsModel(), SLOT(setFilterSources(QMap<DVRServer*,QList<int>>)));

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

    QSplitter *resultSplitter = new QSplitter(Qt::Vertical);
    resultLayout->addWidget(resultSplitter);

    resultSplitter->addWidget(m_resultsView);
    resultSplitter->addWidget(createTimeline());

    /* Settings */
    QSettings settings;
    restoreGeometry(settings.value(QLatin1String("ui/events/geometry")).toByteArray());
}

void EventsWindow::createDateFilter(QBoxLayout *layout)
{
    QCheckBox *title = new QCheckBox(tr("Starting Date"));
    title->setStyleSheet(QLatin1String("font-weight:bold;"));
    layout->addWidget(title);

    m_startDate = new QDateTimeEdit(QDateTime::currentDateTime());
    m_startDate->setCalendarPopup(true);
    m_startDate->setMaximumDate(QDate::currentDate());
    layout->addWidget(m_startDate);

    connect(m_startDate, SIGNAL(dateTimeChanged(QDateTime)), m_resultsView->eventsModel(),
            SLOT(setFilterBeginDate(QDateTime)));
    connect(title, SIGNAL(clicked(bool)), this, SLOT(setStartDateEnabled(bool)));
    title->setChecked(false);
    m_startDate->setEnabled(false);
}

QWidget *EventsWindow::createLevelFilter()
{
    m_levelFilter = new QComboBox;
    m_levelFilter->addItem(tr("Any"), -1);
    m_levelFilter->addItem(tr("Info"), EventLevel::Info);
    m_levelFilter->addItem(tr("Warning"), EventLevel::Warning);
    m_levelFilter->addItem(tr("Alarm"), EventLevel::Alarm);
    m_levelFilter->addItem(tr("Critical"), EventLevel::Critical);

    connect(m_levelFilter, SIGNAL(currentIndexChanged(int)), SLOT(levelFilterChanged()));
    return m_levelFilter;
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
    m_resultTitle = new QLabel;
    QFont font = m_resultTitle->font();
    font.setPointSize(font.pointSize()+4);
    m_resultTitle->setFont(font);
    m_resultTitle->setWordWrap(true);

    connect(m_resultsView->eventsModel(), SIGNAL(filtersChanged()), SLOT(updateResultTitle()));
    updateResultTitle();

    return m_resultTitle;
}

QWidget *EventsWindow::createResultsView()
{
    m_resultsView = new EventResultsView;
    connect(m_resultsView, SIGNAL(doubleClicked(QModelIndex)), SLOT(showEvent(QModelIndex)));
    return m_resultsView;
}

QWidget *EventsWindow::createTimeline()
{
    QWidget *container = new QWidget;
    QGridLayout *layout = new QGridLayout(container);
    layout->setMargin(0);

    m_timeline = new EventTimelineWidget;
    m_timeline->setModel(m_resultsView->eventsModel());
    m_timeline->setSelectionModel(m_resultsView->selectionModel());

    m_timelineZoom = new QSlider(Qt::Horizontal);
    timelineZoomRangeChanged(m_timeline->minZoomSeconds(), m_timeline->maxZoomSeconds());
    timelineZoomChanged(m_timeline->zoomSeconds());

    connect(m_timeline, SIGNAL(doubleClicked(QModelIndex)), SLOT(showEvent(QModelIndex)));

    connect(m_timeline, SIGNAL(zoomSecondsChanged(int)), SLOT(timelineZoomChanged(int)));
    connect(m_timeline, SIGNAL(zoomRangeChanged(int,int)), SLOT(timelineZoomRangeChanged(int,int)));

    connect(m_timelineZoom, SIGNAL(valueChanged(int)), SLOT(timelineSliderChanged(int)));

    layout->addWidget(m_timeline, 0, 0, 1, 2);

    QLabel *label = new QLabel(tr("Zoom:"));
    layout->addWidget(label, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    layout->addWidget(m_timelineZoom, 1, 1);
    return container;
}

void EventsWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue(QLatin1String("ui/events/geometry"), saveGeometry());
    QWidget::closeEvent(event);
}

void EventsWindow::setStartDateEnabled(bool enabled)
{
    m_startDate->setEnabled(enabled);
    if (enabled)
        m_resultsView->eventsModel()->setFilterBeginDate(m_startDate->dateTime());
    else
        m_resultsView->eventsModel()->setFilterBeginDate(QDateTime());
}

void EventsWindow::levelFilterChanged()
{
    int level = m_levelFilter->itemData(m_levelFilter->currentIndex()).toInt();
    if (level < 0)
        level = EventLevel::Minimum;

    m_resultsView->eventsModel()->setFilterLevel((EventLevel::Level)level);
}

void EventsWindow::updateResultTitle()
{
    m_resultTitle->setText(m_resultsView->eventsModel()->filterDescription());
}

void EventsWindow::timelineZoomChanged(int value)
{
    m_timelineZoom->setValue(m_timelineZoom->maximum() - (value - m_timelineZoom->minimum()));
}

void EventsWindow::timelineSliderChanged(int value)
{
    m_timeline->setZoomSeconds(m_timelineZoom->maximum() - (value - m_timelineZoom->minimum()));
}

void EventsWindow::timelineZoomRangeChanged(int min, int max)
{
    bool block = m_timelineZoom->blockSignals(true);
    m_timelineZoom->setRange(min, max);
    m_timelineZoom->blockSignals(block);
}

void EventsWindow::showEvent(const QModelIndex &index)
{
    EventData *data = index.data(EventsModel::EventDataPtr).value<EventData*>();

    EventViewWindow *window = new EventViewWindow(this);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->setEvent(data);
    window->show();
}
