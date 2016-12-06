#include "graph.h"

#include <QStack>
#include <QStringList>
#include <QDebug>
#include <QImageWriter>

#include "logic/printutil.h"

Graph::Graph(QObject *parent,QList<GraphVertex *> *graph) : QObject(parent)
{
    vertexes = graph;
}

Graph::Graph(QObject *parent, QString str): QObject(parent)
{
    QStringList strList = str.split("\n");
    vertexes = new QList<GraphVertex *>();
    if(strList.size() == 0) return;
    int lenth = strList.at(0).toInt();
    strList.removeFirst();
    for(int i=0;i<lenth;i++){
        QStringList vertextString = strList.first().split("\t");
        vertexes->append(new GraphVertex(this,vertextString.at(1).toDouble(),vertextString.at(2).toDouble()));
        vertexes->last()->setName(vertextString.at(0));
        strList.removeFirst();
    }
    for(int i=0;i<lenth;i++){
        QStringList connectionList = strList.first().split(":").at(1).split(" ");
        for(int c=0;c<connectionList.size();c++){
            if(connectionList.at(c).length()<3)continue;
            QStringList connectionDescription = connectionList.at(c).split("\t");
            GraphVertex *endpoint = vertexes->at(connectionDescription.at(0).toInt());
            float w = connectionDescription.at(1).toFloat();
            vertexes->at(i)->addConnection(endpoint,w);
        }
        strList.removeFirst();
    }
}

Graph::~Graph()
{
}

QList<Connection *> *Graph::getAllEdges()
{
    if(edges)
        return edges;
    edges = new QList<Connection *> ();
    for(int i=0;i<vertexes->size();i++)
        for(int e=0;e<vertexes->at(i)->getChildCount();e++)
            edges->append(vertexes->at(i)->getChildAt(e));
    return edges;
}

QList<Graph::EdgesByLenth *> *Graph::getSortedEdges()
{
    getAllEdges();
    QList<EdgesByLenth *> * result = new QList<EdgesByLenth *> ();
    for(int i=0;i<edges->size();i++){
        Connection *con = edges->at(i);

        for(int c = 0;c<=result->size();c++){
            if(result->size() == c){
                EdgesByLenth *newItem = new EdgesByLenth();
                newItem->weight = con->getWeigth();
                newItem->edges->append(con);
                result->append(newItem);
                break;
            }
            if(result->at(c)->weight == con->getWeigth())
            {
                result->at(c)->edges->append(con);
                break;
            }
            if(result->at(c)->weight > con->getWeigth()){
                EdgesByLenth *newItem = new EdgesByLenth();
                newItem->weight = con->getWeigth();
                newItem->edges->append(con);
                result->insert(c,newItem);
                break;
            }
        }
    }
    return result;
}

Graph::PrimaKraskelResult *Graph::primaKraskel()
{
    return primaKraskel(getSortedEdges());
}

