#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <logic/discretcaprinter.h>
#include "./graph/graphvertex.h"

namespace Ui {
class MainWindow;
class Field;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

private slots:
    void openMenuForPoint(GraphVertex *v);
    void addNewPointConnection(GraphVertex *v);
    void clearOptionMenu();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_load_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_generate3_clicked();

    void on_generate2_clicked();

    void on_generate1_clicked();

private:
    GraphVertex* selectedVertex = NULL;
    void addNewRowToOptionList(  Connection *c );
      DiscretcaPrinter *printer;
};

#endif // MAINWINDOW_H
