#ifndef CRASHREPORTDIALOG_H
#define CRASHREPORTDIALOG_H

#include <QDialog>
#include <QFile>

class QNetworkReply;
class QLineEdit;
class QCheckBox;
class QProgressBar;
class QStackedLayout;

class CrashReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CrashReportDialog(const QString &dumpFile, QWidget *parent = 0);
    virtual ~CrashReportDialog();

private slots:
    void uploadAndRestart();
    void uploadAndExit();

    void setUploadProgress(qint64 done);
    void uploadFinished();

protected:
    virtual void showEvent(QShowEvent *event);

private:
    QFile m_dumpFile;
    QNetworkReply *m_uploadReply;

    QLineEdit *m_emailInput;
    QCheckBox *m_allowReport;
    QProgressBar *m_uploadProgress;
    QStackedLayout *m_stackLayout;
    int m_finalResult;

    void sendReport();
    void deleteDump();
};

#endif // CRASHREPORTDIALOG_H
