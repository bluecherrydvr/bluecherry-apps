QT += core gui network phonon

TARGET = bluecherry-client
TEMPLATE = app

DEFINES += -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_TO_ASCII

SOURCES += src\main.cpp \
        src\MainWindow.cpp \
    src/CameraAreaWidget.cpp \
    src/CameraAreaControls.cpp \
    src/OptionsDialog.cpp \
    src/core/DVRServer.cpp \
    src/core/BluecherryApp.cpp

HEADERS  += src\MainWindow.h \
    src/CameraAreaWidget.h \
    src/CameraAreaControls.h \
    src/OptionsDialog.h \
    src/core/DVRServer.h \
    src/core/BluecherryApp.h