Graph::TravellingSalesmanStep * Graph::TravellingSalesmanAlgaritm()
{
    TravellingSalesmanStep *firstStep = new TravellingSalesmanStep(vertexes->size(),toMatrix(true));
    TravellingSalesmanStep *currentStep = new TravellingSalesmanStep(vertexes->size(),toMatrix(true));
    currentStep = firstStep;
    firstStep->selectedFork = 0;
    QList<GraphVertex *> *newGraph = new QList<GraphVertex *>();
    float w;
    Connection *con;
    for(int i=0;i<vertexes->size();i++)
        newGraph->append(vertexes->at(i)->copy());
    while (currentStep->size>1) {

        bringingMatrix(currentStep);
        estimateZero(currentStep);
        QList<QPoint> maxPositions = currentStep->biggestZero.size() != 0?currentStep->biggestZero:
                                                                          maxPoint(currentStep->zeroAlternative,currentStep->size);
        currentStep->biggestZero = maxPositions;

        qDebug()<<"current step metic "<< currentStep->forks->size() << currentStep->selectedFork;
        currentStep->forks->clear();
        currentStep->deletedEdges->clear();
        currentStep->selectedFork = -1;
        selectBestWay(maxPositions,currentStep,newGraph);

        int minForkPos = currentStep->selectedFork;

        currentStep->deletedEdges->clear();
        currentStep->deletedEdges = currentStep->forks->at(minForkPos)->parent->deletedEdges;
        currentStep = currentStep->forks->at(minForkPos)->parent;
        qDebug()<<"selected Edge"<<currentStep->getColumnPos(currentStep->selectEdge[0])
                <<currentStep->getRowPos(currentStep->selectEdge[1]);

        con = vertexes->at(currentStep->selectEdge[0])->getConnectionFor(vertexes->at(currentStep->selectEdge[1]));
        if(con != nullptr)
            w = con->getWeigth();
        else
        {
            con = vertexes->at(currentStep->selectEdge[1])->getConnectionFor(vertexes->at(currentStep->selectEdge[0]));
            w = con->getWeigth();
        }
        newGraph->at( currentStep->selectEdge[0] )->addConnection(newGraph->at( currentStep->selectEdge[1] ));
        newGraph->at( currentStep->selectEdge[1] )->addConnection(newGraph->at( currentStep->selectEdge[0] ),w);


        currentStep = currentStep->forks->at(minForkPos);

        if(currentStep->size == 1){
            currentStep->selectEdge[0] = currentStep->collumnNum[0];
            currentStep->selectEdge[1] = currentStep->rowsNum[0];
            currentStep->bringingColumn=new float[1] {0};
            currentStep->bringingRows=new float[1]  {0};
            newGraph->at(currentStep->selectEdge[0])->addConnection(newGraph->at(currentStep->selectEdge[1]));
            currentStep->biggestZero.append( QPoint(0,0));
            currentStep->selectedFork = 0;
        }


        QImage *image = drawGraph(newGraph,500);
        QImageWriter *imagefile= new QImageWriter();
        static int k=1;
        imagefile->setFileName("/Users/yura/testFiles/travalingMan"+QString::number(k)+".png");
        imagefile->setFormat("png");
        imagefile->setQuality(100);
        imagefile->write(*image);
        k++;
    }

    newGraph->clear();
    for(int i=0;i<vertexes->size();i++){
        newGraph->append(vertexes->at(i)->copy());
    }
    while (currentStep->parent) {
        qDebug()<<vertexes->at(currentStep->selectEdge[0])->getName() <<"-"<< vertexes->at(currentStep->selectEdge[1])->getName()
                <<(currentStep->selectEdge[0]) +1 <<"-"<< (currentStep->selectEdge[1])+1 << "selected fork ::" << currentStep->selectedFork;
        newGraph->at(currentStep->selectEdge[0])->addConnection(newGraph->at(currentStep->selectEdge[1]));
        currentStep = currentStep->parent;
    }

    QImage *image = drawGraph(newGraph,500);
    QImageWriter *imagefile= new QImageWriter();
    imagefile->setFileName("/Users/yura/testFiles/travalingMan.png");
    imagefile->setFormat("png");
    imagefile->setQuality(100);
    imagefile->write(*image);
    qDebug()<<"__________________________________";
    return firstStep;


}

QList<Graph::VengerStep *> * Graph::VengerAlgoritm()
{
    float **matrix = toMatrix();
    QList<VengerStep *> *result = new QList<VengerStep *>();
    bool isContinue = true;
    int size = vertexes->size();
    do{
        VengerStep *firstStep = new VengerStep();
        firstStep->result  = removeInfinityCollumn(matrix,size);
        firstStep->size = size;
        result->append(firstStep);
        VengerStepBringing *secondStep = new VengerStepBringing(firstStep->result,firstStep->size);
        secondStep->isByCollumn = false;
        bringByRow(secondStep->result,secondStep->koef,secondStep->size);
        for(int i=0;i<secondStep->size;i++)
            secondStep->kSum+=secondStep->koef[i];
        result->append(secondStep);


        VengerStepBringing *third = new VengerStepBringing(secondStep->result,secondStep->size);
        third->isByCollumn = true;
        bringByCollumn(third->result,third->koef,third->size);
        for(int i=0;i<third->size;i++)
            third->kSum+=third->koef[i];
        result->append(third);


        VengerStepEgervary *fourth = new VengerStepEgervary(third->result,third->size);
        isContinue = egervary(fourth->result,fourth->minValuePos,fourth->selectedZeroInRows
                              ,fourth->checkZeroRow,fourth->checkZeroCollumn,fourth->size);
        fourth->isFinal = !isContinue;
        matrix = fourth->result;
        result->append(fourth);
    }
    while (isContinue);
    return result;
}

//якщо в кінці виходить нескінченість  - вичеркувати її і повертатись назад

float **Graph::copy(float **source,int size){
    float **matrix= new float*[size];
    for(int i=0;i<size;i++){
        matrix[i] = new float[size];
        for(int j=0;j<size;j++){
            matrix[i][j] = source[i][j];
        }
    }
    return matrix;
}

QStringList Graph::matrixToString(float **matrix, int size)
{
    QStringList result;
    for(int i=0;i<size;i++){
        for (int j = 0; j < size; ++j) {
            result<<(QString::number(matrix[i][j]));
            result << ("\t");
        }
        result << ("\n");
    }
    return result;
}

