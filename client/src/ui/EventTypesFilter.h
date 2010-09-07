#ifndef EVENTTYPESFILTER_H
#define EVENTTYPESFILTER_H

#include <QTreeWidget>
#include <QBitArray>

class EventType;

class EventTypesFilter : public QTreeWidget
{
    Q_OBJECT

public:
    explicit EventTypesFilter(QWidget *parent = 0);

    QBitArray checkedTypes() const;

signals:
    void checkedTypesChanged(const QBitArray &checkedTypes);

private slots:
    void checkStateChanged(QTreeWidgetItem *item);

private:
    QTreeWidgetItem *addItem(const QString &name, EventType min, EventType max);
};

#endif // EVENTTYPESFILTER_H
