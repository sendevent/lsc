#include "capturer.h"
#include "capturingareaplugin.h"


#include <QThreadPool>

#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QTime>
#include <QMutexLocker>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

#if QT_VERSION >= 0x050000
#include <QWindow>
#include <QScreen>
#endif

#ifdef WITH_ANIMATED_GIF
    #include "gifsaver.h"
#endif

LSCCapturer::LSCCapturer( QObject *pParent /*= 0*/ )
    :QObject( pParent )
    ,mFps( 0 )
    ,mCurrentCapturedNum( 0 )
    ,mDuration( 10 )
    ,mAreaSelector( 0 )
#ifdef WITH_ANIMATED_GIF
    ,mGIFSaver( new LGSGifSaver() )
#endif

    ,mSelectedAreaNum( 0 )
    ,mLastTime( 0 )
    ,workerPtr(0)
{
#ifdef WITH_ANIMATED_GIF
    connect( mGIFSaver, SIGNAL(savingProgress(int, int, QString)),
             this, SIGNAL(savingProgress(int, int, QString)));
#endif // WITH_ANIMATED_GIF
}

LSCCapturer::~LSCCapturer()
{
    qDebug() << Q_FUNC_INFO;
    images.clear();
#ifdef WITH_ANIMATED_GIF
    delete mGIFSaver;
#endif //WITH_ANIMATED_GIF
}

QPixmapPtr LSCCapturer::getCapture()
{
    if( !images.isEmpty() )
        return images.last();

    return QPixmapPtr();
}

void LSCCapturer::setAreaSelector( const LSGCapturingAreaPlugin *areaSelector, int option )
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

void LSCCapturer::startGrab()
{
    mLastTime = QDateTime::currentMSecsSinceEpoch();

    mCurrentCapturedNum = 0;

    images.clear();

    const QPixmapPtr pImg = shotScreenSync();
    const int imgCnt = mFps*mDuration;
    images.reserve(imgCnt);

    for( int i = 0; i < imgCnt; ++i)
    {
        images.append( QPixmapPtr(new QPixmap(*pImg.data())));
    }

    if( workerPtr )
    {
        delete workerPtr;
    }
    workerPtr = new LSCWorker( mFps, mDuration );
    workerPtr->moveToThread(&workerThread);
    connect( workerPtr, SIGNAL(capture(int)), this, SLOT(onCaptureRequested(int)) );
    connect( workerPtr, SIGNAL(finished()), this, SLOT(onFinished()) );

    connect(this, SIGNAL(startCapturing()), workerPtr, SLOT(start()) );

    workerThread.start();



    QTimer::singleShot( 10, this, SIGNAL(startCapturing()) );
}

void LSCCapturer::onFinished()
{
    qDebug() << Q_FUNC_INFO << "duration:" <<( QDateTime::currentMSecsSinceEpoch() - mLastTime ) << QThread::currentThreadId() << "caps:" << images.size();

    workerThread.quit();
    workerThread.wait();

    emit finished();
}

QPixmapPtr LSCCapturer::shotScreenSync()
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


void LSCCapturer::setFps( int i )
{
    mFps = i;
}

void LSCCapturer::actualizeScreenArea()
{
    mScreenRect = mAreaSelector->getArea( mSelectedAreaNum ).boundingRect();
}

void LSCCapturer::setDuration( int seconds )
{
    mDuration = seconds;
}


void LSCCapturer::saveSeparatedFiles( const QString& path )
{
    static const QString tmpl( "_%1.png" );
    for(  int i = 0; i < images.size(); ++i )
    {
        const QString name = tmpl.arg( i ).prepend( path );
        QFile f( name );

        emit savingProgress( images.size(), i, tr( "Saving %1" ).arg( name  ) );

        const QPixmapPtr pPixmap = images.at( i );

        bool saved = false;
        if( !pPixmap
                || !f.open( QIODevice::WriteOnly )
                || !pPixmap->save( &f, "PNG" ))
        {
            static const QString title( tr( "Save failed" ) );
            static const QString msgTmpl( tr( "Can't save file\n%1\n%2" ) );
            if( QMessageBox::Abort == QMessageBox::question( 0, title, msgTmpl.arg( name ).arg( pPixmap ? f.errorString() : QString::null ), QMessageBox::Abort, QMessageBox::Ignore ) )
            {
                return;
            }
        }
    }
}

#ifdef WITH_ANIMATED_GIF
void LSCCapturer::saveGIF() const
{
    if( mGIFSaver )
    {
        mGIFSaver->save( images, mDuration/10 );
    }
}
#endif // WITH_ANIMATED_GIF


void LSCCapturer::onCaptureRequested( int imgNum  )
{
    qDebug() << "capture" << imgNum  << "\t\t" << QDateTime::currentMSecsSinceEpoch(); ;

//    qApp->processEvents();

//    images.append( shotScreenSync() );
    images[imgNum] = shotScreenSync();

    if( workerPtr && !(workerPtr->mQueue.isEmpty() ) )
       workerPtr->mQueue.dequeue();
}
