#ifndef LSG_MAINWINDOW_H
#define LSG_MAINWINDOW_H

#include <QWidget>
#include <QPixmap>
#include "lsg_capturer.h"

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
    void slotCaptureChanged();
    void updatePreview();
    
    void on_startButton_clicked();
    void on_grabSerialButton_clicked();
    void on_modesComboBox_activated( int index );
    void on_capLimitCombo_valueChanged( int count );
    void on_capDelaySpinBox_valueChanged( int delay );
    void on_startDelaySpinBox_valueChanged( int delay );
    
    void saveGif();
    void saveGifCustom();
    void saveSeria();
    void saveCopy();
    void saveSendTo();
    
    void showSaveProgress( int steps, int step, const QString& msg = QString() );
    
protected:
    void resizeEvent ( QResizeEvent * event );
    void showEvent( QShowEvent* e);

private:
    Ui::LSGMainWindow *ui;
    int mStartDelay;
    int mPrevMode;
    LSGCapturer *pGrabber;
    
    QPixmapPtr mLastCapture;
    
    bool loadPlugins();
    bool populateMenu( const QObject *plugin );
    
    QList<LSGCapturingAreaPlugin*> plugins;

    int getFramesCount() const;
};

#endif // LSG_MAINWINDOW_H
