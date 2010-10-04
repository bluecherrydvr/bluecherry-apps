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

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
};

#endif // DVRSERVERSVIEW_H
