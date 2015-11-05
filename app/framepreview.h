#ifndef FRAMEPREVIEW_H
#define FRAMEPREVIEW_H

#include "imagewrapper.h"
#include <QWidget>
#include <QCheckBox>

namespace Ui {
class FramePreview;
}

class FramePreview : public QWidget
{
    Q_OBJECT

public:
    explicit FramePreview(ImageWrapperPtr img, int num, QWidget *parent = 0);
    ~FramePreview();

signals:
    void clicked( const QPoint& pnt, int ) const;

protected:
    void mouseReleaseEvent(QMouseEvent *);
private:
    Ui::FramePreview *ui;
    ImageWrapperPtr mImg;
    QCheckBox *mCheckBox;

    const int mNum;

};

#endif // FRAMEPREVIEW_H
