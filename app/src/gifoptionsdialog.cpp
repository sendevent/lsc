#include "gifoptionsdialog.h"
#include "ui_gifoptionsdialog.h"

#include <QFileDialog>

#include <Magick++.h>

GifOptionsDialog::GifOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GifOptionsDialog)
{
    ui->setupUi(this);

    ui->compressCombo->addItem( "No", MagickCore::NoCompression );
    ui->compressCombo->addItem( "BZip", MagickCore::BZipCompression ) ;
    ui->compressCombo->addItem( "DXT1", MagickCore::DXT1Compression ) ;
    ui->compressCombo->addItem( "DXT3", MagickCore::DXT3Compression ) ;
    ui->compressCombo->addItem( "DXT5", MagickCore::DXT5Compression ) ;
    ui->compressCombo->addItem( "Fax", MagickCore::FaxCompression ) ;
    ui->compressCombo->addItem( "Group4", MagickCore::Group4Compression ) ;
    ui->compressCombo->addItem( "JPEG", MagickCore::JPEGCompression ) ;
    ui->compressCombo->addItem( "JPEG2000", MagickCore::JPEG2000Compression ) ;
    ui->compressCombo->addItem( "LosslessJPEG", MagickCore::LosslessJPEGCompression ) ;
    ui->compressCombo->addItem( "LZW", MagickCore::LZWCompression ) ;
    ui->compressCombo->addItem( "RLE", MagickCore::RLECompression ) ;
    ui->compressCombo->addItem( "Zip", MagickCore::ZipCompression ) ;
    ui->compressCombo->addItem( "ZipS", MagickCore::ZipSCompression ) ;
    ui->compressCombo->addItem( "Piz", MagickCore::PizCompression ) ;
    ui->compressCombo->addItem( "Pxr24", MagickCore::Pxr24Compression ) ;
    ui->compressCombo->addItem( "B44", MagickCore::B44Compression ) ;
    ui->compressCombo->addItem( "B44A", MagickCore::B44ACompression ) ;
    ui->compressCombo->addItem( "LZMA", MagickCore::LZMACompression ) ;
    ui->compressCombo->addItem( "JBIG1", MagickCore::JBIG1Compression ) ;
    ui->compressCombo->addItem( "JBIG2", MagickCore::JBIG2Compression ) ;
}

GifOptionsDialog::~GifOptionsDialog()
{
    delete ui;
}

void GifOptionsDialog::on_pathButton_clicked()
{
    const QString path = QFileDialog::getSaveFileName( this, tr( "Save as..." ) );
    ui->lineEdit->setText( path.isEmpty() ? QString::null : path );
}

int GifOptionsDialog::getQuality() const
{
    return ui->spinBox->value();
}
QString GifOptionsDialog::getPath() const
{
    return ui->lineEdit->text();
}
unsigned int GifOptionsDialog::getCompression() const
{
    bool ok = false;
    const QVariant varDat =
#if QT_VERSION >= 0x050000
            ui->compressCombo->currentData();
#else
            ui->compressCombo->itemData( ui->compressCombo->currentIndex() );
#endif //QT_VERSION >= 0x050000

    const Magick::CompressionType ct = (Magick::CompressionType)varDat.toUInt( &ok );

    return ok ? ct : Magick::NoCompression;
}

void GifOptionsDialog::setImagesList( ImagesList pImages )
{
    ui->previewFrame->setImages( pImages );
}
