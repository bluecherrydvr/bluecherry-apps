#include "EventResultsView.h"
#include "EventsModel.h"
#include "RecentEventDelegate.h"

EventResultsView::EventResultsView(QWidget *parent)
    : QListView(parent)
{
    setModel(new EventsModel(this));
    setItemDelegate(new RecentEventDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectRows);
}
