#-------------------------------------------------
#
# Project created by QtCreator 2012-09-08T17:35:51
#
#-------------------------------------------------

QT       += core gui
TEMPLATE = lib
CONFIG   += plugin
TARGET = FullScreenPLugin

CONFIG(debug, debug|release) {
    OUT_PATH = $$PWD/../../bin_debug/plugins
    DEFINES += DEBUG
} else {
    OUT_PATH = $$PWD/../../bin_release/plugins
}

DESTDIR = $$OUT_PATH
OBJECTS_DIR = $$OUT_PATH/intermediate/obj
MOC_DIR = $$OUT_PATH/intermediate/moc
RCC_DIR = $$OUT_PATH/intermediate/rcc
UI_DIR = $$OUT_PATH/intermediate/ui

INCLUDEPATH += ../../app/include/

SOURCES += \
    capturingfullscreen.cpp



HEADERS  += \
    ../../app/include/capturingareaplugin.h \
    capturingfullscreen.h


FORMS    +=

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
}
