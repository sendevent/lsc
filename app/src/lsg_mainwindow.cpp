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



LSGMainWindow::LSGMainWindow(QWidget *parent) :
    QWidget(parent)
  ,ui(new Ui::LSGMainWindow)
  ,mStartDelay( 0 )
  ,mPrevMode( 0 )
{
    qDebug() << Q_FUNC_INFO;
    
    ui->setupUi(this);
    
    /*QMap<quint8,QString> modes = LSGCapturer::getModesNames();
    
    
    
    qDebug() <<modes;
    Q_FOREACH( quint8 id, modes.keys() )
    {
        qDebug() << id;
        ui->modesComboBox->addItem( modes.value( id ), id );
    }*/
    
    ui->previewLabel->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Expanding);
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
    connect( pGrabber, SIGNAL(savingProgress(int, int)),
             this, SLOT(showSaveProgress( int, int )));
    
    if( !loadPlugins() )
    {
        qWarning() << "Not found plugins! Aborting.";
        QTimer::singleShot( 10, qApp, SLOT(quit()) ) ;
    }
    
    pGrabber->setAreaSelector( plugins.at( 0 ) );
    
    pGrabber->setMaxCapturesLimit( 1 );
    pGrabber->startGrab();
    updatePreview();
    
}

void LSGMainWindow::showSaveProgress( int steps, int step )
{
    
    ui->progressBar->setRange( 0, steps );
    ui->progressBar->reset();
    ui->progressBar->setValue( step );
    
    bool visible = steps || step;
    qDebug() << Q_FUNC_INFO << steps << step << visible;
    ui->progressBar->setVisible( visible );
    ui->progressLabel->setVisible( visible );
    
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
    qDebug() << Q_FUNC_INFO;
    delete ui;
}


void LSGMainWindow::slotCaptureChanged()
{
    qDebug() << Q_FUNC_INFO;
    
    QPixmapPtr pImg = pGrabber->getCapture();
    
    
    if( !pImg )
    {
        qWarning() << Q_FUNC_INFO << "invalid img!";
    }
    else
    {
        mLastCapture = *pImg;
        updatePreview();
    }
}

void LSGMainWindow::on_startButton_clicked()
{
    mLastCapture = QPixmap();
    
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


void LSGMainWindow::on_grabSerialButton_clicked()
{
    int iMaxLimit = ui->capLimitCombo->value();
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
    ui->previewLabel->setPixmap( mLastCapture.scaled( ui->previewLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation ) );
}


void LSGMainWindow::resizeEvent ( QResizeEvent * event ) 
{
    QWidget::resizeEvent( event );
    updatePreview();
}


void LSGMainWindow::on_modesComboBox_activated( int index )
{
    qDebug() << Q_FUNC_INFO;
    LSGCapturingAreaPlugin *areaSelector = plugins.at( index );
    if( areaSelector )
    {
        QPainterPath path = areaSelector->selectArea();
        if( !path.isEmpty() )
        {
            pGrabber->setAreaSelector( areaSelector );

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
    //QString filePath = QFileDialog::getSaveFileName( this, tr( "Save Files in" ) );
    QString filePath( "/home/denis/data/develop/LSC/bin_debug/123.gif" );
    if( !filePath.isEmpty() )
    {
        showSaveProgress( ui->capLimitCombo->value()+1, 0 );
        pGrabber->saveGIF( filePath );
        
        showSaveProgress( 0, 0 );
    }
    
}

void LSGMainWindow::saveGifCustom()
{
    qDebug() << Q_FUNC_INFO << "not implemented yet!";
}

void LSGMainWindow::saveSeria()
{
    QString filePath = QFileDialog::getSaveFileName( this, tr( "Save Files in" ) );
    if( !filePath.isEmpty() )
    {
        pGrabber->saveSeparatedFiles( filePath );
    }
}

void LSGMainWindow::saveCopy()
{
    if( !mLastCapture.isNull() )
    {
        QClipboard *cpb = QApplication::clipboard();
        cpb->setPixmap( mLastCapture );
    }
}

void LSGMainWindow::saveSendTo()
{
    qDebug() << Q_FUNC_INFO << "not implemented yet!";
}


bool LSGMainWindow::loadPlugins()
{
    
    bool res = false;
    QObjectList plugs = QPluginLoader::staticInstances();
    qDebug() << Q_FUNC_INFO << plugs.count();
    foreach (QObject *plugin, plugs)
    {
        qDebug() << "FINDED static plugin";
        
        if( !res )
            res = populateMenu( plugin );
        else
            populateMenu( plugin );
    }
    
    qDebug() << "NOT FINDED static plugin";

    QDir pluginsDir = QDir(qApp->applicationDirPath());

//#if defined(Q_OS_WIN)
//    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
//        pluginsDir.cdUp();
//#elif defined(Q_OS_MAC)
//    if (pluginsDir.dirName() == "MacOS") {
//        pluginsDir.cdUp();
//        pluginsDir.cdUp();
//        pluginsDir.cdUp();
//    }
//#endif
    pluginsDir.cd( "plugins" );
    foreach( QString fileName, pluginsDir.entryList( QDir::Files ) ) 
    {
        QPluginLoader loader( pluginsDir.absoluteFilePath( fileName ) );
        QObject *plugin = loader.instance();
        if (plugin) 
        {
            if( !res )
                res = populateMenu( plugin );
            else
                populateMenu( plugin );
            //pluginFileNames += fileName;
            qDebug() << "FINDED dynamic plugin";
        }
        else
        qDebug() << "NOT FINDED dynamic  plugin";
    }
    
    return res;
}


bool LSGMainWindow::populateMenu(QObject *plugin)
{
    qDebug() << Q_FUNC_INFO;
    LSGCapturingAreaPlugin *areaSelector = qobject_cast<LSGCapturingAreaPlugin *>(plugin);
    if ( areaSelector )
    {
        ui->modesComboBox->addItem( areaSelector->getAreaDescription(), QVariant::fromValue( plugins.size() ) );
        plugins.append( areaSelector );
        
        qDebug() << Q_FUNC_INFO << plugins.size();
        
        return true;
    }
    
    
    return false;
}
