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

    m_scaleResults.resize(m_scaleSizes.size());
    for (int i = 0; i < m_scaleSizes.size(); ++i)
    {
        m_scaleResults[i] = m_result.scaled(m_scaleSizes[i], Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
}
