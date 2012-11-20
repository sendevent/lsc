#ifndef LSG_CAPTURER_H
#define LSG_CAPTURER_H

#include <QObject>
#include <QWidget>
#include <QPixmap>

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
    void saveGIF( const QString& path );
    QPixmapPtr shotScreen();
    void setAreaSelector( LSGCapturingAreaPlugin *mAreaSelector );
    
signals:
    void captured( int count );
    void finished();
    void savingProgress( int steps, int step );
    
public slots:
    QPixmapPtr startGrab();
    
    
    void setMaxCapturesLimit( int i );
    void setCapturingDelay( int i );
    
protected slots:
    QPixmapPtr doGrab();
    
protected:
    QPixmapsList images;
    QWidget *pMainWnd;
    
    int mMaxCaptures, mCurrentCapturedNum;
    int mCapturingDelay;
    int mScreenNumber;
    
    //QPainterPath mCapturingPath;
    //QRectF mCapturingRect;

    LSGCapturingAreaPlugin *mAreaSelector;
    
    LGSGifSaver *mGIFSaver;
};

#endif // LSG_CAPTURER_H
