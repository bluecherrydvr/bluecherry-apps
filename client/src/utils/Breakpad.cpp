#include <QString>
#include <QDesktopServices>
#include <QFileInfo>

#ifdef Q_OS_WIN
/* Platform-specific code for Windows */
#include "client/windows/handler/exception_handler.h"
#include <windows.h>
using namespace google_breakpad;

static const wchar_t *executablePath = 0;

bool breakpadDumpCallback(const wchar_t *dump_path, const wchar_t *minidump_id, void *, EXCEPTION_POINTERS *,
                          MDRawAssertionInfo *, bool succeeded)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    wchar_t cmdline[MAX_PATH];
    wcscpy_s(cmdline, executablePath);
    wcscat_s(cmdline, L" --crash \"");
    if (succeeded)
    {
        wcscat_s(cmdline, dump_path);
        wcscat_s(cmdline, L"/");
        wcscat_s(cmdline, minidump_id);
        wcscat_s(cmdline, L".dmp\"");
    }

    if (!CreateProcessW(executablePath, cmdline, 0, 0, FALSE, 0, 0, 0, &si, &pi))
        return false;

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

void initBreakpad()
{
    /* Store the path of the executable in case we crash; it might not be safe to do this later. */
    wchar_t buf[MAX_PATH];
    DWORD sz = GetModuleFileNameW(0, buf, MAX_PATH);
    executablePath = new wchar_t[sz+1];
    wmemcpy_s((wchar_t*)executablePath, sz+1, buf, sz+1);

    QString tmp = QDesktopServices::storageLocation(QDesktopServices::TempLocation);

    new ExceptionHandler(std::wstring(reinterpret_cast<const wchar_t*>(tmp.utf16()), tmp.size()), 0,
                         &breakpadDumpCallback, 0, ExceptionHandler::HANDLER_ALL);
}

#elif defined(Q_OS_MAC)
/* Platform-specific code for Mac OS X */
#include "client/mac/handler/exception_handler.h"
using namespace google_breakpad;

#error Not implemented

#else
/* Platform-specific code for Linux */
#include "client/linux/handler/exception_handler.h"
#include <limits.h>
using namespace google_breakpad;

static QByteArray executablePath;

bool breakpadDumpCallback(const char *dump_path, const char *minidump_id, void *, bool succeeded)
{
    char param[] = "--crash";

    char dumpFile[PATH_MAX];
    if (succeeded)
    {
        strcpy(dumpFile, dump_path);
        strcat(dumpFile, "/");
        strcat(dumpFile, minidump_id);
        strcat(dumpFile, ".dmp");
    }

    char * const args[] = { (char*)executablePath.constData(), param, succeeded ? dumpFile : 0, 0 };

    execv(executablePath.constData(), args);
    return true;
}

void initBreakpad()
{
    QFileInfo fi(QLatin1String("/proc/self/exe"));
    executablePath = fi.symLinkTarget().toLocal8Bit();

    QString tmp = QDesktopServices::storageLocation(QDesktopServices::TempLocation);

    new ExceptionHandler(tmp.toStdString(), 0, &breakpadDumpCallback, 0, true);
}

#endif
