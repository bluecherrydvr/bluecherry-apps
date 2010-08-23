#ifndef EVENTTAGSMODEL_H
#define EVENTTAGSMODEL_H

#include <QAbstractListModel>

class EventTagsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EventTagsModel(QObject *parent = 0);

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    void removeTag(const QModelIndex &index);

private:
    QList<QString> tags;
};

#endif // EVENTTAGSMODEL_H
