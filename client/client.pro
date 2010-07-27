QT += core gui network phonon

TARGET = bluecherry-client
TEMPLATE = app

DEFINES += -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII
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
    src/ui/OptionsServerPage.cpp

HEADERS  += src/ui/MainWindow.h \
    src/ui/CameraAreaWidget.h \
    src/ui/CameraAreaControls.h \
    src/ui/OptionsDialog.h \
    src/core/DVRServer.h \
    src/core/BluecherryApp.h \
    src/ui/DVRServersModel.h \
    src/ui/DVRServersView.h \
    src/ui/OptionsServerPage.h
