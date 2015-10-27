#include "lsg_mainwindow.h"

#include <QApplication>
#include <QtPlugin>
#include <QDebug>

#include <Magick++.h>

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050000
    qSetMessagePattern( "%{file}#%{line}, %{function}: %{message}" );
#endif // QT_VERSION >= 0x050000

    Magick::InitializeMagick(*argv);

    QApplication a(argc, argv);
    LSGMainWindow w;
    w.show();
    
    return a.exec();
    
}