void Graph::printToDebufMatrix(float **matrix, int size)
{
    for(int i=0;i<size;i++){
        QDebug debug = qDebug();
        for (int j = 0; j < size; ++j) {
            debug << matrix[i][j] << "\t";
        }
    }
    qDebug()<<"\n";
}

QList<QPoint> Graph::maxPoint(float **matrix, int size)
{
    QList<QPoint> result;
    result.append( QPoint(0,0));
    float max = -1;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if(matrix[i][j]>max){
                result.clear();
                max = matrix[i][j];
            }
            if(matrix[i][j] == max)

                result.append(QPoint(j,i));
        }
    }
    return result;
}

void Graph::bringingMatrix( Graph::TravellingSalesmanStep *currentStep)
{
    bool isBringin = false;
    //привидение строк
    bool isNeedBreangRow = true;
    bool isNeedBreangColumn = true;
    for(int i=0;i<currentStep->size;i++){
        if(currentStep->bringingRows[i] != infinity)
        {
            isNeedBreangRow = false;
            break;
        }
    }
    if(isNeedBreangRow){
        for(int i=0;i<currentStep->size;i++){
            for(int j=0;j<currentStep->size;j++){
                if(currentStep->bringingRows[i]>currentStep->matrix[j][i])
                {
                    currentStep->bringingRows[i] = currentStep->matrix[j][i];
                    if(currentStep->matrix[j][i] == 0){
                        break;
                    }
                }
            }
        }
        for(int i=0;i<currentStep->size;i++){
            for(int j=0;j<currentStep->size;j++){
                if(currentStep->matrix[j][i] < infinity){
                    currentStep->matrix[j][i]-=  currentStep->bringingRows[i];
                    isBringin = isBringin||(currentStep->bringingRows[i]!=0);
                }
            }
        }
    }
    if(isBringin){
        for(int i=0;i<currentStep->size;i++){
            qDebug()<<"bring row :: "<<currentStep->bringingRows[i];
        }
    }
    isBringin = false;
    //привидение столбцов
    for(int i=0;i<currentStep->size;i++){
        if(currentStep->bringingColumn[i] != infinity)
        {
            isNeedBreangColumn = false;
            break;
        }
    }
    if(isNeedBreangColumn){
        for(int i=0;i<currentStep->size;i++){
            for(int j=0;j<currentStep->size;j++){
                if(currentStep->bringingColumn[i]>currentStep->matrix[i][j])
                {
                    currentStep->bringingColumn[i] = currentStep->matrix[i][j];
                    if(currentStep->matrix[i][j] == 0){
                        break;
                    }
                }
            }
            for(int j=0;j<currentStep->size;j++){
                if(currentStep->matrix[i][j] < infinity){
                    currentStep->matrix[i][j]-=  currentStep->bringingColumn[i];
                    isBringin = (currentStep->bringingColumn[i]!=0) || isBringin;
                }
            }
        }
    }
    if(isBringin){

    }

    for(int i=0;i<currentStep->size;i++){
        if(isNeedBreangColumn)
            currentStep->commonEstimation += currentStep->bringingColumn[i];
        if(isNeedBreangRow)
            currentStep->commonEstimation += currentStep->bringingRows[i];
    }
}

void Graph::estimateZero(Graph::TravellingSalesmanStep *currentStep)
{
    int matrixSize = currentStep->size;
    for(int i=0;i<matrixSize;i++){
        for(int j=0;j<matrixSize;j++){
            if(currentStep->matrix[i][j] == 0){
                float minC = infinity;
                float minR = infinity;
                int minCountC = 1;
                int minCountR = 1;
                for(int d = 0;d<matrixSize;d++)
                {
                    if(d!=j ){
                        if(currentStep->matrix[i][d] < minC ){
                            minC = currentStep->matrix[i][d];
                            minCountC = 1;
                        }
                        else if(currentStep->matrix[i][d] == minC){
                            minCountC ++;
                        }
                    }
                    if(d!=i){
                        if(currentStep->matrix[d][j] < minR){
                            minR = currentStep->matrix[d][j];
                            minCountR = 1;
                        }else if(currentStep->matrix[d][j] == minR){
                            minCountR ++;
                        }
                    }
                }
                currentStep->zeroAlternative[i][j] = minC*minCountC + minR*minCountR;
            }
        }
    }
}

