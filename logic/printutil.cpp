#include "printutil.h"
#include <QPainter>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QPoint>
#include <QImageWriter>
#include <cmath>
#include <graph/graph.h>
#include <QBitmap>
#include <QDebug>


PrintUtil::PrintUtil(QObject *parent) : QObject(parent)
{

}

QImage *generateImageFromMatrix(int width, int height, float **num, float infinity)
{

    QString ** text = new QString*[width];
    int longestText = 0;
    QFont font("times", 30);
    QFontMetrics fm(font);

    for(int i=0;i<width;i++){
        text[i] = new QString[height];
        for(int j=0;j<height;j++){
            text[i][j] = (num[i][j]<infinity)?QString::number(num[i][j]):"∞";
            if(fm.width(text[i][j])>longestText) longestText = fm.width(text[i][j]);
        }
    }
    int imageW = longestText*width+fm.width(" ")*(width-1)+margin*4;
    int imageH = fm.height()*(height)+margin*3;
    QImage *result = new QImage(imageW, imageH, QImage::Format_ARGB32_Premultiplied);
    result->fill(QColor::fromRgba(qRgba(255,255,255,255)));

    QPainter *painter = new QPainter(result);
    painter->setRenderHint(QPainter::HighQualityAntialiasing,true);
    painter->setBackgroundMode(Qt::TransparentMode);
    QBrush blackBrush(Qt::black);
    painter->setBrush(blackBrush);
    painter->setPen(QPen(Qt::black,2,Qt::SolidLine));

    // --
    // |
    // |
    // --
    painter->drawLine(margin,margin,margin,imageH-margin);
    painter->drawLine(margin,margin,margin*2,margin);
    painter->drawLine(margin,imageH-margin,margin*2,imageH-margin);

    //      --
    //       |
    //       |
    //      --
    painter->drawLine(imageW -margin,margin,imageW -margin,imageH-margin);
    painter->drawLine(imageW -margin,margin,imageW -margin*2,margin);
    painter->drawLine(imageW -margin,imageH-margin,imageW -margin*2,imageH-margin);

    painter->setFont(font);

    int offsetX;
    int offsetY = margin*5;
    int spaceLenth = fm.width(" ");
    for(int i=0;i<width;i++){
        offsetX = margin*2;
        for(int j=0;j<height;j++){
            while(fm.width(text[i][j])<=longestText)
                text[i][j]+=" ";
            painter->drawText(offsetX,offsetY,text[i][j]);
            offsetX+= longestText + spaceLenth;
        }
        offsetY+=fm.height();
    }
    painter->end();
    result = new QImage(result->scaled(QSize(result->width()/2,result->height()/2)));
    return result;
}

void drawGraph(QPainter *painter, QList<GraphVertex *> *graph, QSize size)
{
    QBrush brush(QColor::fromRgb(0,200,0));
    painter->setBrush(brush);
    QFont font("times", 14);
    QFontMetrics fm(font);
    painter->setFont(font);

    for(int i=0;i<graph->size();i++){
        QPointF *start = graph->at(i)->getPos();
        if(!(start)) continue;
        QPoint realStart = getPositionOnField(start,size);
        for(int j =0;j<graph->at(i)->getChildCount();j++){
            QPointF *end = graph->at(i)->getChildAt(j)->getEndpoint()->getPos();
            if(!(end)) continue;
            QPoint realEnd = getPositionOnField(end,size);
            painter->drawLine(realStart,realEnd);
            QPoint center((realEnd.x()+realStart.x())/2,(realEnd.y()+realStart.y())/2);
            center = QPoint((realStart.x()+center.x())/2,(realStart.y()+center.y())/2);
            painter->drawText(center, QString::number(graph->at(i)->getChildAt(j)->getWeigth()));
            //            float k = ((double)(realEnd.y()- realStart.y()))/((double)(realEnd.x()-realStart.x()));
            //            float angle  = atan(k);
            //            float arraowAngle  = M_PI/60;
            //            float x = realEnd.x() - cos(angle+arraowAngle)*25;
            //            float y = realEnd.y() - sin(angle+arraowAngle)*25;
            //            painter->drawLine(realEnd.x(),realEnd.y(),x,y);
            //             x = realEnd.x() - cos(angle-arraowAngle)*25;
            //             y = realEnd.y() - sin(angle-arraowAngle)*25;
            //            painter->drawLine(realEnd.x(),realEnd.y(),x,y);
        }
    }
    for(int i=0;i<graph->size();i++){
        QPointF *start = graph->at(i)->getPos();
        if(!(start)) continue;
        QPoint realStart = getPositionOnField(start,size);
        painter->drawEllipse(realStart.x()-pointPixelSize/2,realStart.y()-pointPixelSize/2,pointPixelSize,pointPixelSize);
        realStart.setX(realStart.x()-fm.width(graph->at(i)->getName()));
        realStart.setY(realStart.y()-fm.height());
        painter->drawText(realStart,graph->at(i)->getName());
    }
}

