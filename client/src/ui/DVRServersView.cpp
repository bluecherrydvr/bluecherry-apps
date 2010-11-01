#include "DVRServersView.h"
#include "DVRServersModel.h"
#include "OptionsDialog.h"
#include "OptionsServerPage.h"
#include "ServerConfigWindow.h"
#include "core/DVRServer.h"
#include "LiveFeedWidget.h"
#include "core/BluecherryApp.h"
#include "MainWindow.h"
#include "CameraAreaWidget.h"
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
    setAnimated(true);

    DVRServersModel *model = new DVRServersModel(this);
    model->setOfflineDisabled(true);
    setModel(model);

    /* We only show the server name in this list; hide other columns */
    for (int i = 1, n = model->columnCount(); i < n; ++i)
        header()->setSectionHidden(i, true);
}

void DVRServersView::setModel(QAbstractItemModel *m)
{
    if (model())
        model()->disconnect(this);

    QTreeView::setModel(m);

    connect(m, SIGNAL(rowsAboutToBeInserted(QModelIndex,int,int)), SLOT(rowsAboutToBeInserted(QModelIndex,int,int)));
}

DVRServer *DVRServersView::currentServer() const
{
    QModelIndex c = currentIndex();
    DVRServer *re = 0;

    while (c.isValid())
    {
        re = c.data(DVRServersModel::ServerPtrRole).value<DVRServer*>();
        if (re)
            break;
        c = c.parent();
    }

    return re;
}

void DVRServersView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);

    QAction *aConnect = 0, *aRefreshDevices = 0, *aEditServer = 0, *aServerConfig = 0, *aOptions = 0, *aAddServer = 0;
    QAction *aSelectOnly = 0, *aSelectElse = 0;
    QAction *aAddFeed = 0, *aOpenWin = 0, *aOpenFull = 0, *aCamRename = 0;

    QModelIndex index = indexAt(event->pos());
    DVRServer *server = index.data(DVRServersModel::ServerPtrRole).value<DVRServer*>();
    DVRCamera camera = index.data(DVRServersModel::DVRCameraRole).value<DVRCamera>();
    if (index.isValid())
    {
        if (index.flags() & Qt::ItemIsUserCheckable)
        {
            aSelectOnly = menu.addAction(tr("Select only this"));
            aSelectElse = menu.addAction(tr("Select everything else"));
            if (!(index.flags() & Qt::ItemIsEnabled))
            {
                aSelectOnly->setEnabled(false);
                aSelectElse->setEnabled(false);
            }
            menu.addSeparator();
        }

        if (server)
        {
            aConnect = server->api->isOnline() ? menu.addAction(tr("Disconnect")) : menu.addAction(tr("Connect"));
            menu.addSeparator();
            aRefreshDevices = menu.addAction(tr("Refresh devices"));
            menu.addSeparator();
            aServerConfig = menu.addAction(tr("Configure server"));
            aEditServer = menu.addAction(tr("Edit server"));
        }
        else if (camera)
        {
            aAddFeed = menu.addAction(tr("Add to view"));
            menu.addSeparator();
            aOpenWin = menu.addAction(tr("Open in window"));
            aOpenFull = menu.addAction(tr("Open as fullscreen"));
            menu.addSeparator();
            aCamRename = menu.addAction(tr("Rename device"));
            aCamRename->setEnabled(false);
        }
    }
    else
    {
        aAddServer = menu.addAction(tr("Add new server..."));
        aOptions = menu.addAction(tr("Client options"));
    }

    QAction *action = menu.exec(event->globalPos());
    if (!action)
        return;

    if (action == aConnect)
    {
        if (!server->api->isOnline())
            server->login();
        else
            server->api->logout();
    }
    else if (action == aRefreshDevices)
    {
        server->updateCameras();
    }
    else if (action == aEditServer || action == aAddServer || action == aOptions)
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
    else if (action == aServerConfig)
    {
        ServerConfigWindow::instance()->setServer(server);
        ServerConfigWindow::instance()->show();
        ServerConfigWindow::instance()->raise();
    }
    else if (action == aAddFeed)
    {
        bcApp->mainWindow->cameraArea()->addCamera(camera);
    }
    else if (action == aOpenWin || action == aOpenFull)
    {
        LiveFeedWidget *w = new LiveFeedWidget;
        w->setWindow();
        w->setAttribute(Qt::WA_DeleteOnClose);
        w->setCamera(camera);
        if (action == aOpenFull)
            w->showFullScreen();
        else
            w->show();
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
    if (event->button() == Qt::LeftButton && (index = indexAt(event->pos())).isValid())
    {
        DVRServer *server = index.data(DVRServersModel::ServerPtrRole).value<DVRServer*>();
        DVRCamera camera = index.data(DVRServersModel::DVRCameraRole).value<DVRCamera>();
        if (index.flags() & Qt::ItemIsUserCheckable)
        {
            Qt::CheckState state = (index.data(Qt::CheckStateRole).toInt() == Qt::Checked) ? Qt::Unchecked : Qt::Checked;
            model()->setData(index, state, Qt::CheckStateRole);

            event->accept();
        }
        else if (server && !(index.flags() & Qt::ItemIsEnabled))
        {
            OptionsDialog *dlg = new OptionsDialog(this);
            dlg->showPage(OptionsDialog::ServerPage);
            dlg->setAttribute(Qt::WA_DeleteOnClose);

            OptionsServerPage *pg = static_cast<OptionsServerPage*>(dlg->pageWidget(OptionsDialog::ServerPage));
            pg->setCurrentServer(server);
            dlg->show();
        }
        else if (server)
        {
            ServerConfigWindow::instance()->setServer(server);
            ServerConfigWindow::instance()->show();
            ServerConfigWindow::instance()->raise();
        }
        else if (camera)
        {
            bcApp->mainWindow->cameraArea()->addCamera(camera);
        }

        event->accept();
        return;
    }

    QAbstractItemView::mouseDoubleClickEvent(event);
}

void DVRServersView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    QModelIndex current = currentIndex();
    if (current.isValid() && (current == topLeft || topLeft < current) &&
        (current == bottomRight || current < bottomRight) && !(current.flags() & Qt::ItemIsEnabled))
    {
        setCurrentIndex(QModelIndex());
    }

    QTreeView::dataChanged(topLeft, bottomRight);
}

void DVRServersView::rowsAboutToBeInserted(const QModelIndex &parent, int start, int end)
{
    if (!model()->hasChildren(parent))
        setExpanded(parent, true);
}