QList<QList<int> *> *Graph::getPathesForTravalinMan(QList<GraphVertex *> *grapjh)
{
    QList<QList<int> *> *result = new QList<QList<int> *>();
    bool *workedEdges = new bool[grapjh->size()];
    for(int i=0;i<grapjh->size();i++)
    {
        workedEdges[i] = false;
    }
    for(int i=0;i<grapjh->size();i++){
        if(workedEdges[i] || (grapjh->at(i)->getChildCount() == 0)) continue;
        workedEdges[i] = true;
        result->append(new QList<int>());
        QList<int> *currentPath = result->last();
        currentPath->append(i);
        GraphVertex *vertex = grapjh->at(i);
        int pos;
        bool isStop = false;
        bool isTowWay = false;
        while(!isStop){
            for(int c=0;c<vertex->getChildCount();c++){
                pos = grapjh->indexOf(vertex->getChildAt(c)->getEndpoint());
                if(!currentPath->contains(pos)){
                    if(isTowWay)
                        currentPath->insert(0,pos);
                    else
                        currentPath->append(pos);
                    workedEdges[pos] = true;
                    vertex = vertex->getChildAt(c)->getEndpoint();
                    break;
                }else if(c==vertex->getChildCount()-1){
                    if(!isTowWay)
                    {
                        vertex = grapjh->at(currentPath->at(0));
                        isTowWay = true;
                        break;
                    }
                    isStop = true;
                }
            }
        }
    }
    for(int i=0;i<result->size();i++){
        QDebug db = qDebug();
        db<<"path::";
        for(int p = 0;p<result->at(i)->size();p++){
            db<<grapjh->at(result->at(i)->at(p))->getName();
        }
    }
    return result;
}

