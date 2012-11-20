#include "lgs_gifsaver.h"

#include <QBuffer>
#include<QImage>
#include <QFile>
#include <QApplication>
#include <QDebug>

/*#include <gif_lib.h>
#include <gd.h>
#include <iostream>
#include <fstream>
#include <QTemporaryFile>
#include <FreeImage.h>*/

LGSGifSaver::LGSGifSaver(QObject *parent) :
    QObject(parent)
{
}

/*gdImagePtr createGDImage( const QImage& image )
{
    gdImagePtr gdImage;
    if( !image.isNull() )
    {
        QTemporaryFile file;
        if (file.open() ) 
        {
            QImage mono = image.convertToFormat( QImage::Format_ARGB32_Premultiplied);
            if( mono.save( &file, "PNG", 0 ) )
            {
                FILE * f = fopen( file.fileName().toStdString().c_str(), "rb" );
                if( f )
                {
                    gdImagePtr tmp = gdImageCreateFromPng( f );
                    gdImage = gdImageCreatePaletteFromTrueColor( tmp, mono.depth(), 256 );
                    //gdImageDestroy( tmp );
                    
                    fclose( f );
                }
                
            }
        }
    }
    return gdImage;
}*/



/*bool addAnimationFrames( gdImagePtr im, const QPixmapsList& imagesList, FILE *f, int delay )
{
    //qDebug() << Q_FUNC_INFO << delay;
    if( !im || !f)
    {
//        qDebug() << Q_FUNC_INFO << "invalid img ptr received, abort";
        return false;
    }
    
    if( imagesList.isEmpty() )
    {
//        qDebug() << Q_FUNC_INFO << "nothing to be added";
        return true;
    }
    
    //int trans = gdImageColorAllocate(im, 1, 1, 1);
    
    gdImagePtr prevGdImgPtr = 0;
    gdImageGifAnimBegin(im, f, 1, 0); //Пишем заголовок gif-а
    
    const int imagesCount = imagesList.size();
    for( int i = 0; i < imagesCount; ++i )
    {
        QImage qtImg = imagesList.at( i )->toImage();
        gdImagePtr currentGdImgPtr = createGDImage( qtImg );
        
//        if( prevGdImgPtr )
//        {
//            gdImagePaletteCopy( currentGdImgPtr, prevGdImgPtr );
//        }
        
        //gdImageColorAllocate(currentGdImgPtr, 255, 255, 255);
        if( imagesCount > 1 )
        {
            gdImagePaletteCopy (currentGdImgPtr, im);
            //gdImageColorTransparent (currentGdImgPtr, trans);
        }
        //gdImageCopyResampled
        gdImageCopy (im, currentGdImgPtr, 0,0,0,0, currentGdImgPtr->sx, currentGdImgPtr->sy );
        
//        FILE *tmpF = fopen ( QString( "gd_%1.png" ).arg( i ).toStdString().c_str(), "wb" );
//        gdImagePng( currentGdImgPtr, tmpF );
//        fclose( tmpF );
        
//        tmpF = fopen ( QString( "gd_%1.gif" ).arg( i ).toStdString().c_str(), "wb" );
//        gdImageGif( currentGdImgPtr, tmpF );
//        fclose( tmpF );
        
//        qtImg.save( QString( "qt_%1.png" ).arg( i ), "PNG", 30 );
                
        
        gdImageGifAnimAdd( currentGdImgPtr, f, 1, 0, 0, delay, gdDisposalNone, prevGdImgPtr );
        
        prevGdImgPtr = currentGdImgPtr;
    }
    gdImageGifAnimEnd( f );
    
    return true;
    
}

FIBITMAP *bitmapFromQImage( const QImage& image )
{
    FIBITMAP *res = 0;
    //qDebug() << Q_FUNC_INFO << image.isNull();
    if( !image.isNull() )
    {
        QTemporaryFile file;
        if (file.open() ) 
        {
            QImage indexed = image.convertToFormat( QImage::Format_Indexed8 );
            if( indexed.save( &file, "PNG", 0 ) )
            {
                res = FreeImage_Load( FIF_PNG, file.fileName().toStdString().c_str(), PNG_DEFAULT );
                res = FreeImage_ConvertTo8Bits( res );
                //qDebug() << Q_FUNC_INFO << "img saved" << (res != 0);
            }
        }
    }
    
    return res;
}

void FreeImageErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) 
{
  if(fif != FIF_UNKNOWN) 
  {
      qDebug() << FreeImage_GetFormatFromFIF(fif) << " Format";
  }
  qDebug() << message;
}



void LGSGifSaver::fiSave( const QString& path, const QPixmapsList& imagesList, int delay )
{
    // In your main program …
    FreeImage_SetOutputMessage(FreeImageErrorHandler);
    //qDebug() << Q_FUNC_INFO << path << imagesList.count();
    
    FIBITMAP *bitmap = bitmapFromQImage( imagesList.first()->toImage() );
    
    if (bitmap) 
    {
        //qDebug() << Q_FUNC_INFO << "1st img created!";
        // bitmap successfully loaded!
        
        FIMULTIBITMAP *multi = FreeImage_OpenMultiBitmap( FIF_GIF, path.toStdString().c_str(), true, false, true );

        if( multi )
        {
            //qDebug() << Q_FUNC_INFO << "multi img created!";
            DWORD dwFrameTime = (DWORD)delay;//((1000.0f / 10) + 0.5f);
            
            const int imagesCount = imagesList.count();
       
            for(int i = 0; i < imagesCount; ++i ) 
            {
                qDebug() << i << "/" << imagesCount;
                emit savingProgress( imagesCount+1, i );
                
                QImage currImg = imagesList.at( i )->toImage();
                FIBITMAP *currFrame = bitmapFromQImage( currImg );
                if( currFrame )
                {
                    FreeImage_SetMetadata( FIMD_ANIMATION, currFrame, NULL, NULL);
                    FITAG *tag = FreeImage_CreateTag();
                    if(tag) 
                    {
                        FreeImage_SetTagKey( tag, "FrameTime" );
                        FreeImage_SetTagType( tag, FIDT_LONG );
                        FreeImage_SetTagCount( tag, 1 );
                        FreeImage_SetTagLength( tag, 4 );
                        FreeImage_SetTagValue( tag, &dwFrameTime );
                        FreeImage_SetMetadata( FIMD_ANIMATION, currFrame, FreeImage_GetTagKey( tag ), tag );
                        FreeImage_DeleteTag( tag );
                    }
                    
                    FreeImage_AppendPage( multi, currFrame );
                    FreeImage_Unload( currFrame );
                }
                qApp->processEvents();
            }
            qDebug() <<"cloooosin";
            emit savingProgress( imagesCount+1, imagesCount );
            qApp->processEvents();
            FreeImage_CloseMultiBitmap(multi);
        }
        FreeImage_Unload(bitmap);
    }
    qDebug() << "saved!";
}
void LGSGifSaver::save( const QString& path, const QPixmapsList& imagesList, int delay )
{
    //qDebug() << Q_FUNC_INFO << path << imagesList.count();
    
    if( !imagesList.isEmpty() )
        fiSave( path, imagesList, delay );
}

void gdSave( const QString& path, const QPixmapsList& imagesList, int delay )
{
    
    
    gdImagePtr im = createGDImage( imagesList.first()->toImage() );
    
    FILE *out1; //обьявляем указатель на файл
    
    
    out1 = fopen( path.toStdString().c_str(), "wb");

    double dDelay = delay/10.;
    bool ok = addAnimationFrames( im, imagesList, out1, dDelay );
    
    //qDebug() << "saved: " <<ok;
    
    fclose(out1); //закрываем файлик
    
    gdImageDestroy(im); // убираем за собой
    
}

int doOutput(GifFileType* gif, const GifByteType * data, int i)
{
    QIODevice* out = (QIODevice*)gif->UserData;
    //qDebug("given %d bytes to write; device is writeable? %d", i, out->isWritable());
    return out->write((const char*)data, i);
}

int doInput(GifFileType* gif, GifByteType* data, int i)
{
    QIODevice* in = (QIODevice*)gif->UserData;
    return in->read((char*)data, i);
}

void LGSGifSaver::write( const QString& path, const QPixmapsList& imagesList )
{
    QImage toWrite = imagesList.first()->toImage();
    /// @todo how to specify dithering method
    if (toWrite.numColors() == 0 || toWrite.numColors() > 256)
        toWrite = toWrite.convertToFormat(QImage::Format_RGB16);
    
    QVector<QRgb> colorTable = toWrite.colorTable();
    ColorMapObject cmap;
    // numColors must be a power of 2
    int numColors = 1 << GifBitSize(toWrite.numColors());
    cmap.ColorCount = numColors;
    //qDebug() << "cmap.ColorCount" << cmap.ColorCount << toWrite.numColors();
    cmap.BitsPerPixel = toWrite.depth();	/// @todo based on numColors (or not? we did ask for Format_Indexed8, so the data is always 8-bit, right?)
    GifColorType* colorValues = (GifColorType*)malloc(cmap.ColorCount * sizeof(GifColorType));
    cmap.Colors = colorValues;
    int c = 0;
    for(; c < toWrite.numColors(); ++c)
    {
        //qDebug("color %d has %02X%02X%02X", c, qRed(colorTable[c]), qGreen(colorTable[c]), qBlue(colorTable[c]));
        colorValues[c].Red = qRed(colorTable[c]);
        colorValues[c].Green = qGreen(colorTable[c]);
        colorValues[c].Blue = qBlue(colorTable[c]);
    }
    // In case we had an actual number of colors that's not a power of 2,
    // fill the rest with something (black perhaps).
    for (; c < numColors; ++c)
    {
        colorValues[c].Red = 0;
        colorValues[c].Green = 0;
        colorValues[c].Blue = 0;
    }
    
    /// @todo transparent GIFs (use alpha?)
    
    /// @todo how to specify which version, or decide based on features in use
    // Because of this call, libgif is not re-entrant
    //	EGifSetGifVersion("89a");
    
    /// @todo write to m_device
    
    QFile f( path );
    
    if( !f.open( QIODevice::WriteOnly ) )
    {
        qWarning() << Q_FUNC_INFO << "can't open file: " << path << " error: " << f.errorString();
    }
    int err = 0;
    GifFileType * gif = EGifOpen(&f, doOutput, &err);
    //	GifFileType* gif = EGifOpenFileName("/tmp/out.gif", 0);
    
    for( int i = 0; i < imagesList.count(); ++i )
    {
        QImage image = imagesList.at( i )->toImage();
        //qDebug() << Q_FUNC_INFO << "image " << i << "/" << imagesList.count() << " valid: " << !image.isNull();
        
        if (EGifPutScreenDesc(gif, image.width(), image.height(), numColors, 0, &cmap) == GIF_ERROR)
            qCritical("EGifPutScreenDesc failed" );
        
        /// @todo foreach of multiple images in an animation...
        if (EGifPutImageDesc(gif, 0, 0, image.width(), image.height(), 0, &cmap) == GIF_ERROR)
            qCritical("EGifPutImageDesc  failed" );
        
        int lc = image.height();
        int llen = image.bytesPerLine()/4;
        //	qDebug("will write %d lines, %d bytes each", lc, llen);
        for (int l = 0; l < lc; ++l)
        {
            //qDebug() << "line " << l << " of " << lc << "wrote: " << llen * l;
            //qDebug() << "bytesPerLine:" << llen;
            //qDebug() << "WxH=:" << image.width() << " x " << image.height() << " = " << image.width() * image.height();
            
            QRgb *line = (QRgb*)image.scanLine(l);
            int err = EGifPutLine(gif, (GifPixelType*)line, llen);
            if (err != GIF_OK )
            {
                //int i = GifLastError();
                //qDebug() << "EGifPutLine returned error  failed" << QString( GifErrorString( gif->Error ));
            }
        }
    }
    
    EGifCloseFile(gif);
}*/
