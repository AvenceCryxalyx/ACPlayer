#include "ACPlayer.h"
#include <QtWidgets/QApplication>
#include <QtCore/qglobal.h>
/** some include file */
#ifdef Q_OS_WIN
#include <QSettings>
//#include <windows.h>
//#include <timeapi.h>  
//            // link winmm.lib
//static void requestHighResTimer()
//{
//    timeBeginPeriod(1);           // pair with timeEndPeriod(1) at exit
//    using Fn = BOOL(WINAPI*)(HANDLE, int, PVOID, ULONG);
//    if (auto f = reinterpret_cast<Fn>(GetProcAddress(
//        GetModuleHandleW(L"kernel32"), "SetProcessInformation"))) {
//        struct { ULONG Version, ControlMask, StateMask; } s{ 1, 0x4, 0 }; // IGNORE_TIMER_RESOLUTION
//        f(GetCurrentProcess(), 4 /*ProcessPowerThrottling*/, &s, sizeof(s));
//    }
//}
#endif

void checkWinRegistry()
{
#ifdef Q_OS_WIN
    QString val;
    QString exePath = qApp->applicationFilePath();
    exePath.replace("/", "\\");

    QSettings regType("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\.abc",
        QSettings::NativeFormat);
    QSettings regIcon("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\.abc\\DefaultIcon",
        QSettings::NativeFormat);
    QSettings regShell("HKEY_CURRENT_USER\\SOFTWARE\\Classes\\.abc\\Shell\\Open\\Command",
        QSettings::NativeFormat);

    if ("" != regType.value(".").toString()) {
        regType.setValue(".", "");
    }

    val = exePath + ",0";
    if (val != regIcon.value(".").toString()) {
        regIcon.setValue(".", val);
    }

    val = exePath + " \"%1\"";
    if (val != regShell.value(".").toString()) {
        regShell.setValue(".", val);
    }
#endif
}

void registerFileAssociation(const QString& extension, const QString& progId, const QString& description) {
#ifdef Q_OS_WIN
    // 1. Path to your application
    QString appPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());

    // 2. Set up QSettings for Registry (NativeFormat)
    // We use HKEY_CLASSES_ROOT, which requires administrative rights
    QSettings root(QStringLiteral("HKEY_CLASSES_ROOT"), QSettings::NativeFormat);

    // 3. Register the file extension with the ProgID
    root.setValue(extension + "/.", progId);

    // 4. Register the ProgID description
    root.setValue(progId + "/.", description);

    // 5. Set the application icon (optional)
    root.setValue(progId + "/DefaultIcon/.", appPath + ",0");

    // 6. Register the command to open the file
    // The "%1" is crucial: it passes the file path as an argument to your app
    root.setValue(progId + "/shell/open/command/.", "\"" + appPath + "\" \"%1\"");
#endif
}

int main(int argc, char *argv[])
{
    //requestHighResTimer();
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    qputenv("QT_MEDIA_BACKEND", "ffmpeg");
#endif

    QApplication app(argc, argv);
    
    checkWinRegistry();
    registerFileAssociation(".mkv", "ac_player", "mkv registration for my app");
    registerFileAssociation(".mp4", "ac_player", "mp4 registration for my app");
    registerFileAssociation(".avi", "ac_player", "avi registration for my app");
    registerFileAssociation(".mov", "ac_player", "mov registration for my app");
    ACPlayer window;
    window.show();

    QTimer::singleShot(200, &window, SLOT(initialize()));

    return app.exec();
}