void Graph::selectBestWay(QList<QPoint> const &maxPositions, Graph::TravellingSalesmanStep *currentStep,QList<GraphVertex *> *newGraph)
{
    TravellingSalesmanStep *forckedCurrentStep;
    Connection *con;
    float w;
    QPoint maxPos ;
    for(int i=0;i<maxPositions.size();i++){
        maxPos = maxPositions.at(i);
        forckedCurrentStep = new TravellingSalesmanStep(currentStep->size,currentStep->matrix);
        forckedCurrentStep->rowsNum = currentStep->rowsNum;
        forckedCurrentStep->collumnNum = currentStep->collumnNum;
        forckedCurrentStep->bringingRows = currentStep->bringingRows;
        forckedCurrentStep->bringingColumn = currentStep->bringingColumn;
        forckedCurrentStep->zeroAlternative = currentStep->zeroAlternative;
        forckedCurrentStep->commonEstimation = currentStep->commonEstimation;
        forckedCurrentStep->biggestZero = maxPositions;
        forckedCurrentStep->forks = currentStep->forks;
        forckedCurrentStep->parent = currentStep->parent;

        forckedCurrentStep->selectEdge[0] =forckedCurrentStep->collumnNum[maxPos.x()];
        forckedCurrentStep->selectEdge[1] =forckedCurrentStep->rowsNum[maxPos.y()];

        con = vertexes->at(forckedCurrentStep->selectEdge[0])->getConnectionFor(vertexes->at(forckedCurrentStep->selectEdge[1]));
        if(con != nullptr)
            w = con->getWeigth();
        else
        {
            con = vertexes->at(forckedCurrentStep->selectEdge[1])->getConnectionFor(vertexes->at(forckedCurrentStep->selectEdge[0]));
            w = con->getWeigth();
        }

        newGraph->at( forckedCurrentStep->selectEdge[0] )->addConnection(newGraph->at( forckedCurrentStep->selectEdge[1] ));
        newGraph->at( forckedCurrentStep->selectEdge[1] )->addConnection(newGraph->at( forckedCurrentStep->selectEdge[0] ),w);
        QList<QList<int> *> *pathes = getPathesForTravalinMan(newGraph);

        int begin,end;
        bool isEdgeWasDelet = false;

        forckedCurrentStep->matrix[maxPos.y()][maxPos.x()] = infinity;

        begin = forckedCurrentStep->getColumnPos(forckedCurrentStep->selectEdge[1]);
        end = forckedCurrentStep->getRowPos(forckedCurrentStep->selectEdge[0]);
        if(begin!=-1 && end!=-1){
            forckedCurrentStep->deletedEdges->append(new int[2]{forckedCurrentStep->selectEdge[0],forckedCurrentStep->selectEdge[1]});
            forckedCurrentStep->matrix[begin][end] = infinity;
        }
        newGraph->at( forckedCurrentStep->selectEdge[0] )->removeConnection(newGraph->at( forckedCurrentStep->selectEdge[1] ));
        newGraph->at( forckedCurrentStep->selectEdge[1] )->removeConnection(newGraph->at( forckedCurrentStep->selectEdge[0] ));

        for(int i=0;i<pathes->size();i++){
            for(int p = 0;p<pathes->at(i)->size();p++){
                for(int w = p+1;w<pathes->at(i)->size();w++){
                    begin = forckedCurrentStep->getRowPos(pathes->at(i)->at(p));
                    end = forckedCurrentStep->getColumnPos(pathes->at(i)->at(w));
                    isEdgeWasDelet = false;
                    if(begin!=-1 && end !=-1){
                        if(forckedCurrentStep->matrix[begin][end] != infinity){
                            forckedCurrentStep->matrix[begin][end] = infinity;
                            qDebug()<<"deleting"<<begin<<end <<vertexes->at(pathes->at(i)->at(p))->getName()<<vertexes->at(pathes->at(i)->at(w))->getName();
                            isEdgeWasDelet = true;
                        }
                    }
                    begin = forckedCurrentStep->getRowPos(pathes->at(i)->at(w));
                    end = forckedCurrentStep->getColumnPos(pathes->at(i)->at(p));
                    if(begin!=-1 && end !=-1){
                        if(forckedCurrentStep->matrix[begin][end] != infinity){
                            forckedCurrentStep->matrix[begin][end] = infinity;
                            qDebug()<<"deleting"<<begin<<end <<vertexes->at(pathes->at(i)->at(w))->getName()<<vertexes->at(pathes->at(i)->at(p))->getName();
                            isEdgeWasDelet = true;
                        }
                    }
                    if(isEdgeWasDelet){
                        begin = pathes->at(i)->at(p);
                        end = pathes->at(i)->at(w);
                        int oldB,oldE;
                        for(int d=0;d<forckedCurrentStep->deletedEdges->size();d++){
                            oldB = forckedCurrentStep->deletedEdges->at(d)[0];
                            oldE = forckedCurrentStep->deletedEdges->at(d)[1];
                            if(oldB == begin || oldB == end || oldE == begin || oldE == end){
                                break;
                            }
                            if(d == forckedCurrentStep->deletedEdges->size()-1){
                                forckedCurrentStep->deletedEdges->append(new int[2]{begin,end});
                            }
                        }
                        forckedCurrentStep->deletedEdges->append(new int[2]{pathes->at(i)->at(p),pathes->at(i)->at(w)});
                    }
                }
            }

        }


        TravellingSalesmanStep *nextStep = new TravellingSalesmanStep(forckedCurrentStep->size-1);
        int rowIterator =0;
        int collumnIterator = 0;
        for (int i = 0; i < forckedCurrentStep->size; ++i) {
            if(i != maxPos.y())
            {
                nextStep->rowsNum[rowIterator] = forckedCurrentStep->rowsNum[i];
                rowIterator++;
            }
            if(i!=maxPos.x()){
                nextStep->collumnNum[collumnIterator] = forckedCurrentStep->collumnNum[i];
                collumnIterator++;
            }
        }
        rowIterator =0;
        collumnIterator = 0;
        for (int i = 0; i < forckedCurrentStep->size; ++i) {
            if(i == maxPos.y())
                continue;
            for (int j = 0; j < forckedCurrentStep->size; ++j) {
                if(j == maxPos.x())
                    continue;
                nextStep->matrix[rowIterator][collumnIterator] = forckedCurrentStep->matrix[i][j];
                collumnIterator++;
            }
            rowIterator++;
            collumnIterator = 0;
        }
        forckedCurrentStep->forks->append(nextStep);
        nextStep->commonEstimation = forckedCurrentStep->commonEstimation;
        nextStep->parent = forckedCurrentStep;

        bringingMatrix(nextStep);
        estimateZero(nextStep);
        if(forckedCurrentStep->commonEstimation!=nextStep->commonEstimation){
            float delta = 0;
            for(int i=0;i<nextStep->size;i++){
                delta+=nextStep->bringingRows[i];
                delta+=nextStep->bringingColumn[i];

            }
        }
    }
    float min = currentStep->forks->at(0)->commonEstimation;
    int minCount = 1;
    int minPosition = 0;
    for(int i=1;i<currentStep->forks->size();i++){
        if(currentStep->forks->at(i)->commonEstimation<min){
            min = currentStep->forks->at(i)->commonEstimation;
            minCount=1;
            minPosition = i;
        }else if(currentStep->forks->at(i)->commonEstimation==min)
            minCount++;
    }


    if(minCount>1&&currentStep->size>2){
        Graph::TravellingSalesmanStep *workedStep;
        //        currentStep->selectedFork = -1;
        for(int i=0;i<currentStep->forks->size();i++){
            if(currentStep->forks->at(i)->commonEstimation== min)
            {
                workedStep = currentStep->forks->at(i);

                workedStep->biggestZero = maxPoint(workedStep->zeroAlternative,workedStep->size);
                newGraph->at( workedStep->parent->selectEdge[0] )->addConnection(newGraph->at( workedStep->parent->selectEdge[1] ));
                newGraph->at( workedStep->parent->selectEdge[1] )->addConnection(newGraph->at( workedStep->parent->selectEdge[0] ),w);

                selectBestWay(workedStep->biggestZero,workedStep,newGraph);

                newGraph->at( workedStep->parent->selectEdge[0] )->removeConnection(newGraph->at( workedStep->parent->selectEdge[1] ));
                newGraph->at( workedStep->parent->selectEdge[1] )->removeConnection(newGraph->at( workedStep->parent->selectEdge[0] ));
            }

        }
        min  = 100000;
        minPosition = 0;
        for(int i=0;i<currentStep->forks->size();i++){
            workedStep = currentStep->forks->at(i);
            if(workedStep->selectedFork == -1)
            {
                qDebug()<<"befor removing "<< forckedCurrentStep->forks->size() << forckedCurrentStep->biggestZero.size();
                forckedCurrentStep->forks->removeAt(i);
                forckedCurrentStep->biggestZero.removeAt(i);
                currentStep->biggestZero.removeAt(i);
                qDebug()<<"after removing "<< forckedCurrentStep->forks->size() << forckedCurrentStep->biggestZero.size();
                i--;
                continue;
            }
            if(workedStep->forks->at(workedStep->selectedFork)->commonEstimation < min)
            {
                min = workedStep->forks->at(workedStep->selectedFork)->commonEstimation;
                minPosition =i;
            }
        }
        workedStep = currentStep->forks->at(minPosition);
        TravellingSalesmanStep * currentFork =workedStep->forks->at(workedStep->selectedFork)->parent;
        (*currentStep->forks)[minPosition] =currentFork;
        currentStep->selectedFork = minPosition;
        qDebug()<<"more then one alternatives";
    }
    if(currentStep->size <=2){
        minPosition =0;
    }
    currentStep->selectedFork = minPosition;
    currentStep->forks->at(minPosition)->parent->selectedFork = minPosition;
}

