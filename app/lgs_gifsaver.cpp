#include "lgs_gifsaver.h"

#include <QBuffer>
#include<QImage>
#include <QFile>
#include <QApplication>
#include <QDebug>

#include <Magick++.h>

#include "gifoptionsdialog.h"

LGSGifSaver::LGSGifSaver(QObject *parent) :
    QObject(parent)
  , mSaveDlg( new GifOptionsDialog )
{
}

LGSGifSaver::~LGSGifSaver()
{
    delete mSaveDlg;
}

void LGSGifSaver::save( const QPixmapsList& imagesList, int delay ) const
{
    if( mSaveDlg->exec() != QDialog::Accepted )
        return;

    const int quality = mSaveDlg->getQuality();
    const QString path = mSaveDlg->getPath();
    const Magick::CompressionType compressionType = (Magick::CompressionType)mSaveDlg->getCompression();

    const int totalSteps = imagesList.size() + 2;
    try
    {
        QVector<Magick::Image> frames;
        for( int i = 0; i < imagesList.size(); ++i )
        {
            const QPixmapPtr pPixmap = imagesList.at( i );
            emit savingProgress( totalSteps, i, tr( "Composing GIF" ) );
            QByteArray ba;
            QBuffer buf( &ba );
            buf.open( QIODevice::WriteOnly );
            pPixmap->save( &buf, "PNG", 100 );
//            buf.close();

            Magick::Image img( Magick::Blob( ba.data(), ba.size() ) );
            img.compressType( compressionType );
            img.quality( quality );
            img.animationDelay( delay );
//            img.gifDisposeMethod( 4 );
            img.magick( "GIF" );
            frames.append( img );
        }

        emit savingProgress( totalSteps, imagesList.size() + 1, tr( "Saving GIF" ) );

        Magick::writeImages( frames.begin(), frames.end(), path.toStdString() );

        emit savingProgress( totalSteps, imagesList.size() + 2, tr( "Saved! %1" ).arg( path ) );
    }
    catch( Magick::Exception &error_ )
    {
        qWarning() << "Caught exception: " << error_.what() << endl;
    }
}
