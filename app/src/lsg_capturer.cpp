#include "lsg_capturer.h"
#include "lsg_capturingareaplugin.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>
#include <QMutexLocker>

#include <QDebug>

#if QT_VERSION >= 0x050000
#include <QWindow>
#include <QScreen>
#endif

#ifdef WITH_ANIMATED_GIF
    #include "lgs_gifsaver.h"
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
#ifdef WITH_ANIMATED_GIF
    ,mGIFSaver( new LGSGifSaver() )
#endif

    ,mSelectedAreaNum( 0 )
    ,mLastTime( 0 )
{
#ifdef WITH_ANIMATED_GIF
    connect( mGIFSaver, SIGNAL(savingProgress(int, int, QString)),
             this, SIGNAL(savingProgress(int, int, QString)));
#endif // WITH_ANIMATED_GIF
    connect( &mTimer, SIGNAL(timeout()),
             this, SLOT(onTimerFired()) );
}

LSGCapturer::~LSGCapturer()
{
    qDebug() << Q_FUNC_INFO;
    images.clear();
#ifdef WITH_ANIMATED_GIF
    delete mGIFSaver;
#endif //WITH_ANIMATED_GIF
}

QPixmapPtr LSGCapturer::getCapture( int num /*= -1*/ )
{
    qDebug() << Q_FUNC_INFO << num;
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
    qDebug() << Q_FUNC_INFO << now() <<  Q_FUNC_INFO;
    
    images.clear();

    mCurrentCapturedNum = 0;
    const QPixmapPtr pImg = shotScreen();
    for( int i = 0; i < mMaxCaptures; ++i )
    {
        images.append( QPixmapPtr( new QPixmap( pImg->width(), pImg->height() ) ) );
    }

    mTimer.start( mCapturingDelay );

    qDebug() << mTimer.interval();
    
    return doGrab();
}
qint64 started = 0;
QPixmapPtr LSGCapturer::doGrab()
{
    if( started == 0 ) started = QDateTime::currentMSecsSinceEpoch();
    qint64 start = QDateTime::currentMSecsSinceEpoch();
//    qDebug() << QDateTime::currentMSecsSinceEpoch() << mCurrentCapturedNum;
    QMutexLocker ml( &mMutex );
    if( mLastTime == 0 )
    {
        mLastTime = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        qint64 now = QDateTime::currentMSecsSinceEpoch();
        qDebug() << "time from prev:" << ( now - mLastTime) << mCurrentCapturedNum;
        mLastTime = now;
    }

    const QPixmapPtr pImg( shotScreen() );
    images.append( pImg );
    
    emit captured( images.size() );

    ++mCurrentCapturedNum;
    
    if( !mMaxCaptures 
            || mCurrentCapturedNum >= mMaxCaptures )
    {
        mTimer.stop();
        mLastTime = 0;
        qDebug() <<Q_FUNC_INFO << "session duration: " << (QDateTime::currentMSecsSinceEpoch() - started);
        started = 0;
        emit finished();
    }
//    else
//    {
//        QTimer::singleShot( mCapturingDelay, this, SLOT( doGrab()) );
//    }
    
    qDebug() <<Q_FUNC_INFO << "method duration: " << (QDateTime::currentMSecsSinceEpoch() - start);
    return pImg;
}

void LSGCapturer::setMaxCapturesLimit( int i )
{
    mMaxCaptures = i;
}

void LSGCapturer::actualizeScreenArea()
{
    mScreenRect = mAreaSelector->getArea( mSelectedAreaNum ).boundingRect();
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

#ifdef WITH_ANIMATED_GIF
void LSGCapturer::saveGIF() const
{
    if( mGIFSaver )
    {
        mGIFSaver->save( images, mCapturingDelay/10 );
    }
}
#endif // WITH_ANIMATED_GIF

QPixmapPtr LSGCapturer::shotScreen()
{
    QPixmapPtr img;
    if( mAreaSelector )
    {
        if( mScreenRect.isEmpty() )
            mScreenRect = mAreaSelector->getArea( mSelectedAreaNum ).boundingRect();

        img = QPixmapPtr( new QPixmap(
#if QT_VERSION >= 0x050000
        QGuiApplication::primaryScreen()->grabWindow(
#else
        QPixmap::grabWindow(
#endif
                                  QApplication::desktop()->winId(),
                                  mScreenRect.x(),
                                  mScreenRect.y(),
                                  mScreenRect.width(),
                                  mScreenRect.height() ) ) );
    }
    return img;
}
