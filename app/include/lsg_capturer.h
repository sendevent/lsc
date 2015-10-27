#ifndef LSG_CAPTURER_H
#define LSG_CAPTURER_H

#include <QObject>
#include <QWidget>
#include <QPixmap>
#include <QTimer>

#include "lgs_gifsaver.h"

class LSGCapturingAreaPlugin;

class LSGCapturer : public QObject
{
    Q_OBJECT
public:
    
    LSGCapturer( QObject *pParent = 0 );
    ~LSGCapturer();
    
    QPixmapPtr getCapture( int num = -1 );
    void saveSeparatedFiles( const QString& path );
    void saveGIF();
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
    
protected slots:
    QPixmapPtr doGrab();
    
protected:
    QPixmapsList images;
    
    int mMaxCaptures, mCurrentCapturedNum;
    int mCapturingDelay;
    
    const LSGCapturingAreaPlugin *mAreaSelector;
    
    LGSGifSaver *mGIFSaver;

    int mSelectedAreaNum;

    QTimer mTimer;
};

#endif // LSG_CAPTURER_H
