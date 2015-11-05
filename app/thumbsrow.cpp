#include "thumbsrow.h"
#include "ui_thumbsrow.h"
#include "framepreview.h"

#include <QDebug>

#define LOG qDebug()
ThumbsRow::ThumbsRow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ThumbsRow)
{
    ui->setupUi(this);
}

ThumbsRow::~ThumbsRow()
{
    delete ui;
}

void ThumbsRow::addThumb( FramePreview *pThumb )
{
    ui->horizontalLayout->addWidget( pThumb );

    LOG << pThumb->width();
}

void ThumbsRow::resizeEvent( QResizeEvent*e)
{
    LOG << width();
}
