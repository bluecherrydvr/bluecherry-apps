#ifndef VIDEOHTTPBUFFER_H
#define VIDEOHTTPBUFFER_H

#include <QObject>
#include <QUrl>
#include <QTemporaryFile>

class QNetworkReply;

struct _GstAppSrc;
typedef struct _GstAppSrc GstAppSrc;

class VideoHttpBuffer : public QObject
{
    Q_OBJECT

public:
    explicit VideoHttpBuffer(GstAppSrc *element, QObject *parent = 0);
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
    qint64 m_fileSize;
    GstAppSrc *m_element;
    QFile m_readFile;

    static void needDataWrap(GstAppSrc *, unsigned, void*);
    static int seekDataWrap(GstAppSrc *, quint64, void*);

    void needData(unsigned size);
    bool seekData(qint64 offset);
};

#endif // VIDEOHTTPBUFFER_H
