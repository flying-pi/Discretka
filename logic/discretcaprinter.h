#ifndef DISCRETCAPRINTER_H
#define DISCRETCAPRINTER_H

#include <QObject>
#include <QPrinter>
#include "../graph/graphvertex.h"
#include <QList>
#include <QtGui/qtextcursor.h>
#include "../graph/graph.h"
#include <QFont>

class DiscretcaPrinter : public QObject
{
    Q_OBJECT
public:
    explicit DiscretcaPrinter(QObject *parent = 0,QList<GraphVertex *> *graph = new QList<GraphVertex *>());
    void print();
    void printFloyd(QString filePath);
    void printFloyd(QTextCursor *cursor,QPrinter &printer);
   void setGraph(QList<GraphVertex *> *graph );
    void saveToFile(QString filePath);
    void saveToODTFile(QString filePath);

    void printPrimaKraskel();
    void TravellingSalesman();
    void Venger();
    QTextDocument * getDoc();
signals:

public slots:

private :
    Graph *graph;
    QPrinter *printer;
    QTextDocument *doc;
    QTextCursor* cursor;
    void insertTravellingSalesmanStep(bool &isFirstIteration, Graph::TravellingSalesmanStep *, QString name);
    void insertTravellingSalesmanDeletedEdge(Graph::TravellingSalesmanStep *currentStep);
    void insertFlodIndex(QTextCursor *cursor,QTextCharFormat &txtformat,int top,int midel1,int midle2);
    void insertVertext(QTextCursor *cursor, QTextCharFormat &txtformat, QString name);
    void prepareToPrint();
    int taskNum;
    int imageNum;

    QTextCharFormat txtformat;
    QFont lableFont;
    QFont normalFont;
    QFont titleFont;

};

#endif // DISCRETCAPRINTER_H
