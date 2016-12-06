#include "field.h"
#include "ui_field.h"
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <qdatetime.h>
#include <cmath>
#include <QString>
#include "logic/printutil.h"


Field::Field(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Field)
{
    ui->setupUi(this);
    allItem = new QList<GraphVertex *>();
}

Field::~Field()
{
    for(int i=0;i<allItem->size();i++)
        delete allItem->at(i);
    delete allItem;
    delete ui;
}

void Field::setMode(FieldMode mode)
{
    this->mode = mode;
}

void Field::setGraph(QList<GraphVertex *> *graph)
{
    this->allItem = graph;
    repaint();
}

void Field::mousePressEvent(QMouseEvent *event)
{
    float realX = ((float)event->x())/((float)width());
    float realY = ((float)event->y())/((float)height());
    float realR = pointPixelSize/((float)std::max(width(),height()));
    realR *= realR;
    for(int i=0;i<allItem->size();i++){
        QPointF *p = allItem->at(i)->getPos();
        if(p){
            if(pow(p->x()-realX,2)+pow(p->y()-realY,2)<realR){
                if(event->button() == Qt::RightButton){
                    emit onPointClicked(allItem->at(i));
                }else if(mode == FieldMode::SELECT_ITEM_MODE){
                    emit onPointSelect(allItem->at(i));
                }
                return;
            }
        }
    }
    if(mode == FieldMode::SELECT_ITEM_MODE){
        emit onCancelSelection();
        return;
    }
    if(event->button() == Qt::LeftButton){
        GraphVertex *newItem=  new GraphVertex(this,realX,realY);
        allItem->append(newItem);
        newItem->setName(QString::number(allItem->size()));
    }
    repaint();
}

void Field::paintEvent(QPaintEvent *)
{
        painter =new QPainter(this);
    drawGraph(painter,allItem, this->size());
}

