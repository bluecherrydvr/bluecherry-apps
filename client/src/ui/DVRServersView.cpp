#include "DVRServersView.h"
#include "DVRServersModel.h"
#include "OptionsDialog.h"
#include "OptionsServerPage.h"
#include "core/DVRServer.h"
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>

DVRServersView::DVRServersView(QWidget *parent)
    : QTreeView(parent)
{
    header()->setVisible(false);
    setEditTriggers(QAbstractItemView::EditKeyPressed | QAbstractItemView::DoubleClicked);
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setDragEnabled(true);

    DVRServersModel *model = new DVRServersModel(this);
    setModel(model);

    /* We only show the server name in this list; hide other columns */
    for (int i = 1, n = model->columnCount(); i < n; ++i)
        header()->setSectionHidden(i, true);
}

void DVRServersView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *aEditServer = 0, *aOptions = 0, *aAddServer = 0;
    QAction *aSelectOnly = 0, *aSelectElse = 0;

    QModelIndex index = indexAt(event->pos());
    DVRServer *server = index.data(DVRServersModel::ServerPtrRole).value<DVRServer*>();
    if (index.isValid())
    {
        if (index.flags() & Qt::ItemIsUserCheckable)
        {
            aSelectOnly = menu.addAction(tr("Select only this"));
            aSelectElse = menu.addAction(tr("Select everything else"));
            menu.addSeparator();
        }

        if (server)
        {
            menu.addAction(tr("Connect"))->setEnabled(false);
            aEditServer = menu.addAction(tr("Edit server"));
        }
    }
    else
    {
        aAddServer = menu.addAction(tr("Add new server..."));
        aOptions = menu.addAction(tr("Options"));
    }

    QAction *action = menu.exec(event->globalPos());
    if (!action)
        return;

    if (action == aEditServer || action == aAddServer || action == aOptions)
    {
        /* Open the options dialog and modify appropriately */
        OptionsDialog *dlg = new OptionsDialog(this);
        dlg->showPage(OptionsDialog::ServerPage);
        dlg->setAttribute(Qt::WA_DeleteOnClose);

        OptionsServerPage *pg = static_cast<OptionsServerPage*>(dlg->pageWidget(OptionsDialog::ServerPage));

        if (action == aEditServer)
            pg->setCurrentServer(server);
        else if (action == aAddServer)
            pg->addNewServer();

        dlg->show();
    }
    else if (action == aSelectOnly)
    {
        /* This assumes that unchecking all top-level items will uncheck everything under them */
        for (int i = 0, n = model()->rowCount(); i < n; ++i)
        {
            QModelIndex idx = model()->index(i, index.column(), QModelIndex());
            if (idx != index)
                model()->setData(idx, Qt::Unchecked, Qt::CheckStateRole);
        }

        model()->setData(index, Qt::Checked, Qt::CheckStateRole);
    }
    else if (action == aSelectElse)
    {
        for (int i = 0, n = model()->rowCount(); i < n; ++i)
        {
            QModelIndex idx = model()->index(i, index.column(), QModelIndex());
            if (idx != index)
                model()->setData(idx, Qt::Checked, Qt::CheckStateRole);
        }

        model()->setData(index, Qt::Unchecked, Qt::CheckStateRole);
    }
}

void DVRServersView::mouseDoubleClickEvent(QMouseEvent *event)
{
    QModelIndex index;
    if (event->button() == Qt::LeftButton && (index = indexAt(event->pos())).isValid() && (index.flags() & Qt::ItemIsUserCheckable))
    {
        Qt::CheckState state = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
        model()->setData(index, state, Qt::CheckStateRole);

        event->accept();
        return;
    }

    QAbstractItemView::mouseDoubleClickEvent(event);
}
