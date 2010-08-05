#ifndef EVENTSWINDOW_H
#define EVENTSWINDOW_H

#include <QWidget>

class DVRServersView;

class EventsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit EventsWindow(QWidget *parent = 0);

private:
    DVRServersView *m_sourcesView;
};

#endif // EVENTSWINDOW_H
