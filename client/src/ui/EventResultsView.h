#ifndef EVENTRESULTSVIEW_H
#define EVENTRESULTSVIEW_H

#include <QListView>
#include "EventsModel.h"

class EventResultsView : public QListView
{
    Q_OBJECT

public:
    explicit EventResultsView(QWidget *parent = 0);

    EventsModel *eventsModel() const { return static_cast<EventsModel*>(model()); }

private:
    using QListView::setModel;
};

#endif // EVENTRESULTSVIEW_H
