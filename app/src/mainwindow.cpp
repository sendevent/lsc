#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "capturingareaplugin.h"

#include <QTimer>
#include <QMap>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>
#include <QClipboard>
#include <QStatusBar>

#include <QPluginLoader>
#include <QDebug>

#ifdef Q_WS_X11
#include <QX11Info>
#endif //-- Q_OS_WIN

#include <QPair>
typedef QPair<int,int> PluginAreaNumsHolder;    // typedef for your type
Q_DECLARE_METATYPE(PluginAreaNumsHolder);       // makes your type available to QMetaType system




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  ,ui(new Ui::MainWindow)
  ,mStartDelay( 0 )
  ,mPrevMode( 0 )
  ,mLastCapture( 0 )
{
    ui->setupUi(this);

    ui->previewLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->previewLabel->setAlignment(Qt::AlignCenter);
    ui->previewLabel->setMinimumSize(240, 160);

    pProgress = new QProgressBar(0);
    statusBar()->addPermanentWidget( pProgress );

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

    connect( pGrabber, SIGNAL(finished()),
             this, SLOT(finishCapturing()) );
    connect( pGrabber, SIGNAL(savingProgress(int, int, QString)),
         this, SLOT(showSaveProgress( int, int, QString )));

    on_snapshotOneBtn_clicked();
}


void MainWindow::showSaveProgress( int steps, int step, const QString& msg )
{

    pProgress->setRange( 0, steps );
    pProgress->reset();
    pProgress->setValue( step );

    const bool visible = steps || step;
    pProgress->setVisible( visible );

    if( visible )
        statusBar()->showMessage( msg );

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

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeScreenshots( int fps, int duration )
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

    hide();

    QTimer::singleShot( iDelay + iStartDelayDelta, this, SLOT(startCapturing()));
}

void MainWindow::startCapturing()
{
    pGrabber->startGrab();
}

void MainWindow::finishCapturing()
{
    const ImageWrapperPtr pImg = pGrabber->getCapture();
    if( pImg )
    {
        mLastCapture = pImg;
        updatePreview();
    }

    if( isHidden() )
        show();
}

void MainWindow::on_snapshotOneBtn_clicked()
{
    makeScreenshots( 1, 1 );
}

void MainWindow::on_snapshotAllBtn_clicked()
{
    makeScreenshots( ui->fpsSpinBox->value(), ui->durationCombo->value() );
}

void MainWindow::updatePreview()
{
    if( mLastCapture && !mLastCapture.isNull() )
    {
        ui->previewLabel->setPixmap(
                    mLastCapture->img()->scaled( ui->previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
    }

    updateTotalFramesCountInfo();
}

void MainWindow::resizeEvent ( QResizeEvent * event )
{
    QMainWindow::resizeEvent( event );
    updatePreview();
}


void MainWindow::on_modesComboBox_activated( int index )
{
    PluginAreaNumsHolder plugAreaNum = ui->modesComboBox->itemData( index ).value<PluginAreaNumsHolder>();
    if( const LSGCapturingAreaPlugin *areaSelector = plugins.at( plugAreaNum.first ) )
    {
        hide(); // removes this from screenshot
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
        if( !areaSelector->selectArea( plugAreaNum.second ).isEmpty() )
        {
            pGrabber->setAreaSelector( areaSelector, plugAreaNum.second );
            on_snapshotOneBtn_clicked(); // updates screenshot and shows this
        }
        else
        {
            show();
        }
    }
}

QString bytesToString( const quint64& bytes )
{
    static const QString tmpl( "RAM: %1 %2" );
    if( bytes < 1024 )
        return tmpl.arg( bytes ).arg( " b" );

    const quint64 kbytes = bytes/1024;
    if( kbytes < 1024 )
        return tmpl.arg( kbytes ).arg( " KiB" );

    const quint64 mbytes = kbytes/1024;
//    if( mbytes < 1024 )
        return tmpl.arg( mbytes ).arg( " MiB" );

//    const quint64 gbytes = mbytes/1024;
//    return tmpl.arg( gbytes ).arg( " GiB" );
}

void MainWindow::updateTotalFramesCountInfo()
{
    const quint64 imgCnt = ui->durationCombo->value() * ui->fpsSpinBox->value();
    const quint64 bytesCnt = mLastCapture
            ? mLastCapture->img()->toImage().byteCount() * imgCnt
            : 0;

    ui->totalCountLabel->setText( QString( "%1 [%2]" ).arg(  imgCnt ).arg( ::bytesToString(  bytesCnt ) )  );
}

void MainWindow::on_durationCombo_valueChanged( int )
{
    updateTotalFramesCountInfo();
}

void MainWindow::on_fpsSpinBox_valueChanged( int )
{
    updateTotalFramesCountInfo();
}

void MainWindow::on_startDelaySpinBox_valueChanged( int delay )
{
    mStartDelay = delay;
}

#ifdef WITH_ANIMATED_GIF
void MainWindow::saveGif()
{
    pGrabber->saveGIF();
    showSaveProgress( 0, 0 );
}

void MainWindow::saveGifCustom()
{
}
#endif // WITH_ANIMATED_GIF

void MainWindow::saveSeria()
{
    const QString filePath = QFileDialog::getSaveFileName( this, tr( "Save Files in" ) );
    if( !filePath.isEmpty() )
    {
        pGrabber->saveSeparatedFiles( filePath );
        showSaveProgress( 0, 0 );
    }
}

void MainWindow::saveCopy()
{
    if( mLastCapture && !mLastCapture->img()->isNull() )
    {
        QClipboard *cpb = QApplication::clipboard();
        cpb->setPixmap( *mLastCapture->img() );
    }
}

void MainWindow::saveSendTo()
{
    qDebug() << Q_FUNC_INFO << "not implemented yet!";
}


bool MainWindow::loadPlugins()
{
    bool res = false;
    QDir pluginsDir( qApp->applicationDirPath() );
    pluginsDir.cd( "plugins" );

    Q_FOREACH( const QString& fileName, pluginsDir.entryList( QDir::Files ) )
        if ( const QObject *plugin = QPluginLoader( pluginsDir.absoluteFilePath( fileName ) ).instance() )
            res |= populateMenu( plugin );

    return res;
}


bool MainWindow::populateMenu( const QObject *plugin )
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

