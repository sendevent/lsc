/*
 *   Based on KSnapShot's RegionGrabber:
 *
 *   Copyright (C) 2007 Luca Gugelmann <lucag@student.ethz.ch>
 *
 *   This program is free software; you can redistribute it and/or modify it
 *   under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "rectselector.h"
#include "ui_rectselector.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QToolTip>
#include <QTimer>
#include <QDebug>

#if QT_VERSION >= 0x050000
#include <QScreen>
#endif

LSGRecSelector::LSGRecSelector( ) :
    QDialog( 0, Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool )
  ,selection()
  ,mouseDown( false )
  ,newSelection( false )
  ,handleSize( 10 )
  ,mouseOverHandle( 0 )
  ,showHelp( true )
  ,TLHandle(0,0,handleSize,handleSize)
  ,TRHandle(0,0,handleSize,handleSize)
  ,BLHandle(0,0,handleSize,handleSize)
  ,BRHandle(0,0,handleSize,handleSize)
  ,LHandle(0,0,handleSize,handleSize)
  ,THandle(0,0,handleSize,handleSize)
  ,RHandle(0,0,handleSize,handleSize)
  ,BHandle(0,0,handleSize,handleSize)
  ,compositingEnabled( !qApp->arguments().contains( "--no-compositing" ) )
{
    handles << &TLHandle << &TRHandle << &BLHandle << &BRHandle
            << &LHandle << &THandle << &RHandle << &BHandle;
    setMouseTracking( true );

    setAttribute( Qt::WA_TranslucentBackground, true );
    setAttribute( Qt::WA_NoSystemBackground, true );
}

LSGRecSelector::~LSGRecSelector()
{
}

int LSGRecSelector::exec()
{
    init();
    activateWindow();
    return QDialog::exec();
}


QPixmap LSGRecSelector::shootDesktop() const
{

    QRect r;
    int screensCnt = 0;
#if QT_VERSION < 0x050000
    QDesktopWidget *pDesktop = qApp->desktop();
    screensCnt = pDesktop->screenCount();
#else
    const QList<QScreen *> screens = QGuiApplication::screens();
    screensCnt = screens.size();
#endif // QT_VERSION < 0x050000

    for( int i = 0; i < screensCnt; ++i )
    {
        const QRect currR =

#if QT_VERSION < 0x050000
                pDesktop->screen( i )->geometry();
#else
                screens.at( i )->geometry();
#endif // QT_VERSION < 0x050000

        QPainterPath currPath;
        currPath.addRect( currR );
        r = r.united( currR );
    }

    return
#if QT_VERSION >= 0x050000
        QGuiApplication::primaryScreen()->grabWindow(
#else
        QPixmap::grabWindow(
#endif
                                  QApplication::desktop()->winId(),
                                  r.x(),
                                  r.y(),
                                  r.width(),
                                  r.height() );
}

void LSGRecSelector::init()
{
    pixmap = shootDesktop();

    pixmap.save( "./t.png", "PNG" );
    resize( pixmap.size() );
    move( 0, 0 );
    setCursor( Qt::CrossCursor );
    show();
}

static void drawRect( QPainter *painter, const QRect &r, const QColor &outline, const QColor &fill = QColor() )
{
    const QRegion clip = QRegion( r ).subtracted( r.adjusted( 1, 1, -1, -1 ) );

    painter->save();
    painter->setClipRegion( clip );
    painter->setPen( Qt::NoPen );
    painter->setBrush( outline );
    painter->drawRect( r );
    if ( fill.isValid() )
    {
        painter->setClipping( false );
        painter->setBrush( fill );
        painter->drawRect( r.adjusted( 1, 1, -1, -1 ) );
    }
    painter->restore();

#ifdef Q_OS_WIN
    // On Win7 selected area transparent for mouse events,
    // fill it with almost transparent color to prevent
    // losing focus on mouse moves/clicks
    painter->save();
    if( !r.isEmpty() && !clip.isEmpty() && !fill.isValid() )
    {
        painter->setBrush( QColor::fromRgb( fill.red(), fill.green(), fill.blue(), 1 ) );
        painter->drawRect( r );
    }
    painter->restore();
#endif // Q_OS_WIN
}

void LSGRecSelector::paintEvent( QPaintEvent* e )
{
    Q_UNUSED( e );

    QPainter painter( this );

    const QPalette pal( QToolTip::palette() );
    const QFont font = QToolTip::font();

    QColor handleColor = pal.color( QPalette::Active, QPalette::Highlight );
    handleColor.setAlpha( 160 );
    const QColor overlayColor( 0, 0, 0, 160 );
    const QColor textColor = pal.color( QPalette::Active, QPalette::Text );
    const QColor textBackgroundColor = pal.color( QPalette::Active, QPalette::Base );

    if( !compositingEnabled )
        painter.drawPixmap(0, 0, pixmap);

    painter.setFont(font);

    const QRegion grey = QRegion( rect() ).subtracted( selection );
    painter.setClipRegion( grey );
    painter.setPen( Qt::NoPen );
    painter.setBrush( overlayColor );
    painter.drawRect( rect() );
    painter.setClipRect( rect() );
    drawRect( &painter, selection, handleColor );

    if ( showHelp )
    {
        painter.setPen( textColor );
        painter.setBrush( textBackgroundColor );
        static const QString helpText = tr( "Select a region using the mouse. To take the snapshot, press the Enter key or double click. Press Esc to quit." );
        helpTextRect = painter.boundingRect( rect().adjusted( 2, 2, -2, -2 ), Qt::TextWordWrap, helpText );
        helpTextRect.adjust( -2, -2, 4, 2 );
        drawRect( &painter, helpTextRect, textColor, textBackgroundColor );
        painter.drawText( helpTextRect.adjusted( 3, 3, -3, -3 ), helpText );
    }

    if ( selection.isNull() )
    {
        return;
    }

    // The grabbed region is everything which is covered by the drawn
    // rectangles (border included). This means that there is no 0px
    // selection, since a 0px wide rectangle will always be drawn as a line.
    const QString txt = QString( "[%1;%2], %3x%4" )
            .arg( selection.x() )
            .arg( selection.y() )
            .arg( selection.width() )
            .arg( selection.height() );

    QRect textRect = painter.boundingRect( rect(), Qt::AlignLeft, txt );
    QRect boundingRect = textRect.adjusted( -4, 0, 0, 0);

    if ( textRect.width() < selection.width() - 2*handleSize &&
         textRect.height() < selection.height() - 2*handleSize &&
         ( selection.width() > 100 && selection.height() > 100 ) ) // center, unsuitable for small selections
    {
        boundingRect.moveCenter( selection.center() );
        textRect.moveCenter( selection.center() );
    }
    else if ( selection.y() - 3 > textRect.height() &&
              selection.x() + textRect.width() < rect().right() ) // on top, left aligned
    {
        boundingRect.moveBottomLeft( QPoint( selection.x(), selection.y() - 3 ) );
        textRect.moveBottomLeft( QPoint( selection.x() + 2, selection.y() - 3 ) );
    }
    else if ( selection.x() - 3 > textRect.width() ) // left, top aligned
    {
        boundingRect.moveTopRight( QPoint( selection.x() - 3, selection.y() ) );
        textRect.moveTopRight( QPoint( selection.x() - 5, selection.y() ) );
    }
    else if ( selection.bottom() + 3 + textRect.height() < rect().bottom() &&
              selection.right() > textRect.width() ) // at bottom, right aligned
    {
        boundingRect.moveTopRight( QPoint( selection.right(), selection.bottom() + 3 ) );
        textRect.moveTopRight( QPoint( selection.right() - 2, selection.bottom() + 3 ) );
    }
    else if ( selection.right() + textRect.width() + 3 < rect().width() ) // right, bottom aligned
    {
        boundingRect.moveBottomLeft( QPoint( selection.right() + 3, selection.bottom() ) );
        textRect.moveBottomLeft( QPoint( selection.right() + 5, selection.bottom() ) );
    }
    // if the above didn't catch it, you are running on a very tiny screen...
    drawRect( &painter, boundingRect, textColor, textBackgroundColor );

    painter.drawText( textRect, txt );

    if ( ( selection.height() > handleSize*2 && selection.width() > handleSize*2 )
         || !mouseDown )
    {
        updateHandles();
        painter.setPen( Qt::NoPen );
        painter.setBrush( handleColor );
        painter.setClipRegion( handleMask( StrokeMask ) );
        painter.drawRect( rect() );
        handleColor.setAlpha( 60 );
        painter.setBrush( handleColor );
        painter.setClipRegion( handleMask( FillMask ) );
        painter.drawRect( rect() );
    }
}

void LSGRecSelector::resizeEvent( QResizeEvent* e )
{
    Q_UNUSED( e );
    if ( selection.isNull() )
        return;
    QRect r = selection;
    r.setTopLeft( limitPointToRect( r.topLeft(), rect() ) );
    r.setBottomRight( limitPointToRect( r.bottomRight(), rect() ) );
    if ( r.width() <= 1 || r.height() <= 1 ) //this just results in ugly drawing...
        selection = QRect();
    else
        selection = normalizeSelection(r);
}

void LSGRecSelector::mousePressEvent( QMouseEvent* e )
{
    showHelp = !helpTextRect.contains( e->pos() );
    if ( e->button() == Qt::LeftButton )
    {
        mouseDown = true;
        dragStartPoint = e->pos();
        selectionBeforeDrag = selection;
        if ( !selection.contains( e->pos() ) )
        {
            newSelection = true;
            selection = QRect();
        }
        else
        {
            setCursor( Qt::ClosedHandCursor );
        }
    }
    else if ( e->button() == Qt::RightButton )
    {
        newSelection = false;
        selection = QRect();
        setCursor( Qt::CrossCursor );
    }
    update();
}

//void LSGRecSelector::focusOutEvent(QFocusEvent * event)
//{
//    qApp->setActiveWindow( this );
//    activateWindow();
//}

void LSGRecSelector::mouseMoveEvent( QMouseEvent* e )
{
    bool shouldShowHelp = !helpTextRect.contains( e->pos() );
    if (shouldShowHelp != showHelp) {
        showHelp = shouldShowHelp;
        update();
    }

    if ( mouseDown )
    {
        if ( newSelection )
        {
            QPoint p = e->pos();
            QRect r = rect();
            selection = normalizeSelection(QRect( dragStartPoint, limitPointToRect( p, r ) ));
        }
        else if ( mouseOverHandle == 0 ) // moving the whole selection
        {
            QRect r = rect().normalized(), s = selectionBeforeDrag.normalized();
            QPoint p = s.topLeft() + e->pos() - dragStartPoint;
            r.setBottomRight( r.bottomRight() - QPoint( s.width(), s.height() ) + QPoint( 1, 1 ) );
            if ( !r.isNull() && r.isValid() )
                selection.moveTo( limitPointToRect( p, r ) );
        }
        else // dragging a handle
        {
            QRect r = selectionBeforeDrag;
            QPoint offset = e->pos() - dragStartPoint;

            if ( mouseOverHandle == &TLHandle || mouseOverHandle == &THandle
                 || mouseOverHandle == &TRHandle ) // dragging one of the top handles
            {
                r.setTop( r.top() + offset.y() );
            }

            if ( mouseOverHandle == &TLHandle || mouseOverHandle == &LHandle
                 || mouseOverHandle == &BLHandle ) // dragging one of the left handles
            {
                r.setLeft( r.left() + offset.x() );
            }

            if ( mouseOverHandle == &BLHandle || mouseOverHandle == &BHandle
                 || mouseOverHandle == &BRHandle ) // dragging one of the bottom handles
            {
                r.setBottom( r.bottom() + offset.y() );
            }

            if ( mouseOverHandle == &TRHandle || mouseOverHandle == &RHandle
                 || mouseOverHandle == &BRHandle ) // dragging one of the right handles
            {
                r.setRight( r.right() + offset.x() );
            }
            r.setTopLeft( limitPointToRect( r.topLeft(), rect() ) );
            r.setBottomRight( limitPointToRect( r.bottomRight(), rect() ) );
            selection = normalizeSelection(r);
        }
        update();
    }
    else
    {
        if ( selection.isNull() )
            return;
        bool found = false;
        foreach( QRect* r, handles )
        {
            if ( r->contains( e->pos() ) )
            {
                mouseOverHandle = r;
                found = true;
                break;
            }
        }
        if ( !found )
        {
            mouseOverHandle = 0;
            if ( selection.contains( e->pos() ) )
                setCursor( Qt::OpenHandCursor );
            else
                setCursor( Qt::CrossCursor );
        }
        else
        {
            if ( mouseOverHandle == &TLHandle || mouseOverHandle == &BRHandle )
                setCursor( Qt::SizeFDiagCursor );
            if ( mouseOverHandle == &TRHandle || mouseOverHandle == &BLHandle )
                setCursor( Qt::SizeBDiagCursor );
            if ( mouseOverHandle == &LHandle || mouseOverHandle == &RHandle )
                setCursor( Qt::SizeHorCursor );
            if ( mouseOverHandle == &THandle || mouseOverHandle == &BHandle )
                setCursor( Qt::SizeVerCursor );
        }
    }
}

void LSGRecSelector::mouseReleaseEvent( QMouseEvent* e )
{
    mouseDown = false;
    newSelection = false;
    if ( mouseOverHandle == 0 && selection.contains( e->pos() ) )
        setCursor( Qt::OpenHandCursor );
    update();
}

void LSGRecSelector::mouseDoubleClickEvent( QMouseEvent* )
{
    if( selection.isValid() )
        QDialog::accept();
}

void LSGRecSelector::keyPressEvent( QKeyEvent* e )
{
    if ( e->key() == Qt::Key_Escape )
    {
        return QDialog::reject();
    }
    else if ( e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return )
    {
        return QDialog::accept();
    }
    
    e->ignore();
}

void LSGRecSelector::updateHandles()
{
    QRect r = selection;
    int s2 = handleSize / 2;

    TLHandle.moveTopLeft( r.topLeft() );
    TRHandle.moveTopRight( r.topRight() );
    BLHandle.moveBottomLeft( r.bottomLeft() );
    BRHandle.moveBottomRight( r.bottomRight() );

    LHandle.moveTopLeft( QPoint( r.x(), r.y() + r.height() / 2 - s2) );
    THandle.moveTopLeft( QPoint( r.x() + r.width() / 2 - s2, r.y() ) );
    RHandle.moveTopRight( QPoint( r.right(), r.y() + r.height() / 2 - s2 ) );
    BHandle.moveBottomLeft( QPoint( r.x() + r.width() / 2 - s2, r.bottom() ) );
}

QRegion LSGRecSelector::handleMask( MaskType type ) const
{
    // note: not normalized QRects are bad here, since they will not be drawn
    QRegion mask;
    foreach( QRect* rect, handles ) {
        if ( type == StrokeMask ) {
            QRegion r( *rect );
            mask += r.subtracted( rect->adjusted( 1, 1, -1, -1 ) );
        } else {
            mask += QRegion( rect->adjusted( 1, 1, -1, -1 ) );
        }
    }
    return mask;
}

QPoint LSGRecSelector::limitPointToRect( const QPoint &p, const QRect &r ) const
{
    QPoint q;
    q.setX( p.x() < r.x() ? r.x() : p.x() < r.right() ? p.x() : r.right() );
    q.setY( p.y() < r.y() ? r.y() : p.y() < r.bottom() ? p.y() : r.bottom() );
    return q;
}

QRect LSGRecSelector::normalizeSelection( const QRect &s ) const
{
    QRect r = s;
    if (r.width() <= 0) {
        int l = r.left();
        int w = r.width();
        r.setLeft(l + w - 1);
        r.setRight(l);
    }
    if (r.height() <= 0) {
        int t = r.top();
        int h = r.height();
        r.setTop(t + h - 1);
        r.setBottom(t);
    }
    return r;
}

QRect LSGRecSelector::getSelectedRect() const
{
    return normalizeSelection( selection );
}
