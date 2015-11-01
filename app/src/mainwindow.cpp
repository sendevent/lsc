#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "capturingareaplugin.h"

#include <QTimer>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QClipboard>

#include <QPluginLoader>
#include <QDebug>

#ifdef Q_WS_X11
#include <QX11Info>
#endif //-- Q_OS_WIN

#include <QPair>
typedef QPair<int,int> PluginAreaNumsHolder;    // typedef for your type
Q_DECLARE_METATYPE(PluginAreaNumsHolder);       // makes your type available to QMetaType system




LSGMainWindow::LSGMainWindow(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::LSGMainWindow)
  ,mStartDelay( 0 )
  ,mPrevMode( 0 )
{
    ui->setupUi(this);

    ui->previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->previewLabel->setAlignment(Qt::AlignCenter);
    ui->previewLabel->setMinimumSize(240, 160);

    QMenu *saveMenu = new QMenu( this );
    QAction *pAct;

#ifdef WITH_ANIMATED_GIF
    pAct = saveMenu->addAction( tr( "Save GIF" ), this, SLOT(saveGif()) );
    pAct = saveMenu->addAction( tr( "Save customized GIF" ), this, SLOT(saveGifCustom()) );
    pAct->setEnabled( false );
#endif // WITH_ANIMATED_GIF

    pAct = saveMenu->addAction( tr( "Save separated PNGs" ), this, SLOT(saveSeria()) );
    pAct = saveMenu->addAction( tr( "Copy last to clipboard" ), this, SLOT(saveCopy()) );
    pAct = saveMenu->addAction( tr( "Send to" ), this, SLOT(saveSendTo()) );
    pAct->setEnabled( false );

    ui->saveButton->setMenu( saveMenu );

    pGrabber = new LSCCapturer( this );


    showSaveProgress( 0,0 );

    if( !loadPlugins() )
    {
        qWarning() << "Not found plugins! Aborting.";
        QTimer::singleShot( 10, qApp, SLOT(quit()) ) ;
    }

    pGrabber->setAreaSelector( plugins.at( 0 ) );
    mLastCapture = QPixmapPtr( new QPixmap() );

    connect( pGrabber, SIGNAL(finished()),
             this, SLOT(finishCapturing()) );
    connect( pGrabber, SIGNAL(savingProgress(int, int, QString)),
         this, SLOT(showSaveProgress( int, int, QString )));

    on_snapshotOneBtn_clicked();
}


void LSGMainWindow::showSaveProgress( int steps, int step, const QString& msg )
{

    ui->progressBar->setRange( 0, steps );
    ui->progressBar->reset();
    ui->progressBar->setValue( step );

    bool visible = steps || step;
    ui->progressBar->setVisible( visible );
    ui->progressLabel->setVisible( visible );
    ui->progressLabel->setText( visible
                                ? msg
                                : tr( "Saving" ) );


    ui->snapshotOneBtn->setEnabled( !visible );
    ui->startDelaySpinBox->setEnabled( !visible );
    ui->snapshotAllBtn->setEnabled( !visible );
    ui->modesComboBox->setEnabled( !visible );
    ui->fpsSpinBox->setEnabled( !visible );
    ui->durationCombo->setEnabled( !visible );
    ui->copyButton->setEnabled( !visible );
    ui->saveButton->setEnabled( !visible );
    qApp->processEvents();
}

LSGMainWindow::~LSGMainWindow()
{
    delete ui;
}


void LSGMainWindow::makeScreenshots( int fps, int duration )
{
    if( mLastCapture )
        mLastCapture.clear();

    pGrabber->setFps( fps );
    pGrabber->setDuration( duration );
    pGrabber->actualizeScreenArea();


    const int iDelay = mStartDelay*1000;

#ifdef Q_WS_X11
    const int iStartDelayDelta = QX11Info::isCompositingManagerRunning() ? 200 : 50;
#else
    const int iStartDelayDelta = 200;
#endif
//    QTimer::singleShot( iDelay, this, SLOT(hide()));
    hide();
    QTimer::singleShot( iDelay + iStartDelayDelta, this, SLOT(startCapturing()));
}


void LSGMainWindow::startCapturing()
{
    pGrabber->startGrab();
}

