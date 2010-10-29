#ifndef DVRSERVERSVIEW_H
#define DVRSERVERSVIEW_H

#include <QTreeView>

class DVRServer;

class DVRServersView : public QTreeView
{
    Q_OBJECT

public:
    explicit DVRServersView(QWidget *parent = 0);

    DVRServer *currentServer() const;

    virtual void setModel(QAbstractItemModel *model);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void rowsAboutToBeInserted(const QModelIndex &parent, int start, int end);
};

#endif // DVRSERVERSVIEW_H
