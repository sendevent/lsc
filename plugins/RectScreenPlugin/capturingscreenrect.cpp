#include "capturingscreenrect.h"

#include "rectselector.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtPlugin>

/*static*/ QPainterPath LSGCapturingScreenRect::mPath = QPainterPath();

QString LSGCapturingScreenRect::getAreaDescription( int ) const
{
    static const QString descr = QObject::tr( "Rectangle" );
    return descr;
}

QPainterPath LSGCapturingScreenRect::getArea( int ) const
{
    return mPath;
}

QPainterPath LSGCapturingScreenRect::selectArea( int ) const
{
    mPath = QPainterPath();
    
    LSGRecSelector *selector = new LSGRecSelector();

    if( QDialog::Accepted == selector->exec() )
    {
        mPath.addRect( selector->getSelectedRect() );
    }
    
    delete selector;
    
    return mPath;
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ScreenRectPLugin, LSGCapturingScreenRect)
#endif
