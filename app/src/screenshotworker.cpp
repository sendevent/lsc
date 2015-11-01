#include "screenshotworker.h"
#include <QDateTime>
#include <QCoreApplication>
#include <QTime>

#include <QDebug>


LSCWorker::LSCWorker( quint8 fps, int duration )
    : QObject()
    , mFps( fps )
    , mDuration( duration )
{

}

LSCWorker::~LSCWorker()
{
}

void LSCWorker::customWait( int millisecondsToWait )
{
    const QTime& dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

void LSCWorker::start()
{
    const double period = (1.0/mFps)*1000;

    int currentFrame = 0;
    const int framesCnt = mFps*mDuration;

    qDebug() << "period:"<<period << "mFps:" << mFps << "duration:" << mDuration << "framesCnt:"<<framesCnt;

    while ( currentFrame < framesCnt || (currentFrame == 0 && framesCnt == 1))
    {
        const qint64 started = QDateTime::currentMSecsSinceEpoch();

        while( !mQueue.isEmpty() )
        {
            continue;
        }

        qDebug() << "request for capture" << currentFrame << "\t\t" << QDateTime::currentMSecsSinceEpoch();
        mQueue.enqueue( "capture" );
        emit capture( currentFrame );
//        qApp->processEvents();

        if( framesCnt > 1 )
        {
            const qint64 spent = QDateTime::currentMSecsSinceEpoch() - started;
            const qint64 wait = period - spent;

            if ( wait>0 ) customWait( wait );
        }

        ++currentFrame;

    }

    mMutex.lock(); // to avoid dequeueing from LSCCapturer::onCaptureRequested
    mQueue.clear();
    mMutex.unlock();

    emit finished();
}