float **Graph::removeInfinityCollumn(float **source, int &size)
{
    int xOffset = 0;
    int yOffset = 0;
    int xEndOffset = size;
    int yEndOffset = size;
    bool isNullRows, isNullCollumn;
    for(int i=0;i<size;i++){
        isNullCollumn = isNullRows = true;
        for(int j=0;j<size;j++){
            isNullCollumn = ((source[j][i] >= infinity) && isNullCollumn);
            isNullRows = ((source[i][j] >=infinity)&&isNullRows);
        }
        if(isNullCollumn){
            if(i ==xOffset)
                xOffset ++;
            else
                xEndOffset --;
        }
        if(isNullRows){
            if(i == yOffset)
                yOffset ++;
            else
                yEndOffset --;
        }
    }
    size = qMax(yEndOffset-yOffset,xEndOffset - xOffset);
    float **result = new float*[size];
    for(int i=0;i<size;i++)
        result[i] = new float[size];

    for(int i=0;i<yEndOffset - yOffset;i++){
        for(int j=0;j<xEndOffset-xOffset;j++){
            result[i][j]  = source[i+yOffset][j+xOffset];
        }
    }
    return result;
}

void Graph::bringByRow(float **matrix, float *koef, int size)
{
    for(int i=0;i<size;i++){
        koef[i] = infinity;
        for(int j=0;j<size;j++){
            if(matrix[i][j]<koef[i])
                koef[i] = matrix[i][j];
        }
        for(int j=0;j<size;j++){
            if(matrix[i][j]<infinity)
                matrix[i][j] -= koef[i];
        }
    }
}

void Graph::bringByCollumn(float **matrix, float *koef, int size)
{
    for(int i=0;i<size;i++){
        koef[i] = infinity;
        for(int j=0;j<size;j++){
            if(matrix[j][i]<koef[i])
                koef[i] = matrix[j][i];
        }
        for(int j=0;j<size;j++){
            if(matrix[j][i]<infinity)
                matrix[j][i] -= koef[i];
        }
    }
}

