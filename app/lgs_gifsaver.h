#ifndef LGS_GIFSAVER_H
#define LGS_GIFSAVER_H

#include <QObject>
#include <QList>
#include <QPixmap>
#include <QSharedPointer>
#include <QImageIOHandler>

typedef QSharedPointer<QPixmap> QPixmapPtr;
typedef QList<QPixmapPtr> QPixmapsList;

class GifOptionsDialog;

class LGSGifSaver : public QObject
{
    Q_OBJECT
public:
    explicit LGSGifSaver(QObject *parent = 0);
    ~LGSGifSaver();
    
    void save( const QPixmapsList& imagesList, int delay ) const;
signals:
    void savingProgress( int steps, int step, const QString& msg ) const;

private:
    GifOptionsDialog *mSaveDlg;
};

#endif // LGS_GIFSAVER_H
