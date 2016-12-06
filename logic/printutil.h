#ifndef PRINTUTIL_H
#define PRINTUTIL_H

#include <QObject>
#include "../graph/graphvertex.h"
#include <QList>
#include <QPainter>
#include <QList>
#include <QSize>
#include "const.h"
#include <graph/graph.h>

const int margin = 7;

QImage *generateImageFromMatrix(int width, int height, float **num, float infinity = 1000);
QImage *generateImageFromMatrix(Graph::TravellingSalesmanStep *travellingSalesmanStep, QList<GraphVertex *> *graph,
                                QString matrixName, bool isBringByRow = false, bool isBringByColumn = false,
                                bool isEstimateZiro = false,QPoint *strikeout = nullptr, float infinity = 1000);

QImage *generateImageFromMatrix(Graph::VengerStep *currentStep, QString name
                                , bool isSelectBestZero = false, bool isShowDeletion = false, float infinity = 1000);

void drawGraph(QPainter *painter, QList<GraphVertex *> *graph, QSize size);

QImage *drawGraph(QList<GraphVertex *> *graph, float width);

void saveImage(QImage *image,QString fileName);

inline QPoint getPositionOnField(QPointF *in,QSize size) {
    QPoint result((int)(in->x()*(float)size.width()),(int)(in->y()*(float)size.height()));
    return result;
}


class PrintUtil : public QObject
{
    Q_OBJECT
public:
    explicit PrintUtil(QObject *parent = 0);

signals:

public slots:
};

#endif // PRINTUTIL_H
