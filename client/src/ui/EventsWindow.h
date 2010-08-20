#ifndef EVENTSWINDOW_H
#define EVENTSWINDOW_H

#include <QWidget>

class DVRServersView;
class EventResultsView;
class EventTimelineWidget;
class QLabel;
class QBoxLayout;
class QDateTimeEdit;
class QComboBox;
class QSlider;
class QModelIndex;

class EventsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EventsWindow(QWidget *parent = 0);

protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void setStartDateEnabled(bool enabled);
    void levelFilterChanged();
    void updateResultTitle();

    void timelineZoomChanged(int value);
    void timelineZoomRangeChanged(int min, int max);
    void timelineSliderChanged(int value);

    void showEvent(const QModelIndex &index);

private:
    DVRServersView *m_sourcesView;
    QDateTimeEdit *m_startDate;
    QComboBox *m_levelFilter;
    QLabel *m_resultTitle;
    EventResultsView *m_resultsView;
    EventTimelineWidget *m_timeline;
    QSlider *m_timelineZoom;

    void createDateFilter(QBoxLayout *layout);
    QWidget *createLevelFilter();
    QWidget *createTypeFilter();

    QWidget *createResultTitle();
    QWidget *createResultsView();
    QWidget *createTimeline();
};

#endif // EVENTSWINDOW_H
