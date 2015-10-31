#ifndef LSG_CAPTURER_H
#define LSG_CAPTURER_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QSharedPointer>
#include <QList>
#include <QMutex>

#include "common.h"

class LGSGifSaver;
class LSGCapturingAreaPlugin;




class LSGCapturer : public QObject
{
    Q_OBJECT
public:
    
    LSGCapturer( QObject *pParent = 0 );
    ~LSGCapturer();
    
    QPixmapPtr getCapture( int num = -1 );
    void saveSeparatedFiles( const QString& path );

#ifdef WITH_ANIMATED_GIF
    void saveGIF() const;
#endif // WITH_ANIMATED_GIF

    QPixmapPtr shotScreen();
    void setAreaSelector( const LSGCapturingAreaPlugin *mAreaSelector, int option = 0 );
    
signals:
    void captured( int count );
    void finished();
    void savingProgress( int steps, int step, const QString& msg );
    
public slots:
    QPixmapPtr startGrab();
    void onTimerFired();
    
    
    void setMaxCapturesLimit( int i );
    void setCapturingDelay( int i );
    void actualizeScreenArea();
    
protected slots:
    QPixmapPtr doGrab();
    
protected:
    PixmapsList images;
    
    int mMaxCaptures, mCurrentCapturedNum;
    int mCapturingDelay;
    
    const LSGCapturingAreaPlugin *mAreaSelector;

#ifdef WITH_ANIMATED_GIF
    LGSGifSaver *mGIFSaver;
#endif

    int mSelectedAreaNum;
    QRectF mScreenRect;

    QTimer mTimer;
    QMutex mMutex;

    qint64 mLastTime;
};

#endif // LSG_CAPTURER_H
