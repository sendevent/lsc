#include "lsg_capturingscreenrect.h"

#include "lsg_rectselector.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtPlugin>

/*static*/ QPainterPath LSGCapturingScreenRect::mPath = QPainterPath();

QString LSGCapturingScreenRect::getAreaDescription() const
{
    static const QString descr = QObject::tr( "Rectangle" );
    return descr;
}

QPainterPath LSGCapturingScreenRect::getArea() const
{
    return mPath;
}

QPainterPath LSGCapturingScreenRect::selectArea() const
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

Q_EXPORT_PLUGIN2(ScreenRectPLugin, LSGCapturingScreenRect)
