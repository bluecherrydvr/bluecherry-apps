#include "DVRServersView.h"
#include "DVRServersModel.h"
#include <QHeaderView>

DVRServersView::DVRServersView(QWidget *parent)
    : QTreeView(parent)
{
    setRootIsDecorated(false);
    header()->setVisible(false);
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);

    DVRServersModel *model = new DVRServersModel(this);
    setModel(model);

    /* We only show the server name in this list; hide other columns */
    for (int i = 1, n = model->columnCount(); i < n; ++i)
        header()->setSectionHidden(i, true);
}
