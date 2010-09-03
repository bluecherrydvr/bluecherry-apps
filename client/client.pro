QT += core gui network
CONFIG(static):QTPLUGIN += qjpeg

TARGET = bluecherry-client
TEMPLATE = app

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
INCLUDEPATH += src

win32-msvc2008|win32-msvc2010 {
    # Qt defaults to setting this, breakpad defaults to not. Qt doesn't use wchar_t in the
    # public API, so we can use a different setting. Otherwise, it would cause linker errors.
    QMAKE_CXXFLAGS -= -Zc:wchar_t-
    QMAKE_CXXFLAGS_RELEASE += -Zi
    QMAKE_LFLAGS_RELEASE += /OPT:REF,ICF
}

!CONFIG(no-breakpad) {
    DEFINES += USE_BREAKPAD
    INCLUDEPATH += "$$PWD/breakpad/src"
    SOURCES += src/utils/Breakpad.cpp

    unix:!macx:LIBS += "$$PWD/breakpad/src/client/linux/.libs/libbreakpad_client.a"

    macx {
        QMAKE_LFLAGS += -F$$PWD/breakpad-bin
        LIBS += -framework Breakpad
    }

    win32 {
        CONFIG(debug, debug|release):LIBS += -L"$$PWD/breakpad/src/client/windows/Debug/lib"
        CONFIG(release, debug|release):LIBS += -L"$$PWD/breakpad/src/client/windows/Release/lib"
        LIBS += common.lib crash_generation_client.lib exception_handler.lib
    }
}

SOURCES += src/main.cpp \
    src/ui/MainWindow.cpp \
    src/ui/CameraAreaWidget.cpp \
    src/ui/CameraAreaControls.cpp \
    src/ui/OptionsDialog.cpp \
    src/core/DVRServer.cpp \
    src/core/BluecherryApp.cpp \
    src/ui/DVRServersModel.cpp \
    src/ui/DVRServersView.cpp \
    src/ui/OptionsServerPage.cpp \
    src/ui/NumericOffsetWidget.cpp \
    src/core/DVRCamera.cpp \
    src/ui/LiveFeedWidget.cpp \
    src/core/MJpegStream.cpp \
    src/ui/RecentEventsView.cpp \
    src/ui/EventsModel.cpp \
    src/ui/EventsWindow.cpp \
    src/ui/EventResultsView.cpp \
    src/ui/EventTimelineWidget.cpp \
    src/utils/ThreadTask.cpp \
    src/utils/ThreadTaskCourier.cpp \
    src/utils/ImageDecodeTask.cpp \
    src/ui/EventViewWindow.cpp \
    src/ui/EventTagsView.cpp \
    src/ui/EventTagsModel.cpp \
    src/ui/EventTagsDelegate.cpp \
    src/ui/EventCommentsWidget.cpp \
    src/ui/ExpandingTextEdit.cpp \
    src/ui/RecentEventDelegate.cpp \
    src/ui/CrashReportDialog.cpp \
    src/ui/EventSourcesModel.cpp \
    src/core/EventData.cpp

HEADERS  += src/ui/MainWindow.h \
    src/ui/CameraAreaWidget.h \
    src/ui/CameraAreaControls.h \
    src/ui/OptionsDialog.h \
    src/core/DVRServer.h \
    src/core/BluecherryApp.h \
    src/ui/DVRServersModel.h \
    src/ui/DVRServersView.h \
    src/ui/OptionsServerPage.h \
    src/ui/NumericOffsetWidget.h \
    src/core/DVRCamera.h \
    src/ui/LiveFeedWidget.h \
    src/core/MJpegStream.h \
    src/ui/RecentEventsView.h \
    src/ui/EventsModel.h \
    src/ui/EventsWindow.h \
    src/ui/EventResultsView.h \
    src/ui/EventTimelineWidget.h \
    src/utils/ThreadTask.h \
    src/utils/ThreadTaskCourier.h \
    src/utils/ImageDecodeTask.h \
    src/ui/EventViewWindow.h \
    src/ui/EventTagsView.h \
    src/ui/EventTagsModel.h \
    src/ui/EventTagsDelegate.h \
    src/ui/EventCommentsWidget.h \
    src/ui/ExpandingTextEdit.h \
    src/ui/RecentEventDelegate.h \
    src/ui/CrashReportDialog.h \
    src/ui/EventSourcesModel.h \
    src/core/EventData.h
