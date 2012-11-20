#include "lsg_capturingfullscreen.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtPlugin>

#include <QDebug>

/*static*/ QPainterPath LSGCapturingFullScreen::mPath = QPainterPath();

QString LSGCapturingFullScreen::getAreaDescription() const
{
    static const QString descr = QObject::tr( "Full screen" );
    return descr;
}

QPainterPath LSGCapturingFullScreen::getArea() const
{
    qDebug() << Q_FUNC_INFO << mPath;
    selectArea();
    return mPath;
}

QPainterPath LSGCapturingFullScreen::selectArea() const
{
    qDebug() << Q_FUNC_INFO << mPath;
    if( mPath.isEmpty() )
    {
        mPath.addRect( QApplication::desktop()->geometry() );
        qDebug() << Q_FUNC_INFO << "2:" << mPath;
    }
    
    return mPath;
}

Q_EXPORT_PLUGIN2(FullScreenPLugin, LSGCapturingFullScreen)
