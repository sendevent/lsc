#ifndef GIFFRAMEPREVIEWER_H
#define GIFFRAMEPREVIEWER_H

#include <QFrame>
#include "imagewrapper.h"

namespace Ui {
class GifFramePreviewer;
}

class ThumbsRow;

class GifFramePreviewer : public QFrame
{
    Q_OBJECT

public:
    explicit GifFramePreviewer(QWidget *parent = 0);
    ~GifFramePreviewer();

    void setImages( ImagesList pImages );

private slots:

    void onPreviewSelected( const QPoint& pnt, int );

    void test(int);

protected:

    void resizeEvent( QResizeEvent * event );

private:
    Ui::GifFramePreviewer *ui;
    ImagesList mImagesList;

    int mCurrFrameNum;

    ThumbsRow *pThumbsRow;

    ImageWrapperPtr currentFrame() const;
    void updateImage();
};

#endif // GIFFRAMEPREVIEWER_H
