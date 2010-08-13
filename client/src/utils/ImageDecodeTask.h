#ifndef IMAGEDECODETASK_H
#define IMAGEDECODETASK_H

#include "ThreadTask.h"
#include <QImage>

class ImageDecodeTask : public ThreadTask
{
public:
    ImageDecodeTask(QObject *caller, const char *callback);

    void setData(const QByteArray &data) { m_data = data; }

    QImage result() const { return m_result; }

protected:
    virtual void runTask();

private:
    QByteArray m_data;
    QImage m_result;
};

#endif // IMAGEDECODETASK_H
