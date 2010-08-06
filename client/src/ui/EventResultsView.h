#ifndef EVENTRESULTSVIEW_H
#define EVENTRESULTSVIEW_H

#include <QTableView>
#include "EventsModel.h"

class EventResultsView : public QTableView
{
    Q_OBJECT

public:
    explicit EventResultsView(QWidget *parent = 0);

    EventsModel *eventsModel() const { return static_cast<EventsModel*>(model()); }

private:
    using QTableView::setModel;
};

#endif // EVENTRESULTSVIEW_H
