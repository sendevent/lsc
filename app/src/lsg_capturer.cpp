#include "lsg_capturer.h"
#include "lsg_capturingareaplugin.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>
#include <QDebug>

#if QT_VERSION >= 0x050000
#include <QWindow>
#include <QScreen>
#endif

QString now()
{
    return QTime::currentTime().toString( "hh:mm:ss:zzz" );
}

LSGCapturer::LSGCapturer( QObject *pParent /*= 0*/ )
    :QObject( pParent )
    ,mMaxCaptures( 0 )
    ,mCurrentCapturedNum( 0 )
    ,mCapturingDelay( 10 )
    ,mAreaSelector( 0 )
    ,mGIFSaver( new LGSGifSaver() )
    ,mSelectedAreaNum( 0 )
{
    connect( mGIFSaver, SIGNAL(savingProgress(int, int, QString)),
             this, SIGNAL(savingProgress(int, int, QString)));

    connect( &mTimer, SIGNAL(timeout()),
             this, SLOT(onTimerFired()) );
}

LSGCapturer::~LSGCapturer()
{
    delete mGIFSaver;
}

QPixmapPtr LSGCapturer::getCapture( int num /*= -1*/ )
{
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

void LSGCapturer::setAreaSelector( const LSGCapturingAreaPlugin *areaSelector, int option )
{
    if( !areaSelector )
    {
        qWarning() << Q_FUNC_INFO << "invalid area selctor pointer!";
        return;
    }
    
    mAreaSelector = areaSelector;
    mSelectedAreaNum = option;
    images.clear();
    mCurrentCapturedNum = 0;
}

void LSGCapturer::onTimerFired()
{

    doGrab();
}

QPixmapPtr LSGCapturer::startGrab()
{
    qDebug() << now() <<  Q_FUNC_INFO;
    
    images.clear();
    mCurrentCapturedNum = 0;

    mTimer.start( mCapturingDelay );
    
    return doGrab();
}

QPixmapPtr LSGCapturer::doGrab()
{
    ++mCurrentCapturedNum;
    
    QPixmapPtr pImg = shotScreen();
    
    if( pImg )
    {
        images.append( pImg );
        emit captured( images.size() );
    }
    
    if( !mMaxCaptures 
            || mCurrentCapturedNum >= mMaxCaptures )
    {
        mTimer.stop();
        emit finished();
    }
//    else
//    {
//        QTimer::singleShot( mCapturingDelay, this, SLOT( doGrab()) );
//    }
    
    return pImg;
}

void LSGCapturer::setMaxCapturesLimit( int i )
{
    mMaxCaptures = i;
}

void LSGCapturer::setCapturingDelay( int i )
{
    mCapturingDelay = 1000/i;
}


void LSGCapturer::saveSeparatedFiles( const QString& path )
{
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

void LSGCapturer::saveGIF()
{
    if( mGIFSaver )
    {
        mGIFSaver->save( images, mCapturingDelay/10 );
    }
}

QPixmapPtr LSGCapturer::shotScreen()
{
    QPixmapPtr img;
    if( mAreaSelector )
    {
        const QRectF r = mAreaSelector->getArea( mSelectedAreaNum ).boundingRect();
        img = QPixmapPtr( new QPixmap(
#if QT_VERSION >= 0x050000
        QGuiApplication::primaryScreen()->grabWindow(
#else
        QPixmap::grabWindow(
#endif
                                  QApplication::desktop()->winId(),
                                  r.x(),
                                  r.y(),
                                  r.width(),
                                  r.height() ) ) );
    }
    return img;
}
