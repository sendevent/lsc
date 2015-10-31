#ifndef LSCCAPTURER_H
#define LSCCAPTURER_H

#include <QObject>

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QSharedPointer>
#include <QQueue>
#include <QMutex>

#include "common.h"
#include "screenshotworker.h"

class LGSGifSaver;
class LSGCapturingAreaPlugin;


class LSCCapturer : public QObject
{
    Q_OBJECT
public:

    LSCCapturer( QObject *pParent = 0 );
    ~LSCCapturer();

    void saveSeparatedFiles( const QString& path );

#ifdef WITH_ANIMATED_GIF
    void saveGIF() const;
#endif // WITH_ANIMATED_GIF

    void setAreaSelector( const LSGCapturingAreaPlugin *mAreaSelector, int option = 0 );

    QPixmapPtr getCapture();

signals:
    void captured( int count, const QPixmapPtr img );
    void finished();
    void savingProgress( int steps, int step, const QString& msg );
    void startCapturing();

public slots:
    void startGrab();
    void setFps( int i );
    void setDuration( int i );
    void actualizeScreenArea();

protected slots:

    void captureRequest();
    void onFinished();

protected:
    PixmapsList images;

    int mFps, mCurrentCapturedNum;
    int mDuration;

    const LSGCapturingAreaPlugin *mAreaSelector;

#ifdef WITH_ANIMATED_GIF
    LGSGifSaver *mGIFSaver;
#endif

    int mSelectedAreaNum;
    QRectF mScreenRect;

    qint64 mLastTime;

    QPixmapPtr shotScreenSync();
    QThread workerThread;
    LSCWorker *workerPtr;
};

#endif // LSCCAPTURER_H
