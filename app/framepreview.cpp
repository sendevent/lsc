#include "framepreview.h"
#include "ui_framepreview.h"

#include <QMouseEvent>
#include <QDebug>

#define LOG qDebug()

FramePreview::FramePreview(ImageWrapperPtr img, int num, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FramePreview)
    , mImg( img )
    , mNum( num )
{
    ui->setupUi(this);

    mCheckBox = new QCheckBox( ui->label );
    mCheckBox->setChecked( mImg->getUsed() );
    mCheckBox->setText( tr( "Save" ) );

    ui->label->setPixmap( mImg->img()->scaled( QSize(120, 120 ), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}

FramePreview::~FramePreview()
{
    delete ui;
}


void FramePreview::mouseReleaseEvent(QMouseEvent *e)
{
    const QPoint pnt = geometry().center();
    LOG << pnt << mapToGlobal( pnt );
    emit clicked( mapToGlobal( pnt ), mNum );
}
