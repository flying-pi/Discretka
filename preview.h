#ifndef PREVIEW_H
#define PREVIEW_H

#include <QWidget>
#include <QTextDocument>
#include "./logic/discretcaprinter.h"

namespace Ui {
class Preview;
}

class Preview : public QWidget
{
    Q_OBJECT

public:
    explicit Preview(QWidget *parent = 0,DiscretcaPrinter *printer = nullptr);
    ~Preview();

private slots:
    void on_saveBtn_clicked();

    void on_saveAsODT_clicked();

private:
    Ui::Preview *ui;
    DiscretcaPrinter *printer;
};

#endif // PREVIEW_H
