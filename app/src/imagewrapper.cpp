#include "imagewrapper.h"

ImageWrapper::ImageWrapper( const QPixmap & pixmap )
    : mPixmap( new QPixmap( pixmap ) )
    , isFrame( true )
{

}

ImageWrapper::ImageWrapper( const ImageWrapper &other )
    : mPixmap( other.mPixmap )
    , isFrame( other.isFrame )
{

}
