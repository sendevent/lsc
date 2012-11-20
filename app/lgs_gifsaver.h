#ifndef LGS_GIFSAVER_H
#define LGS_GIFSAVER_H

#include <QObject>
#include <QList>
#include <QPixmap>
#include <QSharedPointer>
#include <QImageIOHandler>

typedef QSharedPointer<QPixmap> QPixmapPtr;
typedef QList<QPixmapPtr> QPixmapsList;

class LGSGifSaver : public QObject
{
    Q_OBJECT
public:
    explicit LGSGifSaver(QObject *parent = 0);
    
    /*void save( const QString& path, const QPixmapsList& imagesList, int delay );
    QVariant option( QImageIOHandler::ImageOption option ) const;
    void write( const QString& path, const QPixmapsList& imagesList );
    void fiSave( const QString& path, const QPixmapsList& imagesList, int delay );*/
signals:
    void savingProgress( int steps, int step );
};

#endif // LGS_GIFSAVER_H
