#include "RecentEventsView.h"
#include "EventsModel.h"
#include <QHeaderView>

RecentEventsView::RecentEventsView(QWidget *parent)
    : QTableView(parent)
{
    setModel(new EventsModel(this));
    setShowGrid(false);
    verticalHeader()->hide();
    setSelectionMode(QAbstractItemView::NoSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    QHeaderView *hz = horizontalHeader();
    hz->setHighlightSections(false);
}
