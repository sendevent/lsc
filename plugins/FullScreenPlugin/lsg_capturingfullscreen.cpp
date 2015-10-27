#include "lsg_capturingfullscreen.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QtPlugin>

#include <QDebug>
#if QT_VERSION >= 0x050000
#include <QScreen>
#endif //QT_VERSION >= 0x050000
/*static*/ QVector<QPainterPath> LSGCapturingFullScreen::mPaths = QVector<QPainterPath>();

int LSGCapturingFullScreen::getScreensCount() const
{
#if QT_VERSION >= 0x050000
    return QGuiApplication::screens().size();
#else
    return qApp->desktop()->screenCount();
#endif //QT_VERSION >= 0x050000
}

int LSGCapturingFullScreen::getAreasCount() const
{
    const int screensCnt = getScreensCount();
    return screensCnt == 1
            ? 1
            : screensCnt + 1; // separated screens + united
}

QString LSGCapturingFullScreen::getAreaDescription( int num ) const
{
    static const QString descr = QObject::tr( "Full screen%1" );
    const int screensCount = getScreensCount();

    QString suffix;
    if( screensCount == 1 )
        suffix = QString::null;
    else if( num >= screensCount )
        suffix = tr( " (all)" );
    else
    {
#if QT_VERSION >= 0x050000
        suffix = tr( " (#%1 — %2)" ).arg( num ).arg( QGuiApplication::screens().at( num )->name() );
#else
        suffix = tr( " (#%1)" ).arg( num );
#endif // QT_VERSION >= 0x050000
    }
    return descr.arg( suffix );
}

QPainterPath LSGCapturingFullScreen::getArea( int num ) const
{
    return selectArea( num );
}

QPainterPath LSGCapturingFullScreen::selectArea( int num ) const
{
    if( mPaths.isEmpty() )
    {
        QRect r;
#if QT_VERSION < 0x050000
        QDesktopWidget *pDesktop = qApp->desktop();
        for( int i = 0; i < pDesktop->screenCount(); ++i )
#else
        QList<QScreen *> screens = QGuiApplication::screens();
        for( int i = 0; i < screens.size(); ++i )
#endif // QT_VERSION < 0x050000

        {
            const QRect currR =

#if QT_VERSION < 0x050000
                    pDesktop->availableGeometry( i );
#else
                    screens.at( i )->availableGeometry();
#endif // QT_VERSION < 0x050000

            QPainterPath currPath;
            currPath.addRect( currR );
            mPaths.append( currPath );
            r = r.united( currR );
        }
        QPainterPath wholePath;
        wholePath.addRect( r );
        mPaths.append( wholePath );
    }
    
    return mPaths.at( num );
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(FullScreenPLugin, LSGCapturingFullScreen)
#endif
