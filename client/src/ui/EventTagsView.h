#ifndef EVENTTAGSVIEW_H
#define EVENTTAGSVIEW_H

#include <QListView>

class EventTagsView : public QListView
{
    Q_OBJECT

public:
    explicit EventTagsView(QWidget *parent = 0);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    virtual void setModel(QAbstractItemModel *model);
    virtual void reset();

protected:
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

private:
    QSize cachedSizeHint;

    void calculateSizeHint();
};

#endif // EVENTTAGSVIEW_H
