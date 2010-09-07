#include "EventTypesFilter.h"
#include "core/EventData.h"
#include <QHeaderView>

EventTypesFilter::EventTypesFilter(QWidget *parent)
    : QTreeWidget(parent)
{
    header()->hide();

    addItem(tr("Camera"), EventType::MinCamera, EventType::MaxCamera);
    addItem(tr("System"), EventType::MinSystem, EventType::MaxSystem);

    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)), SLOT(checkStateChanged(QTreeWidgetItem*)));
}

QTreeWidgetItem *EventTypesFilter::addItem(const QString &name, EventType min, EventType max)
{
    QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList() << name, 0);
    parent->setFlags(parent->flags() | Qt::ItemIsUserCheckable);
    parent->setData(0, Qt::CheckStateRole, Qt::Checked);

    for (int i = min; i <= (int)max; ++i)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent,
            QStringList() << EventType((EventType::Type)i).uiString(), 1);
        item->setData(0, Qt::UserRole, i);
        item->setData(0, Qt::CheckStateRole, Qt::Checked);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    }

    addTopLevelItem(parent);
    parent->setExpanded(true);
    return parent;
}

void EventTypesFilter::checkStateChanged(QTreeWidgetItem *item)
{
    static bool ignore = false;
    if (ignore)
        return;

    Qt::CheckState state = (Qt::CheckState)item->data(0, Qt::CheckStateRole).toInt();
    QVariant typev = item->data(0, Qt::UserRole);
    if (typev.isNull())
    {
        /* Top-level */
        ignore = true;
        for (int i = 0; i < item->childCount(); ++i)
        {
            QTreeWidgetItem *child = item->child(i);
            child->setData(0, Qt::CheckStateRole, state);
        }
        ignore = false;
    }
    else
    {
        /* Update the state of the top level item */
        QTreeWidgetItem *p = item->parent();
        Q_ASSERT(p);

        Qt::CheckState parentState = Qt::PartiallyChecked;
        for (int i = 0; i < p->childCount(); ++i)
        {
            QTreeWidgetItem *child = p->child(i);
            Qt::CheckState itemState = (Qt::CheckState)child->data(0, Qt::CheckStateRole).toInt();

            if ((itemState == Qt::Checked && parentState == Qt::Unchecked) ||
                (itemState == Qt::Unchecked) && parentState == Qt::Checked)
            {
                parentState = Qt::PartiallyChecked;
                break;
            }

            if (parentState == Qt::PartiallyChecked)
                parentState = itemState;
        }

        ignore = true;
        p->setData(0, Qt::CheckStateRole, parentState);
        ignore = false;
    }

    if (receivers(SIGNAL(checkedTypesChanged(QBitArray))))
        emit checkedTypesChanged(checkedTypes());
}

QBitArray EventTypesFilter::checkedTypes() const
{
    QBitArray re(EventType::Max+1);
    for (int pi = 0; pi < topLevelItemCount(); ++pi)
    {
        QTreeWidgetItem *p = topLevelItem(pi);
        for (int i = 0; i < p->childCount(); ++i)
        {
            QTreeWidgetItem *item = p->child(i);
            if (item->data(0, Qt::CheckStateRole).toInt() == Qt::Checked)
                re[item->data(0, Qt::UserRole).toInt()] = true;
        }
    }

    return re;
}