void LSGMainWindow::finishCapturing()
{
    const QPixmapPtr pImg = pGrabber->getCapture();
    if( pImg )
    {
        mLastCapture = pImg;
        updatePreview();
    }

    if( isHidden() )
        show();
}


void LSGMainWindow::on_snapshotOneBtn_clicked()
{
    makeScreenshots( 1, 1 );
}

void LSGMainWindow::on_snapshotAllBtn_clicked()
{
    makeScreenshots( ui->fpsSpinBox->value(), ui->durationCombo->value() );
}



void LSGMainWindow::updatePreview()
{
    if( mLastCapture && !mLastCapture.isNull() )
    {
        ui->previewLabel->setPixmap( mLastCapture->scaled( ui->previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    }

//    updateTotalFramesCountInfo();
}



void LSGMainWindow::resizeEvent ( QResizeEvent * event )
{
    QWidget::resizeEvent( event );
    updatePreview();
}


void LSGMainWindow::on_modesComboBox_activated( int index )
{
    PluginAreaNumsHolder plugAreaNum = ui->modesComboBox->itemData( index ).value<PluginAreaNumsHolder>();
    if( const LSGCapturingAreaPlugin *areaSelector = plugins.at( plugAreaNum.first ) )
    {
        if( !areaSelector->selectArea( plugAreaNum.second ).isEmpty() )
        {
            pGrabber->setAreaSelector( areaSelector, plugAreaNum.second );

            on_snapshotOneBtn_clicked();
        }
    }
}

void LSGMainWindow::updateTotalFramesCountInfo()
{
    const int imgCnt = ui->durationCombo->value() * ui->fpsSpinBox->value();
//    const QString& size( (!mLastCapture || mLastCapture.isNull()  )
//                ? "unknown"
//                : QString::number( mLastCapture->toImage().byteCount() * imgCnt ) );

//    ui->totalCountLabel->setText( QString( "%1 [%2]" ).arg(  imgCnt ).arg( size  )  );
    ui->totalCountLabel->setText( QString::number( imgCnt ) );
}

void LSGMainWindow::on_durationCombo_valueChanged( int )
{
    updateTotalFramesCountInfo();
}

void LSGMainWindow::on_fpsSpinBox_valueChanged( int )
{
    updateTotalFramesCountInfo();
}

void LSGMainWindow::on_startDelaySpinBox_valueChanged( int delay )
{
    mStartDelay = delay;
}

#ifdef WITH_ANIMATED_GIF
void LSGMainWindow::saveGif()
{
    pGrabber->saveGIF();
    showSaveProgress( 0, 0 );
}

void LSGMainWindow::saveGifCustom()
{
}
#endif // WITH_ANIMATED_GIF

void LSGMainWindow::saveSeria()
{
    const QString filePath = QFileDialog::getSaveFileName( this, tr( "Save Files in" ) );
    if( !filePath.isEmpty() )
    {
        pGrabber->saveSeparatedFiles( filePath );
    }
}

void LSGMainWindow::saveCopy()
{
    if( mLastCapture && !mLastCapture->isNull() )
    {
        QClipboard *cpb = QApplication::clipboard();
        cpb->setPixmap( *mLastCapture );
    }
}

void LSGMainWindow::saveSendTo()
{
    qDebug() << Q_FUNC_INFO << "not implemented yet!";
}


bool LSGMainWindow::loadPlugins()
{
    bool res = false;
    QDir pluginsDir( qApp->applicationDirPath() );
    pluginsDir.cd( "plugins" );

    Q_FOREACH( const QString& fileName, pluginsDir.entryList( QDir::Files ) )
        if ( const QObject *plugin = QPluginLoader( pluginsDir.absoluteFilePath( fileName ) ).instance() )
            res |= populateMenu( plugin );

    return res;
}


bool LSGMainWindow::populateMenu( const QObject *plugin )
{
    if ( LSGCapturingAreaPlugin *areaSelector = qobject_cast<LSGCapturingAreaPlugin *>(plugin) )
    {
        for( int i = 0; i < areaSelector->getAreasCount(); ++i )
        {
            const PluginAreaNumsHolder plugAreaNum = qMakePair<int,int>( plugins.size(), i );
             ui->modesComboBox->addItem( areaSelector->getAreaDescription( i ), QVariant::fromValue( plugAreaNum ) );
            plugins.append( areaSelector );
        }
        return true;
    }

    return false;
}
