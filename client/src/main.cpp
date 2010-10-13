#include "core/BluecherryApp.h"
#include "ui/MainWindow.h"
#include "ui/CrashReportDialog.h"
#include <QApplication>
#include <QSettings>
#include <QtPlugin>
#include <QMessageBox>
#include <QDateTime>

#ifdef QT_STATIC
Q_IMPORT_PLUGIN(qjpeg)
#endif

#ifdef USE_BREAKPAD
void initBreakpad();
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    {
        QStringList args = a.arguments();
        args.takeFirst();
        if (args.size() >= 1 && args[0] == QLatin1String("--crash"))
        {
            CrashReportDialog dlg((args.size() >= 2) ? args[1] : QString());
            dlg.exec();
            if (dlg.result() != QDialog::Accepted)
                return 0;
        }
    }

#ifdef USE_BREAKPAD
    initBreakpad();
#endif

    a.setOrganizationName(QLatin1String("bluecherry"));
    a.setOrganizationDomain(QLatin1String("bluecherrydvr.com"));
    a.setApplicationName(QLatin1String("bluecherry"));
    a.setApplicationVersion(QLatin1String("2.0.0"));

#ifdef Q_OS_WIN
    QSettings::setDefaultFormat(QSettings::IniFormat);
#endif

#ifdef BC_CUTOFF_DATE
    if (QDateTime::currentDateTime() >= QDateTime::fromTime_t(BC_CUTOFF_DATE))
    {
        QMessageBox::critical(0, a.tr("Bluecherry DVR - Beta Expired"), a.tr("This beta release of the Bluecherry DVR software "
                                                                             "has expired.\n\nPlease download the latest update from "
                                                                             "www.bluecherrydvr.com"));
        return 1;
    }
#endif

    bcApp = new BluecherryApp;

    MainWindow w;
    bcApp->mainWindow = &w;

    w.show();

#ifdef BC_CUTOFF_DATE
    if (QDateTime::currentDateTime().addDays(7) >= QDateTime::fromTime_t(BC_CUTOFF_DATE))
    {
        QMessageBox::critical(&w, a.tr("Bluecherry DVR - Beta Expiration"), a.tr("This beta release of the Bluecherry DVR software "
                                                                                 "will expire in %1 days.\nAfter this date, the software "
                                                                                 "will no longer run.\n\nPlease download the latest "
                                                                                 "update from www.bluecherrydvr.com")
                              .arg(QDateTime::currentDateTime().daysTo(QDateTime::fromTime_t(BC_CUTOFF_DATE))));
    }
#endif

    return a.exec();
}
