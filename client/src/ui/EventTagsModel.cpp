#include "EventTagsModel.h"

EventTagsModel::EventTagsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    /* Placeholder for tags that will be parsed out of the event data */
    tags << QLatin1String("fake tag") << QLatin1String("more tags") << QLatin1String("third tag");
}

int EventTagsModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return tags.size();
    else
        return 0;
}

QVariant EventTagsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid());

    switch (role)
    {
    case Qt::DisplayRole:
        return tags[index.row()];
    }

    return QVariant();
}

void EventTagsModel::removeTag(const QModelIndex &index)
{
    /* TODO: Actually remove the tag from wherever we got it */
    if (!index.isValid() || index.row() < 0 || index.row() >= tags.size())
        return;

    beginRemoveRows(QModelIndex(), index.row(), index.row());
    tags.removeAt(index.row());
    endRemoveRows();
}
