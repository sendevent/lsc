#ifndef LSG_CAPTURINGFULLSCREEN_H
#define LSG_CAPTURINGFULLSCREEN_H

#include <QObject>
#include "capturingareaplugin.h"


class LSGCapturingScreenRect : 
        public QObject, 
        public LSGCapturingAreaPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.livescreenshotcapturing.plugin.area" )
#endif

    Q_INTERFACES(LSGCapturingAreaPlugin)
    
public:
    int getAreasCount() const { return 1; }
    QString getAreaDescription( int ) const;
    QPainterPath selectArea( int ) const;
    QPainterPath getArea( int ) const;
    
private:
    static QPainterPath mPath;
};

#endif // LSG_CAPTURINGFULLSCREEN_H