QImage *drawGraph(QList<GraphVertex *> *graph, float width)
{
    float minW =10,minH=10,maxW=0,maxH=0;
    for(int i=0;i<graph->size();i++){
        QPointF *p = graph->at(i)->getPos();
        if(minW>p->x())
            minW = p->x();
        if(maxW<p->x())
            maxW = p->x();
        if(minH>p->y())
            minH = p->y();
        if(maxH<p->y())
            maxH = p->y();
    }
    float height = (width*(maxH-minH))/(maxW - minW);


    QImage *result = new QImage(width, height, QImage::Format_ARGB4444_Premultiplied);
    result->fill(QColor::fromRgba(qRgba(255,255,255,255)));
    QPainter *painter = new QPainter(result);
    painter->setRenderHint(QPainter::HighQualityAntialiasing,true);
    painter->setBackgroundMode(Qt::TransparentMode);
    drawGraph(painter,graph,QSize(width,height));
    return  result;
}

void saveImage(QImage *image, QString fileName)
{
    QImageWriter *imagefile= new QImageWriter();
    imagefile->setFileName(fileName);
    imagefile->setFormat("png");
    imagefile->setQuality(100);
    imagefile->write(*image);
}

QImage *generateImageFromMatrix(Graph::TravellingSalesmanStep *travellingSalesmanStep, QList<GraphVertex *> *graph, QString matrixName,
                                bool isBringByRow, bool isBringByColumn, bool isEstimateZiro, QPoint *strikeout, float infinity)
{
    int w=0;
    int h=0;
    int x =0;
    int y=0;
    int maxW;
    int matrixStart;
    QFont font("times", 16);
    QFontMetrics fm(font);
    QFont smallFont("times", 10);
    QFontMetrics smallFm(smallFont);
    w+=fm.width(matrixName);
    w+=fm.width(" = ");
    matrixStart = x = w;
    maxW = 0;
    bool isNeedRightSide = false;
    bool isDownSide = false;
    QString buf = "";
    int bring;
    int bufSize;

    for(int i=0;i<travellingSalesmanStep->size;i++){
        if(fm.width(graph->at(travellingSalesmanStep->rowsNum[i])->getName())>maxW){
            maxW = fm.width(graph->at(travellingSalesmanStep->rowsNum[i])->getName());
        }
    }
    for(int i=0;i<travellingSalesmanStep->size;i++){
        for(int j=0;j<travellingSalesmanStep->size;j++){
            buf = ((travellingSalesmanStep->matrix[i][j])>=infinity?"∞":QString::number(travellingSalesmanStep->matrix[i][j]));
            bufSize = fm.width(buf);
            if(isEstimateZiro && travellingSalesmanStep->zeroAlternative[i][j]>=0){
                QString index = (travellingSalesmanStep->zeroAlternative[i][j]<infinity?QString::number(travellingSalesmanStep->zeroAlternative[i][j]):"∞");

                bufSize += smallFm.width(index);
                bufSize += smallFm.width("()");
            }
            if(bufSize>maxW){
                maxW = bufSize;
            }
        }
    }
    for(int i=0;i<travellingSalesmanStep->size;i++){
        isNeedRightSide = isNeedRightSide || travellingSalesmanStep->bringingRows[i]!=0;
        if(isNeedRightSide){
            bring = travellingSalesmanStep->bringingRows[i];
            if(bring<infinity&&fm.width(graph->at(travellingSalesmanStep->bringingRows[i])->getName())>maxW){
                maxW = fm.width(graph->at(travellingSalesmanStep->bringingRows[i])->getName());
            }
        }
    }
    w+=2;
    w += maxW*(travellingSalesmanStep->size+1)+fm.width(" ")*(travellingSalesmanStep->size);
    isNeedRightSide = isNeedRightSide&&(!isEstimateZiro);
    if(isNeedRightSide) w+=(maxW+fm.width(" "));
    h = fm.height()*(travellingSalesmanStep->size+1);
    for(int i=0;i<travellingSalesmanStep->size;i++){
        isDownSide = travellingSalesmanStep->bringingColumn[i]!=0;
        if(isDownSide)
            break;
    }

    isDownSide = isDownSide&&(!isEstimateZiro);
    if(isDownSide&&isBringByColumn) h+=fm.height();



    QImage *result = new QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    result->fill(QColor::fromRgba(qRgba(255,255,255,255)));

    QPainter *painter = new QPainter(result);
    painter->setFont(font);
    painter->setRenderHint(QPainter::TextAntialiasing,true);
    QBrush blackBrush(Qt::black);
    QPen pen = QPen(Qt::black,2,Qt::SolidLine);
    pen.setBrush(blackBrush);
    pen.setCosmetic(false);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);

    painter->drawText(0,h/2+fm.height()/2,(matrixName + " = "));
    y = fm.ascent();
    for(int i=-1;i<=travellingSalesmanStep->size;i++){
        for (int j = -1; j <= travellingSalesmanStep->size; ++j) {
            if(i==-1 && j>=0&& j<travellingSalesmanStep->size){
                painter->drawText(x,y,graph->at(travellingSalesmanStep->collumnNum[j])->getName());
            }
            if(j==-1 && i>=0 && i < travellingSalesmanStep->size)
            {
                painter->drawText(x,y,graph->at(travellingSalesmanStep->rowsNum[i])->getName());
            }
            if(i>=0 && i < travellingSalesmanStep->size&& j>=0&& j<travellingSalesmanStep->size){
                float num = travellingSalesmanStep->matrix[i][j];
                if(!isBringByRow){
                    if(travellingSalesmanStep->bringingColumn[j]!=infinity)
                        num+=travellingSalesmanStep->bringingColumn[j];
                    if(travellingSalesmanStep->bringingRows[i]!=infinity)
                        num+=travellingSalesmanStep->bringingRows[i];
                }else
                    if(!isBringByColumn)
                        if(travellingSalesmanStep->bringingColumn[j]!=infinity)
                            num+=travellingSalesmanStep->bringingColumn[j];

                painter->drawText(x,y,((num)>=infinity?"∞":QString::number(num)));
                if(isEstimateZiro && travellingSalesmanStep->zeroAlternative[i][j]>=0){
                    painter->setFont(smallFont);
                    QString index = (travellingSalesmanStep->zeroAlternative[i][j]<infinity?QString::number(travellingSalesmanStep->zeroAlternative[i][j]):"∞");
                    painter->drawText(x+fm.width(num>=infinity?"∞":QString::number(num)),y-fm.ascent()+smallFm.ascent(),"("+index+")");
                    painter->setFont(font);
                }

                if(strikeout!=nullptr){
                    if(strikeout->x() == j){
                        painter->drawLine(x+fm.width(" "),fm.height(),x+fm.width(" "),h);
                    }
                    if(strikeout->y() == i ){
                        painter->drawLine(x-maxW/2,y-fm.ascent()/2,x+maxW,y-fm.ascent()/2);
                    }
                }
            }
            if(isNeedRightSide&&isBringByRow){
                if(i>=0 && i < travellingSalesmanStep->size&& j==travellingSalesmanStep->size ){
                    float bring = travellingSalesmanStep->bringingRows[i];

                    if(bring!=0 && bring<infinity){
                        painter->drawText(x,y,QString::number(bring));
                        painter->drawLine(x-fm.width(" ")/2,y-fm.ascent(),x-fm.width(" ")/2,y-fm.ascent()+fm.height());
                    }
                }
            }
            if(isDownSide &&isBringByColumn){
                if(j>=0 && j < travellingSalesmanStep->size&& i==travellingSalesmanStep->size ){
                    float bring = travellingSalesmanStep->bringingColumn[j];
                    if(bring!=0 && bring<infinity){
                        painter->drawText(x,y,QString::number(bring));
                        painter->drawLine(x,y-fm.ascent()-(fm.height()/2- fm.ascent()/2),x+maxW,y-fm.ascent()-(fm.height()/2- fm.ascent()/2));
                    }
                }
            }
            x+=maxW;
            x+=fm.width(" ");
        }
        x=matrixStart;
        y+=fm.height();
    }
    return result;

}

