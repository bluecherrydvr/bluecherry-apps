#ifndef EVENTSWINDOW_H
#define EVENTSWINDOW_H

#include <QWidget>

class DVRServersView;
class QLabel;
class QBoxLayout;

class EventsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EventsWindow(QWidget *parent = 0);

protected:
    virtual void closeEvent(QCloseEvent *event);

private:
    DVRServersView *m_sourcesView;
    QLabel *m_resultTitle;

    void createDateFilter(QBoxLayout *layout);
    QWidget *createLevelFilter();
    QWidget *createTypeFilter();

    QWidget *createResultTitle();
    QWidget *createResultsView();
    QWidget *createTimeline();
};

#endif // EVENTSWINDOW_H
