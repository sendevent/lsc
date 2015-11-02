#ifndef GIFFRAMEPREVIEWER_H
#define GIFFRAMEPREVIEWER_H

#include <QFrame>
#include "imagewrapper.h"

namespace Ui {
class GifFramePreviewer;
}

class GifFramePreviewer : public QFrame
{
    Q_OBJECT

public:
    explicit GifFramePreviewer(QWidget *parent = 0);
    ~GifFramePreviewer();

    void setImages( ImagesList pImages );

private slots:

    void on_btnPrev_clicked();
    void on_btnRemove_clicked();
    void on_btnNext_clicked();

protected:

    void resizeEvent( QResizeEvent * event );

private:
    Ui::GifFramePreviewer *ui;
    ImagesList mImagesList;

    int mCurrFrameNum;

    ImageWrapperPtr currentFrame() const;
    void updateImage();
    void updateAddRmBtnText();
};

#endif // GIFFRAMEPREVIEWER_H
