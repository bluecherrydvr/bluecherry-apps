#include "DVRServersView.h"
#include "DVRServersModel.h"
#include "OptionsDialog.h"
#include "OptionsServerPage.h"
#include <QHeaderView>
#include <QContextMenuEvent>
#include <QMenu>

DVRServersView::DVRServersView(QWidget *parent)
    : QTreeView(parent)
{
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

    QAction *aEditServer = 0;

    QModelIndex index = indexAt(event->pos());
    if (index.isValid())
    {
        menu.addAction(tr("Connect"))->setEnabled(false);
        aEditServer = menu.addAction(tr("Edit server"));
        menu.addSeparator();
    }

    QAction *aAddServer = menu.addAction("Add new server...");
    QAction *aOptions = menu.addAction("Options");

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
            pg->setCurrentServer(static_cast<DVRServersModel*>(model())->serverForRow(index));
        else if (action == aAddServer)
            pg->addNewServer();

        dlg->show();
    }
}
