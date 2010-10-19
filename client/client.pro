QT += core gui network webkit
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

unix:!macx {
    # GStreamer
    CONFIG += link_pkgconfig
    PKGCONFIG += gstreamer-0.10 gstreamer-interfaces-0.10 gstreamer-app-0.10 gstreamer-video-0.10
    DEFINES += USE_GSTREAMER
}

macx {
    # Bundled GStreamer
    CONFIG(x86_64):error(GStreamer cannot currently be built for x86_64 on OS X)

    DEFINES += USE_GSTREAMER
    GSTREAMER_PATH = "$$PWD/gstreamer-bin/mac"
    LIBS += -L"$$GSTREAMER_PATH/lib" -lgstreamer-0.10.0 -lgstapp-0.10.0 -lgstvideo-0.10.0 -lgstinterfaces-0.10.0 -lglib-2.0.0 -lgobject-2.0.0
    INCLUDEPATH += "$$GSTREAMER_PATH/include"

    QMAKE_POST_LINK += cd $$PWD; mac/deploy.sh $${OUT_PWD}/$${TARGET}.app
    CONFIG(release, debug|release):QMAKE_POST_LINK += " $$[QT_INSTALL_BINS]/macdeployqt"
    QMAKE_POST_LINK += "; cd - >/dev/null;"
}

win32 {
    isEmpty(GSTREAMER_PATH) {
        message(Using bundled GStreamer)
        GSTREAMER_PATH = "$$PWD/gstreamer-bin/win"
    }
    INCLUDEPATH += "$${GSTREAMER_PATH}/include" "$${GSTREAMER_PATH}/include/gstreamer-0.10" "$${GSTREAMER_PATH}/include/glib-2.0" "$${GSTREAMER_PATH}/include/libxml2"
    LIBS += -L"$${GSTREAMER_PATH}/lib" gstreamer-0.10.lib gstinterfaces-0.10.lib gstapp-0.10.lib gstvideo-0.10.lib glib-2.0.lib gobject-2.0.lib
    DEFINES += USE_GSTREAMER
    CONFIG(debug, debug|release):DEFINES += GSTREAMER_PLUGINS=\\\"$$PWD/gstreamer-bin/win/plugins\\\"
    CONFIG(release, debug|release):DEFINES += GSTREAMER_PLUGINS=\\\"plugins\\\"
}

!CONFIG(no-breakpad) {
    DEFINES += USE_BREAKPAD
    INCLUDEPATH += "$$PWD/breakpad/src"
    SOURCES += src/utils/Breakpad.cpp

    unix:QMAKE_CXXFLAGS_RELEASE += -gstabs
    
    unix:!macx {
        LIBS += "$$PWD/breakpad/src/client/linux/.libs/libbreakpad_client.a"

        QMAKE_POST_LINK = python "$$PWD/breakpad-bin/symbolstore.py" "$$PWD/breakpad/src/tools/linux/dump_syms/dump_syms" $${TARGET}.symbols $(TARGET); $$QMAKE_POST_LINK
    }

    macx {
        QMAKE_LFLAGS += -F$$PWD/breakpad-bin/mac
        LIBS += -framework Breakpad

        QMAKE_POST_LINK += cd "$$PWD"; breakpad-bin/mac/gather_symbols.sh $${OUT_PWD}/$${TARGET}; cd - >/dev/null;
    }

    win32 {
        CONFIG(debug, debug|release):LIBS += -L"$$PWD/breakpad-bin/win/lib-debug"
        CONFIG(release, debug|release):LIBS += -L"$$PWD/breakpad-bin/win/lib-release"
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
    src/core/EventData.cpp \
    src/ui/EventTypesFilter.cpp \
    src/ui/EventVideoPlayer.cpp \
    src/video/VideoPlayerBackend_gst.cpp \
    src/video/VideoHttpBuffer.cpp \
    src/ui/EventVideoDownload.cpp \
    src/ui/ServerConfigWindow.cpp \
    src/ui/AboutDialog.cpp \
    src/ui/SavedLayoutsModel.cpp \
    src/core/ServerRequestManager.cpp

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
    src/core/EventData.h \
    src/ui/EventTypesFilter.h \
    src/ui/EventVideoPlayer.h \
    src/video/VideoPlayerBackend_gst.h \
    src/video/VideoHttpBuffer.h \
    src/video/VideoSurface.h \
    src/ui/EventVideoDownload.h \
    src/ui/ServerConfigWindow.h \
    src/ui/AboutDialog.h \
    src/ui/SavedLayoutsModel.h \
    src/core/ServerRequestManager.h

RESOURCES += \
    res/resources.qrc
