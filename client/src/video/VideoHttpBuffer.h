#ifndef VIDEOHTTPBUFFER_H
#define VIDEOHTTPBUFFER_H

#include <QObject>
#include <QUrl>
#include <QTemporaryFile>
#include <QMutex>
#include <QWaitCondition>

class QNetworkReply;

typedef struct _GstAppSrc GstAppSrc;
typedef struct _GstElement GstElement;

class VideoHttpBuffer : public QObject
{
    Q_OBJECT

public:
    explicit VideoHttpBuffer(GstAppSrc *element, GstElement *pipeline, QObject *parent = 0);
    ~VideoHttpBuffer();

    QString bufferFileName() const { return m_bufferFile.fileName(); }
    qint64 fileSize() const { return m_fileSize; }
    qint64 bufferedSize() const { return m_bufferFile.pos(); }

public slots:
    bool start(const QUrl &url);

signals:
    void streamError(const QString &message);

private slots:
    void networkRead();
    void networkFinished();
    void networkMetaData();

private:
    QTemporaryFile m_bufferFile;
    QNetworkReply *m_networkReply;
    qint64 m_fileSize, m_readPos, m_writePos;
    GstAppSrc *m_element;
    GstElement *m_pipeline;
    QMutex m_lock;
    QWaitCondition m_bufferWait;
    bool m_streamInit, m_bufferBlocked;

    /* Rate estimation; circular buffer holding amounts for the last 64 buffer requests */
    static const int rateCount = 64;
    Q_PACKED struct
    {
        quint64 time;
        unsigned size;
    } rateData[rateCount];
    int ratePos, rateMax;

    void addRateData(quint64 time, unsigned size);
    void getRateEstimation(quint64 *duration, unsigned *size);

    static void needDataWrap(GstAppSrc *, unsigned, void*);
    static int seekDataWrap(GstAppSrc *, quint64, void*);

    void needData(unsigned size);
    bool seekData(qint64 offset);
};

#endif // VIDEOHTTPBUFFER_H
