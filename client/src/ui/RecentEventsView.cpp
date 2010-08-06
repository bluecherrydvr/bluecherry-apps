#include "RecentEventsView.h"

RecentEventsView::RecentEventsView(QWidget *parent)
    : EventResultsView(parent)
{
    setSelectionMode(QAbstractItemView::NoSelection);
}
