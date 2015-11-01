#ifndef LSG_RECSELECTOR_H
#define LSG_RECSELECTOR_H

#include <QDialog>
#include <QRectF>

namespace Ui {
class LSGRecSelector;
}

class QPainter;
class LSGRecSelector : public QDialog
{
    Q_OBJECT

    enum MaskType { StrokeMask, FillMask };
    
public:
    LSGRecSelector();
    ~LSGRecSelector();

    QRect getSelectedRect() const;
    
public slots:
    int exec();
    
protected slots:
    void init();

//signals:
//    void regionGrabbed( const QPixmap & );
    

protected:
    void paintEvent( QPaintEvent* e );
    void resizeEvent( QResizeEvent* e );
    void mousePressEvent( QMouseEvent* e );
    void mouseMoveEvent( QMouseEvent* e );
    void mouseReleaseEvent( QMouseEvent* e );
    void mouseDoubleClickEvent( QMouseEvent* );
    void keyPressEvent( QKeyEvent* e );
//    void focusOutEvent(QFocusEvent * event);
    void updateHandles();
    QRegion handleMask( MaskType type ) const;
    QPoint limitPointToRect( const QPoint &p, const QRect &r ) const;
    QRect normalizeSelection( const QRect &s ) const;


    QPixmap shootDesktop() const;

    QRect selection;
    bool mouseDown;
    bool newSelection;
    const int handleSize;
    QRect* mouseOverHandle;
    QPoint dragStartPoint;
    QRect  selectionBeforeDrag;
    bool showHelp;

    // naming convention for handles
    // T top, B bottom, R Right, L left
    // 2 letters: a corner
    // 1 letter: the handle on the middle of the corresponding side
    QRect TLHandle, TRHandle, BLHandle, BRHandle;
    QRect LHandle, THandle, RHandle, BHandle;
    QRect helpTextRect;

    QVector<QRect*> handles;
    QPixmap pixmap;

    const bool compositingEnabled;
};

#endif // LSG_RECSELECTOR_H
