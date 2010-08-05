#ifndef CAMERASOURCESMODEL_H
#define CAMERASOURCESMODEL_H

#include "DVRServersModel.h"
#include <QSet>
#include <QBitArray>

class DVRCamera;

class CameraSourcesModel : public DVRServersModel
{
    Q_OBJECT

public:
    explicit CameraSourcesModel(QObject *parent = 0);

    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);

private:
    QSet<DVRCamera*> checkedCameras;
};

#endif // CAMERASOURCESMODEL_H
