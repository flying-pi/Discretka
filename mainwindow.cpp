#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QLabel>
#include "optionmenurow.h"
#include "logic/printutil.h"
#include <QImage>
#include <QImageWriter>
#include <QTextDocument>
#include <QTextFrame>
#include "nameeditmenu.h"
#include "logic/discretcaprinter.h"
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QTextStream>
#include "./graph/graph.h"
#include "preview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->field,SIGNAL(onPointClicked(GraphVertex*)),this,SLOT(openMenuForPoint(GraphVertex*)));
    connect(ui->field,SIGNAL(onPointSelect(GraphVertex*)),this,SLOT(addNewPointConnection(GraphVertex*)));
    connect(ui->field,SIGNAL(onCancelSelection()),this,SLOT(clearOptionMenu()));

    printer   = new DiscretcaPrinter(this,nullptr);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openMenuForPoint(GraphVertex *v)
{
    selectedVertex = v;
    ui->option_list->clear();
    NameEditMenu *newRow = new NameEditMenu(ui->option_list,v);
    QListWidgetItem* item = new QListWidgetItem( ui->option_list );
    item->setSizeHint(newRow->sizeHint());
    ui->option_list->setItemWidget(item,newRow);
    for(int i=0;i<v->getChildCount();i++){
        addNewRowToOptionList( v->getChildAt(i));
    }
    ui->field->setMode(FieldMode::SELECT_ITEM_MODE);

}

void MainWindow::addNewPointConnection(GraphVertex *v)
{
    if(selectedVertex){
        selectedVertex->addConnection(v);
        addNewRowToOptionList(selectedVertex->getLastConnection());
    }
    ui->field->repaint();
}

void MainWindow::clearOptionMenu()
{
    ui->option_list->clear();
    ui->field->setMode(FieldMode::ADD_NEW_ITEM_MODE);
}

void MainWindow::addNewRowToOptionList(Connection *c)
{
    OptionMenuRow *newRow = new OptionMenuRow(ui->option_list,c);
    QListWidgetItem* item = new QListWidgetItem( ui->option_list );
    item->setSizeHint(newRow->sizeHint());
    ui->option_list->setItemWidget(item,newRow);
}

void MainWindow::on_pushButton_clicked()
{
    printer->setGraph(ui->field->getGraph());
    printer->print();
    (new Preview(nullptr,printer))->show();
}

void MainWindow::on_pushButton_2_clicked()
{
    QFileDialog objFlDlg(this);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.selectFile(("Граф (*."+graphTypeName+")"));
    objFlDlg.setDirectory("~/");
    QString filename =objFlDlg.getSaveFileName();
    if(filename.size()== 0)
        return;
    filename+="."+graphTypeName;
    QFile *file = new QFile(filename);
    file->open(QFile::WriteOnly);
    QTextStream outputStream(file);
    Graph *graph = new Graph(this,ui->field->getGraph());
    outputStream<<graph->siralizeToString();
    file->close();
}

void MainWindow::on_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,  tr("Відкрити "), "~/", ("Граф (*."+graphTypeName+")"));
    if(fileName.size()== 0)
        return;
    QFile *file = new QFile(fileName);
    file->open(QFile::ReadOnly);
    QTextStream outputStream(file);
    Graph *graph = new Graph(this,outputStream.readAll());
    file->close();
    ui->field->setGraph(graph->getAsList());
}

void MainWindow::on_pushButton_3_clicked()
{
    QFileDialog objFlDlg(this);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.selectFile("graph.png");
    QString filename =objFlDlg.getSaveFileName();



    QImage *image  = drawGraph(ui->field->getGraph(),1500);
    QImageWriter *imagefile= new QImageWriter();
    imagefile->setFileName(filename+".png");
    imagefile->setFormat("png");
    imagefile->setQuality(100);
    imagefile->write(*image);
}

void MainWindow::on_pushButton_4_clicked()
{
//    Graph *graph = new Graph(this,ui->field->getGraph());
//    QList<float **> *result = graph->floid(1000);
//    int size=ui->field->getGraph()->size();
//    for(int i=0;i<result->size();i++){

//        QImage *image  = generateImageFromMatrix(size,size,result->at(i),1000);
//        QImageWriter *imagefile= new QImageWriter();
//        imagefile->setFileName("/Users/yura/testFiles/4floid_"+QString::number(i)+".png");
//        imagefile->setFormat("png");
//        imagefile->setQuality(100);
//        imagefile->write(*image);
//    }
    DiscretcaPrinter printer(this,ui->field->getGraph());
    printer.printFloyd("/Users/yura/testFiles/test.pdf");
}

void MainWindow::on_generate3_clicked()
{
    printer->setGraph(ui->field->getGraph());
    printer->Venger();
    (new Preview(nullptr,printer))->show();
}

void MainWindow::on_generate2_clicked()
{
    printer->setGraph(ui->field->getGraph());
    printer->TravellingSalesman();
    (new Preview(nullptr,printer))->show();
}

void MainWindow::on_generate1_clicked()
{
    printer->setGraph(ui->field->getGraph());
    printer->printPrimaKraskel();
    (new Preview(nullptr,printer))->show();
}
