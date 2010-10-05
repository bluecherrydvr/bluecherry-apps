#include "SavedLayoutsModel.h"
#include <QSettings>
#include <QStringList>

SavedLayoutsModel::SavedLayoutsModel(QObject *parent)
    : QAbstractListModel(parent)
{
    populate();
}

void SavedLayoutsModel::populate()
{
    items.clear();

    QSettings settings;
    settings.beginGroup(QLatin1String("cameraLayouts"));
    QStringList keys = settings.childKeys();

    if (keys.isEmpty())
    {
        /* There always must be at least one layout; create the default */
        settings.setValue(tr("Default"), QByteArray());
        keys = settings.childKeys();
    }

    foreach (QString key, keys)
    {
        SavedLayoutData d;
        d.name = key;
        d.data = settings.value(key).toByteArray();
        items.append(d);
    }

    settings.endGroup();

    SavedLayoutData d;
    d.name = tr("New layout...");
    items.append(d);

    reset();
}

int SavedLayoutsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return items.size();
}

QModelIndex SavedLayoutsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || row < 0 || column != 0 || row >= items.size())
        return QModelIndex();

    return createIndex(row, column, row);
}

QVariant SavedLayoutsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const SavedLayoutData &d = items[index.row()];

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return d.name;
    case LayoutDataRole:
        return d.data;
    default:
        return QVariant();
    }
}

bool SavedLayoutsModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (row < 0 || row > items.size() || parent.isValid() || !count)
        return false;

    beginInsertRows(QModelIndex(), row, row+count-1);
    SavedLayoutData d;
    for (int i = row; i < (row+count); ++i)
        items.insert(i, d);
    endInsertRows();

    return true;
}

bool SavedLayoutsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() < 0 || index.row() >= items.size())
        return false;

    SavedLayoutData &d = items[index.row()];
    QSettings settings;
    settings.beginGroup(QLatin1String("cameraLayouts"));

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (value.toString().isEmpty())
            return false;

        if (!d.name.isEmpty())
            settings.remove(d.name);

        d.name = value.toString();
        settings.setValue(d.name, d.data);
        break;

    case LayoutDataRole:
        d.data = value.toByteArray();
        if (!d.name.isEmpty())
            settings.setValue(d.name, d.data);
        break;

    default:
        return false;
    }

    return true;
}
