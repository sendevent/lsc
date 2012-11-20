#ifndef LSGCapturingAreaPlugin_H
#define LSGCapturingAreaPlugin_H

#include <QtPlugin>

class QString;
class QPainterPath;

class LSGCapturingAreaPlugin
{
public:
    virtual ~LSGCapturingAreaPlugin() {}
    
    virtual QString getAreaDescription() const = 0;
    virtual QPainterPath selectArea() const = 0;
    virtual QPainterPath getArea() const = 0;
};

Q_DECLARE_INTERFACE(LSGCapturingAreaPlugin, 
                     "org.livescreenshotcapturing.plugin.area/1.0")

#endif // LSGCapturingAreaPlugin_H
