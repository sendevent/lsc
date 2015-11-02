#include "gifframepreviewer.h"
#include "ui_gifframepreviewer.h"

GifFramePreviewer::GifFramePreviewer(QWidget *parent /*= 0*/) :
    QFrame(parent),
    ui(new Ui::GifFramePreviewer)
{
    ui->setupUi(this);
    ui->previewLabel->move( 0,0 );
}

GifFramePreviewer::~GifFramePreviewer()
{
    delete ui;
}

void GifFramePreviewer::setImages( ImagesList pImages )
{
    mCurrFrameNum = 0;
    mImagesList = pImages;
    updateImage();
}

void GifFramePreviewer::on_btnPrev_clicked()
{
    if( mCurrFrameNum > 1 )
    {
        --mCurrFrameNum;
    }

    updateImage();
}

ImageWrapperPtr GifFramePreviewer::currentFrame() const
{
    return mCurrFrameNum >= 0 && mCurrFrameNum < mImagesList.size()
            ? mImagesList.at( mCurrFrameNum )
            : ImageWrapperPtr( 0 );
}

void GifFramePreviewer::on_btnRemove_clicked()
{
    ImageWrapperPtr pImg = currentFrame();
    if( pImg )
    {
        pImg->setUsed( !pImg->getUsed() );
        updateAddRmBtnText();
    }
}

void GifFramePreviewer::on_btnNext_clicked()
{
    const int nextNum = mCurrFrameNum + 1;
    if( nextNum < mImagesList.size() )
    {
        mCurrFrameNum = nextNum;
    }

    updateImage();
}

void GifFramePreviewer::updateImage()
{
    const ImageWrapperPtr pImg = currentFrame();
    if( pImg )
    {
        ui->previewLabel->setPixmap( pImg->img()->scaled( ui->previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        updateAddRmBtnText();
    }
}

void GifFramePreviewer::updateAddRmBtnText()
{
    const ImageWrapperPtr pImg = currentFrame();
    if( pImg )
    {
        ui->btnRemove->setText( pImg->getUsed()
                                ? tr( "X" )
                                : tr( "+" ) );
    }
}

void GifFramePreviewer::resizeEvent( QResizeEvent * event )
{
    QFrame::resizeEvent( event );

    ui->previewLabel->resize( ui->frame->size() );

    updateImage();
}
