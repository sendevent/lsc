#ifndef IMAGEWRAPPER_H
#define IMAGEWRAPPER_H

#include <QPixmap>
#include <QImage>

#include <QSharedPointer>


typedef QSharedPointer<QPixmap> ImageDataPtr;

class ImageWrapper
{
public:
    ImageWrapper( const QPixmap &pixmap );
    ImageWrapper( const ImageWrapper &other );

    ImageDataPtr img() const { return mPixmap; }

    void setUsed( bool on ) { isFrame = on; }
    bool getUsed() const { return isFrame; }


private:
    ImageWrapper();

    ImageDataPtr mPixmap;
    bool isFrame;
};

typedef QSharedPointer<ImageWrapper> ImageWrapperPtr;

typedef QList<ImageWrapperPtr> ImagesList;

#endif // IMAGEWRAPPER_H
