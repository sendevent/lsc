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

SOURCES += src/main.cpp \
    src/gifsaver.cpp \
    src/capturer.cpp \
    src/mainwindow.cpp \
    src/screenshotworker.cpp \
    src/gifframepreviewer.cpp \
    src/imagewrapper.cpp \
    framepreview.cpp \
    thumbsrow.cpp

HEADERS  += \
    include/gifsaver.h \
    include/capturer.h \
    include/capturingareaplugin.h \
    include/mainwindow.h \
    include/screenshotworker.h \
    include/gifframepreviewer.h \
    include/imagewrapper.h \
    framepreview.h \
    thumbsrow.h

FORMS    += \
    src/ui/displayselector.ui \
    src/ui/gifframepreviewer.ui \
    src/ui/mainwindow.ui \
    framepreview.ui \
    thumbsrow.ui

RESOURCES += src/resources.qrc

animatedgif {
DEFINES = WITH_ANIMATED_GIF
unix {
#    INCLUDEPATH  += /usr/include/ImageMagick-6
    QMAKE_CXXFLAGS += `pkg-config --cflags Magick++`
    LIBS += `pkg-config --libs Magick++`
}
windows {
    IMAGEMAGICK_LIBS=$$(IMAGEMAGICK_LIBS)
    IMAGEMAGICK_INCS=$$(IMAGEMAGICK_INCS)

    isEmpty(IMAGEMAGICK_LIBS) {
        message("Environment variable 'IMAGEMAGICK_LIBS' should point to dir with ImageMagic's libs" )
    }
    isEmpty(IMAGEMAGICK_INCS) {
        message("Environment variable 'IMAGEMAGICK_INCS' should point to dir with ImageMagic's includes" )
    }

    INCLUDEPATH += $$IMAGEMAGICK_INCS
    QMAKE_LIBDIR += $$IMAGEMAGICK_LIBS
greaterThan(QT_MAJOR_VERSION, 4){
    LIBS += -lMagick++-6.Q16-1 -lMagickCore-6.Q16-1
}else{
    LIBS += -lCORE_RL_Magick++_ -lCORE_RL_Magick_
}
}
SOURCES += \
    src/gifoptionsdialog.cpp
HEADERS  += \
    include/gifoptionsdialog.h
FORMS    += src/ui/gifoptionsdialog.ui
}
