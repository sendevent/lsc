#-------------------------------------------------
#
# Project created by QtCreator 2012-09-06T16:57:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LiveScreenCapture
TEMPLATE = app

CONFIG(debug, debug|release) {
    OUT_PATH = $$PWD/../bin_debug/
    

    DEFINES += DEBUG
} else {
    OUT_PATH = $$PWD/../bin_release/
    
}

DESTDIR = $$OUT_PATH
OBJECTS_DIR = $$OUT_PATH/intermediate/obj
MOC_DIR = $$OUT_PATH/intermediate/moc
RCC_DIR = $$OUT_PATH/intermediate/rcc
UI_DIR = $$OUT_PATH/intermediate/ui

INCLUDEPATH  += include

LIBS = -L$$OUT_PATH/plugins/ \
-lFullScreenPLugin


SOURCES += src/main.cpp\
        src/lsg_mainwindow.cpp \
    src/lsg_capturer.cpp \
    lgs_gifsaver.cpp



HEADERS  += include/lsg_mainwindow.h \
    include/lsg_capturer.h \
    include/lsg_capturingareaplugin.h \
    lgs_gifsaver.h

FORMS    += src/ui/lsg_mainwindow.ui \
    src/ui/lsg_displayselector.ui

RESOURCES += \
    src/resources.qrc
