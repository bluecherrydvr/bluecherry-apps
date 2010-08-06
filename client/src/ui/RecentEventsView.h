#ifndef RECENTEVENTSVIEW_H
#define RECENTEVENTSVIEW_H

#include "EventResultsView.h"

class RecentEventsView : public EventResultsView
{
    Q_OBJECT

public:
    explicit RecentEventsView(QWidget *parent = 0);
};

#endif // RECENTEVENTSVIEW_H