bool Graph::egervary(float **matrix, int *minPos,int *selectedZero
                     ,int *checkZeroRow,int *checkZeroCollumn, int size)
{
    QList<EvengaryBestZero *> switchingItem;
    EvengaryBestZero *bestResul = nullptr;

    switchingItem.append(new EvengaryBestZero(matrix,size));
    bestResul = switchingItem.first();
    EvengaryBestZero *currentItem;
    bool isFindZero = false;
    while (switchingItem.size()) {
        currentItem = switchingItem.first();
        switchingItem.removeFirst();
        isFindZero = false;
        if(currentItem->startRow >= currentItem->size){
            if(currentItem->lenth>bestResul->lenth)
                bestResul = currentItem;
            continue;
        }
        for(int r = currentItem->startRow;r<currentItem->size;r++){
            for(int i=0;i<size;i++){
                if(currentItem->matrix[r][i] == 0){
                    if(!currentItem->selectedCollumn[i]){
                        isFindZero = true;
                        switchingItem.append(new EvengaryBestZero(matrix,size,currentItem->lenth+1,r+1,currentItem,currentItem->selectedCollumn));
                        switchingItem.last()->selectedCollumn[i] = true;
                        switchingItem.last()->selectedZero = i;
                    }
                }
            }
            if(r>=currentItem->size-1){
                if(currentItem->lenth>bestResul->lenth)
                    bestResul = currentItem;
            }
        }
        //        }

    }
    int bestLenth = bestResul->lenth;
    while(bestResul !=nullptr ){
        if(bestResul->startRow>=1)
            selectedZero[bestResul->startRow-1] = bestResul->selectedZero;
        bestResul = bestResul->parent;
    }
    if(bestLenth==size){
        return false;
    }
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            checkZeroRow[i] += (matrix[i][j] == 0);
            checkZeroCollumn[i]+=(matrix[j][i] ==0);
        }
    }
    for(int i=0;i<size;i++){
        for(int j=0;j<size;j++){
            if(checkZeroRow[i]||checkZeroCollumn[j])
                continue;
            if(matrix[i][j] == 0){
                checkZeroCollumn[j] ++;
            }
        }
    }

    int min = infinity;
    for(int i=0;i<size;i++){
        for (int j = 0; j < size; ++j) {
            if(checkZeroRow[i]||checkZeroCollumn[j])
                continue;
            if(min>matrix[i][j])
            {
                min = matrix[i][j];
                minPos[0] = i;
                minPos[1] = j;
            }
        }
    }
    for(int i=0;i<size;i++){
        for (int j = 0; j < size; ++j) {
            if(checkZeroRow[i]>0&&checkZeroCollumn[j]>0){
                matrix[i][j]+=min;
            }
            if(checkZeroRow[i]||checkZeroCollumn[j])
                continue;
            if(matrix[i][j]<infinity)
                matrix[i][j]-=min;
        }
    }

    //    float **matrix, int *collumnZero, int *rowZero, int *minPos, int size)

    return true;
}



QList<float **> *Graph::floid(int inf)
{
    QList<float **>  *result = new QList<float **> ();
    float **matrix = toMatrix(inf);

    for(int i=0;i<vertexes->size();i++){
        matrix[i][i] = 0;
    }

    result->append(copy(matrix,vertexes->size()));
    for(int k=0;k<vertexes->size();k++){
        for(int i=0;i<vertexes->size();i++){
            for(int j=0;j<vertexes->size();j++){
                result->last()[i][j] =  qMin(result->last()[i][j], result->last()[i][k] + result->last()[k][j]);
            }
        }
        result->append(copy(result->last(),vertexes->size()));
    }
    return result;
}

QString Graph::siralizeToString()
{
    QString str = QString::number(vertexes->size()) + "\n";
    for(int i=0;i< vertexes->size();i++){
        str+= vertexes->at(i)->getName();
        str+="\t";
        str += QString::number(vertexes->at(i)->getPos()->x());
        str+="\t";
        str += QString::number(vertexes->at(i)->getPos()->y());
        str +="\n";
    }
    for(int i=0;i<  vertexes->size();i++){
        GraphVertex *v =  vertexes->at(i);
        str+=QString::number(i)+":";
        for(int c = 0;c<v->getChildCount();c++){
            Connection *con = v->getChildAt(c);
            str+=QString::number(vertexes->indexOf(con->getEndpoint()));
            str+="\t";
            str+=QString::number(con->getWeigth());
            str+=" ";
        }
        str+="\n";
    }
    return str;
}

float **Graph::toMatrix(bool isSimetric, int inf)
{
    float **matrix = new float*[vertexes->size()];
    for(int i=0;i<vertexes->size();i++){
        matrix[i] = new float[vertexes->size()];
        for(int j=0;j<vertexes->size();j++){
            matrix[i][j] = inf;
        }
    }
    for(int i=0;i<vertexes->size();i++){
        for(int c =0;c<vertexes->at(i)->getChildCount();c++){
            Connection *con = vertexes->at(i)->getChildAt(c);
            int pos = vertexes->indexOf(con->getEndpoint());
            matrix[i][pos] = con->getWeigth();
            if(isSimetric)
                matrix[pos][i] = con->getWeigth();
        }
    }
    return matrix;
}

