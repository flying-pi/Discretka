#include "preview.h"
#include "ui_preview.h"

#include <qfiledialog.h>
#include <QDebug>


Preview::Preview(QWidget *parent, DiscretcaPrinter *printer) :
    QWidget(parent),
    ui(new Ui::Preview)
{
    ui->setupUi(this);
    if(printer)
        ui->textBrowser->setDocument(printer->getDoc());
    this->printer = printer;
}

Preview::~Preview()
{
    delete ui;
}

void Preview::on_saveBtn_clicked()
{
    QFileDialog objFlDlg(this);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.setDirectory("~/");
    QString filename =objFlDlg.getSaveFileName();
    if(filename.size()== 0)
        return;
    qDebug()<<filename;
    printer->saveToFile(filename+".pdf");
}

void Preview::on_saveAsODT_clicked()
{QFileDialog objFlDlg(this);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.setDirectory("~/");
    QString filename =objFlDlg.getSaveFileName();
    if(filename.size()== 0)
        return;
    qDebug()<<filename;
    printer->saveToODTFile(filename+".odf");
}
