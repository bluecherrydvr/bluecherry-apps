#include "EventResultsView.h"
#include "EventsModel.h"
#include <QHeaderview>

EventResultsView::EventResultsView(QWidget *parent)
    : QTableView(parent)
{
    setModel(new EventsModel(this));
    setShowGrid(false);
    verticalHeader()->hide();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSortingEnabled(true);
    sortByColumn(3, Qt::DescendingOrder);

    horizontalHeader()->setHighlightSections(false);
    horizontalHeader()->setSortIndicatorShown(true);
}
