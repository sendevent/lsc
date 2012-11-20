#ifndef LSG_CAPTURINGFULLSCREEN_H
#define LSG_CAPTURINGFULLSCREEN_H

#include <QObject>
#include "lsg_capturingareaplugin.h"


class LSGCapturingFullScreen : 
        public QObject, 
        public LSGCapturingAreaPlugin
{
    Q_OBJECT
    Q_INTERFACES(LSGCapturingAreaPlugin)
public:
    QString getAreaDescription() const;
    QPainterPath selectArea() const;
    QPainterPath getArea() const;
    
private:
    static QPainterPath mPath;
};

#endif // LSG_CAPTURINGFULLSCREEN_H