QImage *generateImageFromMatrix(Graph::VengerStep *currentStep, QString name,bool isSelectBestZero, bool isShowDeletion,float infinity)
{
    int w = 0;
    int h =0;
    int x,y;
    int xOffset;
    int maxW=0;
    QFont font("times", 16);
    QFontMetrics fm(font);
    w +=fm.width(name);
    w += fm.width(" =");
    w+=2;
    xOffset = w;
    QStringList element;
    for(int i=0;i<currentStep->size;i++){
        for(int j=0;j<currentStep->size;j++)
        {
            if(isSelectBestZero || isShowDeletion){
                Graph::VengerStepEgervary *step = (Graph::VengerStepEgervary *)currentStep;
                element.append(step->sourceMatrix[i][j]>=infinity?"∞":QString::number(step->sourceMatrix[i][j]));
            }else{
            element.append(currentStep->result[i][j]>=infinity?"∞":QString::number(currentStep->result[i][j]));
            }
            if(fm.width(element.last())>maxW){
                maxW = fm.width(element.last());
            }
        }
    }
    xOffset+=maxW;
    w+=maxW*(currentStep->size+2)+fm.width(" ")*(currentStep->size-1);
    h = fm.height()*(currentStep->size+2);
    QImage *result = new QImage(w, h, QImage::Format_ARGB32_Premultiplied);
    result->fill(QColor::fromRgba(qRgba(255,255,255,255)));

    QPainter *painter = new QPainter(result);
    painter->setFont(font);
    painter->setRenderHint(QPainter::TextAntialiasing,true);
    QBrush blackBrush(Qt::black);
    QPen pen = QPen(Qt::black,2,Qt::SolidLine);
    pen.setBrush(blackBrush);
    pen.setCosmetic(false);
    pen.setCapStyle(Qt::FlatCap);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);
    painter->drawText(0,h/2+fm.ascent()/2,(name + " ="));
    int iterator=0;
    y = fm.ascent();
    for(int i=0;i<currentStep->size;i++)
    {
        x = xOffset;
        y += fm.height();
        for(int j=0;j<currentStep->size;j++){
            painter->drawText(x,y,element.at(iterator));
            if(isSelectBestZero){
                Graph::VengerStepEgervary *step = (Graph::VengerStepEgervary *)currentStep;
                if(step->selectedZeroInRows[i] == j){
                    qDebug()<<"slected item "<< i << step->selectedZeroInRows[i];
                    painter->drawRect(x-2,y-fm.ascent(),fm.width(element.at(iterator))+4,fm.height()-2);
                }
            }
            if(isShowDeletion){
                Graph::VengerStepEgervary *step = (Graph::VengerStepEgervary *)currentStep;
                if(step->checkZeroCollumn[j] && i ==0){
                    painter->drawLine(x+fm.width(element.at(iterator))/2,fm.ascent(),x+fm.width(element.at(iterator))/2,h-fm.ascent());
                }
                if(step->checkZeroRow[i] && j ==0){
                    painter->drawLine(xOffset,y-fm.ascent()/2,w-maxW,y-fm.ascent()/2);
                }

            }
            iterator++;
            x+=maxW + fm.width(" ");
        }
    }
    int lineLenth = fm.width("_");

    painter->drawLine(xOffset-lineLenth,fm.height()/2,xOffset-lineLenth,h-fm.height()/2);
    painter->drawLine(xOffset-lineLenth,fm.height()/2,xOffset,fm.height()/2);
    painter->drawLine(xOffset-lineLenth,h-fm.height()/2,xOffset,h-fm.height()/2);

    painter->drawLine(w-maxW+lineLenth,fm.height()/2,w-maxW+lineLenth,h-fm.height()/2);
    painter->drawLine(w-maxW+lineLenth,fm.height()/2,w-maxW,fm.height()/2);
    painter->drawLine(w-maxW+lineLenth,h-fm.height()/2,w-maxW,h-fm.height()/2);

    return result;
}
