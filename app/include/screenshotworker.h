#ifndef SCREENSHOTTHREAD_H
#define SCREENSHOTTHREAD_H

#include <QThread>
#include <QQueue>
#include <QMutex>

class LSCWorker : public QObject
{
    Q_OBJECT
public:
    explicit LSCWorker( quint8 fps, int duration );
    ~LSCWorker();



    QQueue<QString> mQueue;

signals:
    void capture( int num );
    void finished();

public slots:
    void start();

private:
    const quint8 mFps;
    const int mDuration;

    QMutex mMutex;
    void customWait( int millisecondsToWait );
};


#endif // SCREENSHOTTHREAD_H
