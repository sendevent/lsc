#include "lsg_mainwindow.h"
#include "ui_lsg_mainwindow.h"

#include "lsg_capturingareaplugin.h"

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
    QAction *pAct = saveMenu->addAction( tr( "Save GIF" ), this, SLOT(saveGif()) );
    pAct = saveMenu->addAction( tr( "Save customized GIF" ), this, SLOT(saveGifCustom()) );
    pAct->setEnabled( false );
    pAct = saveMenu->addAction( tr( "Save separated PNGs" ), this, SLOT(saveSeria()) );
    pAct = saveMenu->addAction( tr( "Copy current to buffer" ), this, SLOT(saveCopy()) );
    pAct = saveMenu->addAction( tr( "Send to" ), this, SLOT(saveSendTo()) );
    pAct->setEnabled( false );
    
    ui->saveButton->setMenu( saveMenu );
    
    pGrabber = new LSGCapturer( this );
    
    connect( pGrabber, SIGNAL(captured(int)),
             this, SLOT(slotCaptureChanged()) );
    connect( pGrabber, SIGNAL(finished()),
             this, SLOT(show()) );
    
    showSaveProgress( 0,0 );
    connect( pGrabber, SIGNAL(savingProgress(int, int, QString)),
             this, SLOT(showSaveProgress( int, int, QString )));
    
    if( !loadPlugins() )
    {
        qWarning() << "Not found plugins! Aborting.";
        QTimer::singleShot( 10, qApp, SLOT(quit()) ) ;
    }
    
    pGrabber->setAreaSelector( plugins.at( 0 ) );
    
    pGrabber->setMaxCapturesLimit( 1 );
    on_startButton_clicked();
//    updatePreview();
    
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

    
    ui->startButton->setEnabled( !visible );
    ui->startDelaySpinBox->setEnabled( !visible );
    ui->grabSerialButton->setEnabled( !visible );
    ui->modesComboBox->setEnabled( !visible );
    ui->capDelaySpinBox->setEnabled( !visible );
    ui->capLimitCombo->setEnabled( !visible );
    ui->copyButton->setEnabled( !visible );
    ui->saveButton->setEnabled( !visible );
    qApp->processEvents();
}

LSGMainWindow::~LSGMainWindow()
{
    delete ui;
}


void LSGMainWindow::slotCaptureChanged()
{
    QPixmapPtr pImg = pGrabber->getCapture();
    
    
    if( !pImg )
    {
        qWarning() << Q_FUNC_INFO << "invalid img!";
    }
    else
    {
        mLastCapture = pImg;
        updatePreview();
    }
}

void LSGMainWindow::on_startButton_clicked()
{
    if( mLastCapture )
        mLastCapture.clear();

    mLastCapture = QPixmapPtr( new QPixmap() );
    
    pGrabber->setMaxCapturesLimit( 1 );
    
    const int iDelay = mStartDelay*1000;
    
#ifdef Q_WS_X11
    const int iStartDelayDelta = QX11Info::isCompositingManagerRunning() ? 200 : 50;
#else
    const int iStartDelayDelta = 200;
#endif
    QTimer::singleShot( iDelay, this, SLOT(hide()));
    QTimer::singleShot( iDelay + iStartDelayDelta, pGrabber, SLOT(startGrab()));
}


int LSGMainWindow::getFramesCount() const
{
    return ui->capLimitCombo->value()*ui->capDelaySpinBox->value();
}

void LSGMainWindow::on_grabSerialButton_clicked()
{
    const int iMaxLimit = getFramesCount();
    if( !iMaxLimit )
    {
        QMessageBox::warning( this, tr( "Warning!" ), tr( "Max count not valid for serial grabbing!" ) );
        return;
    }
    
    pGrabber->setMaxCapturesLimit( iMaxLimit );
    pGrabber->setCapturingDelay( ui->capDelaySpinBox->value() );
    
    const int iDelay = mStartDelay*1000;
#ifdef Q_WS_X11
    const int iStartDelayDelta = QX11Info::isCompositingManagerRunning() ? 200 : 50;
#else
    const int iStartDelayDelta = 200;
#endif
    QTimer::singleShot( iDelay, this, SLOT(hide()));
    QTimer::singleShot( iDelay + iStartDelayDelta, pGrabber, SLOT(startGrab()));
    
}

void LSGMainWindow::updatePreview()
{
    if( isVisible() && mLastCapture )
        ui->previewLabel->setPixmap( mLastCapture->scaled( ui->previewLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
}


void LSGMainWindow::showEvent( QShowEvent* e)
{
    QWidget::showEvent( e );
    if( isVisible() )
        updatePreview();
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

            on_startButton_clicked();
        }
    }
}

void LSGMainWindow::on_capLimitCombo_valueChanged( int count )
{
    pGrabber->setMaxCapturesLimit( count );
}

void LSGMainWindow::on_capDelaySpinBox_valueChanged( int delay )
{
    pGrabber->setCapturingDelay( delay );
}

void LSGMainWindow::on_startDelaySpinBox_valueChanged( int delay )
{
    mStartDelay = delay;
}

void LSGMainWindow::saveGif()
{
    pGrabber->saveGIF();
    showSaveProgress( 0, 0 );
}

void LSGMainWindow::saveGifCustom()
{
}

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
