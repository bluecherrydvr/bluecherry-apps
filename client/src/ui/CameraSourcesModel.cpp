#include "CameraSourcesModel.h"
#include "core/DVRServer.h"

CameraSourcesModel::CameraSourcesModel(QObject *parent)
    : DVRServersModel(parent)
{
}

Qt::ItemFlags CameraSourcesModel::flags(const QModelIndex &index) const
{
    return DVRServersModel::flags(index) | Qt::ItemIsUserCheckable;
}

QVariant CameraSourcesModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::CheckStateRole)
    {
        DVRCamera *camera = cameraForRow(index);
        DVRServer *server;
        if (camera)
        {
            if (checkedCameras.contains(camera))
                return Qt::Checked;
        }
        else if ((server = serverForRow(index)))
        {
            QList<DVRCamera*> cameras = server->cameras();
            int count = 0;
            for (QList<DVRCamera*>::Iterator it = cameras.begin(); it != cameras.end(); ++it)
            {
                if (checkedCameras.contains(*it))
                    ++count;
            }

            if (count == cameras.size())
                return Qt::Checked;
            else if (count)
                return Qt::PartiallyChecked;
        }

        return Qt::Unchecked;
    }

    return DVRServersModel::data(index, role);
}

bool CameraSourcesModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::CheckStateRole)
    {
        DVRCamera *camera = cameraForRow(index);
        DVRServer *server;
        if (camera)
        {
            if (value.toBool())
                checkedCameras.insert(camera);
            else
                checkedCameras.remove(camera);

            emit dataChanged(index, index);
            QModelIndex parent = index.parent();
            emit dataChanged(parent, parent);
            return true;
        }
        else if ((server = serverForRow(index)))
        {
            QList<DVRCamera*> cameras = server->cameras();
            bool add = value.toBool();
            for (QList<DVRCamera*>::Iterator it = cameras.begin(); it != cameras.end(); ++it)
            {
                if (add)
                    checkedCameras.insert(*it);
                else
                    checkedCameras.remove(*it);
            }

            emit dataChanged(index, index);
            emit dataChanged(this->index(0, 0, index), this->index(this->rowCount(index)-1, this->columnCount(index)-1, index));
            return true;
        }
        else
            return false;
    }
    else
        return DVRServersModel::setData(index, value, role);
}
