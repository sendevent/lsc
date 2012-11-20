#include "lsg_mainwindow.h"

#include <QApplication>
#include <QtPlugin>
#include <QDebug>
Q_IMPORT_PLUGIN(FullScreenPLugin)


#include <gd.h>

QImage loadImg()
{
    static const QString imgName( "./test.png" );
    QImage img;
    img.load( imgName );
    
    return img;
}

void save( const QString& name, const QImage& img )
{
    const QString tmp( "./%1_%2.png" );
    for ( int i = 0; i <= 100; i+=10 )
    {
        img.save( tmp.arg( name ).arg( i ), "PNG", i );
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LSGMainWindow w;
    w.show();
    
    return a.exec();
    

//    FILE *f = fopen( "./test.gif", "rb" );
//    gdImagePtr gdImg = gdImageCreateFromGif( f );
    
//    FILE *f2 = fopen( "./test2.gif", "wb" );
//    gdImageGif( gdImg, f2 );
    
    
//    QImage img8 = loadImg();
//    img8 = img8.convertToFormat( QImage::Format_Indexed8 );
//    save( "test8", img8 );
    
//    QImage img32 = loadImg();
//    img32 = img32.convertToFormat( QImage::Format_ARGB32_Premultiplied );
//    save( "test32", img32 );
    
}
