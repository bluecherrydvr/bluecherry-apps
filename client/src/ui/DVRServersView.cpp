#include "DVRServersView.h"
#include "DVRServersModel.h"
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>

DVRServersView::DVRServersView(QWidget *parent)
    : QTreeView(parent)
{
    setRootIsDecorated(false);
    header()->setVisible(false);
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
    setContextMenuPolicy(Qt::DefaultContextMenu);

    DVRServersModel *model = new DVRServersModel(this);
    setModel(model);

    /* We only show the server name in this list; hide other columns */
    for (int i = 1, n = model->columnCount(); i < n; ++i)
        header()->setSectionHidden(i, true);
}

void DVRServersView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QModelIndex index = indexAt(event->pos());
    if (index.isValid())
    {
        menu.addAction(tr("Connect"))->setEnabled(false);
        menu.addAction(tr("Edit server"));
        menu.addSeparator();
    }

    menu.addAction("Add new server...");
    menu.addAction("Options");

    menu.exec(event->globalPos());
}
