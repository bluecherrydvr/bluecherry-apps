#include "ImageDecodeTask.h"
#include <QImageReader>
#include <QBuffer>
#include <QDebug>

ImageDecodeTask::ImageDecodeTask(QObject *caller, const char *callback)
    : ThreadTask(caller, callback)
{
}

void ImageDecodeTask::runTask()
{
    if (isCancelled() || m_data.isNull())
        return;

    QBuffer buffer(&m_data);
    QImageReader reader(&buffer, "jpeg");
    reader.setAutoDetectImageFormat(false);

    if (!reader.read(&m_result))
    {
        qDebug() << "Image decoding error:" << reader.errorString();
        return;
    }

    m_data.clear();
}
