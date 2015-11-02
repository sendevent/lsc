#ifndef GIFOPTIONSDIALOG_H
#define GIFOPTIONSDIALOG_H

#include <QDialog>
#include "imagewrapper.h"

namespace Ui {
class GifOptionsDialog;
}

class GifOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GifOptionsDialog(QWidget *parent = 0);
    ~GifOptionsDialog();

    int getQuality() const;
    QString getPath() const;
    unsigned int getCompression() const;

    void setImagesList( ImagesList pImages );


private slots:
    void on_pathButton_clicked();

private:
    Ui::GifOptionsDialog *ui;


};

#endif // GIFOPTIONSDIALOG_H
