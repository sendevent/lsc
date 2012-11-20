#include "lsg_capturer.h"
#include "lsg_capturingareaplugin.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>
#include <QDebug>

QString now()
{
    return QTime::currentTime().toString( "hh:mm:ss:zzz" );
}

LSGCapturer::LSGCapturer( QObject *pParent /*= 0*/ )
    :QObject( pParent )
    ,mMaxCaptures( 0 )
    ,mCurrentCapturedNum( 0 )
    ,mCapturingDelay( 250 )
    ,mScreenNumber( 0 )
    ,mAreaSelector( 0 )
    ,mGIFSaver( new LGSGifSaver() )
{
    QWidget *mainWnd = qobject_cast<QWidget*>( pParent );
    //-- must be ok!
//    if( mainWnd )
//    {
        pMainWnd = mainWnd;
    //}
    
    qDebug() << now() <<  Q_FUNC_INFO;
    
    connect( mGIFSaver, SIGNAL(savingProgress(int, int)),
             this, SIGNAL(savingProgress(int, int)));
}

LSGCapturer::~LSGCapturer()
{
    delete mGIFSaver;
}

QPixmapPtr LSGCapturer::getCapture( int num /*= -1*/ )
{
    qDebug() << now() <<  Q_FUNC_INFO;
    QPixmapPtr res;
    if( images.isEmpty() 
            || num >= images.size() )
    {
        res = doGrab();
    }
    else if( num < 0 )
    {
        res = images.last();
    }
    else
    {
        res = images.at( num );
    }
    return res;
}

void LSGCapturer::setAreaSelector( LSGCapturingAreaPlugin *areaSelector )
{
    if( !areaSelector )
    {
        qWarning() << Q_FUNC_INFO << "invalid area selctor pointer!";
        return;
    }
    
    mAreaSelector = areaSelector;
    //mCapturingPath = areaSelector->getArea();
    //mCapturingRect = mCapturingPath.boundingRect();
    images.clear();
    mCurrentCapturedNum = 0;
}

QPixmapPtr LSGCapturer::startGrab()
{
    qDebug() << now() <<  Q_FUNC_INFO;
    
    images.clear();
    mCurrentCapturedNum = 0;
    
    return doGrab();
}

QPixmapPtr LSGCapturer::doGrab()
{
    qDebug() << now() <<  Q_FUNC_INFO << mCurrentCapturedNum;
    
    ++mCurrentCapturedNum;
    
    QPixmapPtr pImg = shotScreen();
    
    
    
    if( pImg )
    {
        images.append( pImg );
        
        qDebug() << now() <<  Q_FUNC_INFO << "grabbed!" << images.size();
        
        emit captured( images.size() );
    }
    
    if( !mMaxCaptures 
            || mCurrentCapturedNum >= mMaxCaptures )
    {
        emit finished();
    }
    else
    {
        QTimer::singleShot( mCapturingDelay, this, SLOT( doGrab()) );
    }
    
    return pImg;
}

/*static*/ /*QMap<quint8,QString> LSGCapturer::getModesNames()
{
    static QMap<quint8,QString> res;
    if( res.isEmpty() )
    {
        res.insert( CP_ALL, tr( "Full screen" ) );
        res.insert( CP_DISPLAY, tr( "Display" ) );
        res.insert( CP_WINDOW, tr( "Window" ) );
        res.insert( CP_AREA, tr( "Rectangle" ) );
    }
    
    return res;
}*/



void LSGCapturer::setMaxCapturesLimit( int i )
{
    mMaxCaptures = i;
}

void LSGCapturer::setCapturingDelay( int i )
{
    mCapturingDelay = i;
}


void LSGCapturer::saveSeparatedFiles( const QString& path )
{
    qDebug() << now() <<  Q_FUNC_INFO << 1;
    static const QString tmpl( "_%1.png" );
    QList<QPixmapPtr>::const_iterator it;
    int i = 0;
    qDebug() << now() <<  Q_FUNC_INFO << 2;
    for( it = images.constBegin(); it != images.constEnd(); ++ it )
    {
        ++i;
        const QString name = tmpl.arg( i ).prepend( path );
        QPixmapPtr pPixmap = *it;
        if( pPixmap )
        {
            bool res = (*it)->save( name );
            
            qDebug() << now() <<  Q_FUNC_INFO << name << " saved: " << res;
        }
        else
        {
            qDebug() << now() <<  Q_FUNC_INFO << name << " skipped invalid img";
        }
    }
}

void LSGCapturer::saveGIF( const QString& /*path*/ )
{
    /*if( mGIFSaver )
        mGIFSaver->save( path, images, mCapturingDelay );*/
}

QPixmapPtr LSGCapturer::shotScreen()
{
    QPixmapPtr img;
    if( mAreaSelector )
    {
        const QRectF r = mAreaSelector->getArea().boundingRect();
        qDebug() << now() <<  Q_FUNC_INFO << r;
        img = QPixmapPtr( new QPixmap( QPixmap::grabWindow( QApplication::desktop()->winId(), 
                                                        r.x(), 
                                                        r.y(), 
                                                        r.width(), 
                                                        r.height() ) ) );
    }
    qDebug() << now() <<  Q_FUNC_INFO << img->isNull();
    return img;
}
