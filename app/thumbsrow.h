#ifndef THUMBSROW_H
#define THUMBSROW_H

#include <QWidget>

namespace Ui {
class ThumbsRow;
}

class FramePreview;

class ThumbsRow : public QWidget
{
    Q_OBJECT

public:
    explicit ThumbsRow(QWidget *parent = 0);
    ~ThumbsRow();

    void addThumb( FramePreview *pThumb );

private:
    Ui::ThumbsRow *ui;

    void resizeEvent( QResizeEvent*e);
};

#endif // THUMBSROW_H
