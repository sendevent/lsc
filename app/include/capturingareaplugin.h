#ifndef LSGCapturingAreaPlugin_H
#define LSGCapturingAreaPlugin_H

#include <QtPlugin>

class QString;
class QPainterPath;

class LSGCapturingAreaPlugin
{
public:
    virtual ~LSGCapturingAreaPlugin() {}
    
    virtual int getAreasCount() const = 0;
    virtual QString getAreaDescription( int areaNum = 0 ) const = 0;
    virtual QPainterPath selectArea( int areaNum = 0 ) const = 0;
    virtual QPainterPath getArea( int areaNum = 0 ) const = 0;
};

Q_DECLARE_INTERFACE(LSGCapturingAreaPlugin, 
                     "org.livescreenshotcapturing.plugin.area/1.0")

#endif // LSGCapturingAreaPlugin_H
