#include "mainwindow.h"

#include <QApplication>
#include <QtPlugin>
#include <QDebug>

#ifdef WITH_ANIMATED_GIF
#include <Magick++.h>
#endif // WITH_ANIMATED_GIF
int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050000
    qSetMessagePattern( "%{file}#%{line}, %{function}: %{message}" );
#endif // QT_VERSION >= 0x050000

#ifdef WITH_ANIMATED_GIF
    Magick::InitializeMagick(*argv);
#endif //WITH_ANIMATED_GIF

    QApplication a(argc, argv);
    LSGMainWindow w; // schedules first screenshot
    w.hide(); // and will be shown after that automaticaly
    
    return a.exec();
    
}
