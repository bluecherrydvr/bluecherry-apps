#ifndef EVENTVIDEODOWNLOAD_H
#define EVENTVIDEODOWNLOAD_H

#include <QObject>
#include <QPointer>
#include <QFutureWatcher>

class QProgressDialog;
class VideoHttpBuffer;

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
    VideoHttpBuffer *m_videoBuffer;
    QPointer<QObject> m_videoBufferParent;
    QString m_tempFilePath;
    QString m_finalPath;
};

#endif // EVENTVIDEODOWNLOAD_H