Graph::PrimaKraskelResult *Graph::primaKraskel(QList<Graph::EdgesByLenth *> *edges)
{
    Graph::PrimaKraskelResult * result = new Graph::PrimaKraskelResult();
    for(int i=0;i<vertexes->size();i++)
        vertexes->at(i)->setValue(NULL);
    int lastColor = 1,color;
    for(int i=0;i<edges->size();i++){
        for(int e =0;e<edges->at(i)->edges->size();e++){
            Connection *con = edges->at(i)->edges->at(e);
            myNum *start =(myNum *)con->getParent()->getValue();
            myNum *end =(myNum *)con->getEndpoint()->getValue();

            if((!start) && (!end))
            {
                result->steps->append(new PrimaKraskelStep(con,-1,-1));
                con->getParent()->setValue(new myNum(lastColor));
                con->getEndpoint()->setValue(new myNum(lastColor));
                result->steps->last()->finalColor = lastColor;
                lastColor ++ ;
                continue;
            }else
                if(!start && end){
                    con->getParent()->setValue(new myNum(end->value));
                    result->steps->append(new PrimaKraskelStep(con,-1,end->value));
                    result->steps->last()->finalColor = end->value;
                    continue;
                }else
                    if(!end &&  start){
                        con->getEndpoint()->setValue(new myNum(start->value));
                        result->steps->append(new PrimaKraskelStep(con,start->value,-1));
                        result->steps->last()->finalColor = start->value;
                        continue;
                    }else
                        if(start->value!=end->value){
                            color = qMin(start->value,end->value);
                            int oldColor  = qMax(start->value,end->value);
                            for(int v=0;v<vertexes->size();v++){
                                myNum *vValue = (myNum *)vertexes->at(v)->getValue();
                                if(vValue){
                                    if(vValue->value == oldColor)
                                    {
                                        vertexes->at(v)->setValue(new myNum(color));
                                    }
                                }
                            }
                            PrimaKraskelStep *s = new PrimaKraskelStep(con,start->value,end->value);
                            s->finalColor = color;
                            result->steps->append(s);
                            con->getParent()->setValue(new myNum(color));
                            con->getEndpoint()->setValue(new myNum(color));
                            delete start;
                            delete end;
                        }
        }
    }
    for(int i=0;i<vertexes->size();i++){
        result->graph->append(vertexes->at(i)->copy());
    }
    int parentPos,endpointPos;
    for(int i =0;i<result->steps->size();i++){
        Connection *edge = result->steps->at(i)->edge;
        if(((myNum*)edge->getEndpoint()->getValue())->value!=1 || ((myNum*)edge->getParent()->getValue())->value!=1)
            continue;
        parentPos = vertexes->indexOf(edge->getParent());
        endpointPos = vertexes->indexOf(edge->getEndpoint());
        result->graph->at(parentPos)->addConnection(result->graph->at(endpointPos),edge->getWeigth());
    }
    return result;
}



Graph::TravellingSalesmanStep::TravellingSalesmanStep(int size, float **matrix, int *rowsNum, int *collumNum){
    this->size = size;
    if(matrix == nullptr){
        this->matrix = new float*[size];
        for (int i = 0; i < size; ++i) {
            this->matrix[i]= new float[size];
        }
    }else{
        this->matrix = copy(matrix,size);
    }
    this->zeroAlternative = new float*[size];
    for(int i=0;i<size;i++){
        zeroAlternative[i] = new float[size];
        for(int j=0;j<size;j++){
            zeroAlternative[i][j] = -1;
        }
    }
    if(rowsNum == nullptr){
        this->rowsNum = new int[size];
        for(int i=0;i<size;i++)
            this->rowsNum[i] = i;
    }else{
        this->rowsNum = rowsNum;
    }
    if(collumNum== nullptr){
        this->collumnNum = new int[size];
        for(int i=0;i<size;i++)
            this->collumnNum[i] = i;
    }else{
        this->collumnNum = collumNum;
    }
    bringingRows = new float[size];
    bringingColumn = new float[size];
    for(int i=0;i<size;i++){
        bringingRows[i] = infinity;
        bringingColumn[i] = infinity;
    }
    selectEdge = new int[2]{-1,-1};
    forks = new QList<TravellingSalesmanStep *>();
    parent = nullptr;
    selectedFork = -1;
    deletedEdges = new QList<int*>();
}

bool Graph::TravellingSalesmanStep::isBringedByRow()
{
    for(int i=0;i<bringingRows[i];i++)
        if(bringingRows[i]!=0) return true;
    return false;
}

bool Graph::TravellingSalesmanStep::isBringedByCollumn()
{
    for(int i=0;i<bringingColumn[i];i++)
        if(bringingColumn[i]!=0) return true;
    return false;
}
