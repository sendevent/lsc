#ifndef LSG_MAINWINDOW_H
#define LSG_MAINWINDOW_H

#include <QWidget>
#include <QPixmap>
#include "capturer.h"

namespace Ui {
class LSGMainWindow;
}

class LSGCapturingAreaPlugin;
class LSGMainWindow : public QWidget
{
    Q_OBJECT
    
public:
    explicit LSGMainWindow(QWidget *parent = 0);
    ~LSGMainWindow();

protected slots:

    void updatePreview();
    
    void on_snapshotOneBtn_clicked();
    void on_snapshotAllBtn_clicked();
    void on_modesComboBox_activated( int index );
    void on_durationCombo_valueChanged( int count );
    void on_fpsSpinBox_valueChanged( int delay );
    void on_startDelaySpinBox_valueChanged( int delay );

    void startCapturing();
    void finishCapturing();
    
#ifdef WITH_ANIMATED_GIF
    void saveGif();
    void saveGifCustom();
#endif //WITH_ANIMATED_GIF
    void saveSeria();
    void saveCopy();
    void saveSendTo();
    
    void showSaveProgress( int steps, int step, const QString& msg = QString() );
    
protected:
    void resizeEvent ( QResizeEvent * event );

    void makeScreenshots( int fps, int duration );

private:
    Ui::LSGMainWindow *ui;
    int mStartDelay;
    int mPrevMode;
    LSCCapturer *pGrabber;
    
    QPixmapPtr mLastCapture;
    
    bool loadPlugins();
    bool populateMenu( const QObject *plugin );
    
    QList<LSGCapturingAreaPlugin*> plugins;

    void updateTotalFramesCountInfo();
};

#endif // LSG_MAINWINDOW_H
