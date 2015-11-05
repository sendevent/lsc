#include "gifframepreviewer.h"
#include "ui_gifframepreviewer.h"
#include "framepreview.h"
#include "thumbsrow.h"

#include <QHBoxLayout>
#include <QListView>
#include <QScrollBar>
#include <QDebug>
#define LOG qDebug()

GifFramePreviewer::GifFramePreviewer(QWidget *parent /*= 0*/) :
    QFrame(parent),
    ui(new Ui::GifFramePreviewer)
  , pThumbsRow( 0 )
{
    ui->setupUi(this);
    ui->previewLabel->move( 0,0 );

    ui->scrollArea->setBackgroundRole(QPalette::Dark);
//    QHBoxLayout *pLayout = new QHBoxLayout;
//    ui->thumbsHolder->setLayout( pLayout );

    connect( ui->scrollArea->horizontalScrollBar(), SIGNAL(valueChanged(int)),
             this, SLOT(test(int)) );
}

GifFramePreviewer::~GifFramePreviewer()
{
    delete ui;
}

void GifFramePreviewer::setImages( ImagesList pImages )
{
    mCurrFrameNum = 0;
    mImagesList = pImages;

    if( pThumbsRow )
        delete pThumbsRow;

    pThumbsRow = new ThumbsRow( 0 );
    ui->scrollArea->setWidget( pThumbsRow );

    for( int i = 0; i < mImagesList.size(); ++i )
    {
        FramePreview *framePreview = new FramePreview( mImagesList.at( i ), i );
//        ui->thumbsHolder->layout()->addWidget( framePreview );
        pThumbsRow->addThumb( framePreview );

        qApp->processEvents();

        LOG << ui->scrollArea->horizontalScrollBar()->maximum();

        connect(framePreview, SIGNAL(clicked(QPoint,int)),
                this, SLOT(onPreviewSelected(QPoint,int)));
    }
    updateImage();
}

ImageWrapperPtr GifFramePreviewer::currentFrame() const
{
    return mCurrFrameNum >= 0 && mCurrFrameNum < mImagesList.size()
            ? mImagesList.at( mCurrFrameNum )
            : ImageWrapperPtr( 0 );
}
void GifFramePreviewer::updateImage()
{
    const ImageWrapperPtr pImg = currentFrame();
    if( pImg )
    {
        ui->previewLabel->setPixmap( pImg->img()->scaled( ui->previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    }
}

void GifFramePreviewer::resizeEvent( QResizeEvent * event )
{
    QFrame::resizeEvent( event );

    ui->previewLabel->resize( ui->frame->size() );

    updateImage();
}

void GifFramePreviewer::onPreviewSelected( const QPoint& pnt,int num )
{
    if( num < mImagesList.size() )
    {
        mCurrFrameNum = num;
    }

    updateImage();

//    LOG << pnt << ui->scrollArea->pos();// << ui->scrollArea->horizontalScrollBar()->value() << "/" << ui->scrollArea->horizontalScrollBar()->maximum();
    ui->scrollArea->horizontalScrollBar()->setValue( mapFromGlobal( pnt ).x() );
//    ui->scrollArea->scroll( ui->scrollArea->pos().x() - pnt.x(), 0 );

//scroll( pnt.x() - ui->scrollArea->pos().x(), 0 );

//    LOG <<2<< pnt << ui->scrollArea->pos();
}

void  GifFramePreviewer::test( int i )
{
    LOG << i;
}
