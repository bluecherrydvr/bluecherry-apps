#ifndef VIDEOHTTPBUFFER_H
#define VIDEOHTTPBUFFER_H

#include <QObject>
#include <QUrl>
#include <QTemporaryFile>

class QNetworkReply;

class VideoHttpBuffer : public QObject
{
    Q_OBJECT

public:
    explicit VideoHttpBuffer(QObject *parent = 0);

    QString bufferFileName() const { return m_bufferFile.fileName(); }
    qint64 fileSize() const { return m_fileSize; }
    qint64 bufferedSize() const { return m_bufferFile.pos(); }

public slots:
    bool start(const QUrl &url);

signals:
    void readyRead();
    void endOfFile();
    void streamError(const QString &message);

private slots:
    void networkRead();
    void networkFinished();
    void networkMetaData();

private:
    QTemporaryFile m_bufferFile;
    QNetworkReply *m_networkReply;
    qint64 m_fileSize;
};

#endif // VIDEOHTTPBUFFER_H
