#ifndef EVENTVIDEODOWNLOAD_H
#define EVENTVIDEODOWNLOAD_H

#include <QObject>
#include <QPointer>
#include <QFutureWatcher>
#include "video/VideoHttpBuffer.h"

class QProgressDialog;

class EventVideoDownload : public QObject
{
    Q_OBJECT

public:
    explicit EventVideoDownload(QObject *parent = 0);
    ~EventVideoDownload();

    void setVideoBuffer(VideoHttpBuffer *buffer);
    void setFilePath(const QString &path);

    void start(QWidget *parentWindow = 0);

private slots:
    void startCopy();
    void copyFinished();
    void updateBufferProgress();

private:
    QProgressDialog *m_dialog;
    QFutureWatcher<bool> *m_futureWatch;
    QPointer<VideoHttpBuffer> m_videoBuffer;
    QString m_tempFilePath;
    QString m_finalPath;
};

#endif // EVENTVIDEODOWNLOAD_H
