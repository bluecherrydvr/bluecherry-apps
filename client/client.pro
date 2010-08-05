QT += core gui network phonon

TARGET = bluecherry-client
TEMPLATE = app

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
INCLUDEPATH += src

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
    src/ui/CameraSourcesModel.cpp \
    src/ui/EventsWindow.cpp

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
    src/ui/CameraSourcesModel.h \
    src/ui/EventsWindow.h
