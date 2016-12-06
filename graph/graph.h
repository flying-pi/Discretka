#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include "graphvertex.h"
#include <QList>


class Graph : public QObject
{
    Q_OBJECT
public:
    struct EdgesByLenth{
        float weight = 0;
        QList<Connection *> *edges = new QList<Connection *>();
    };

    enum class ColorintType{
        Parent,All,Endrpoint
    };
    struct PrimaKraskelStep{

        PrimaKraskelStep(Connection *edge,int parentColor,int endpointColor){
            this->edge = edge;
            this->parentColor = parentColor;
            this->endpointColor = endpointColor;

        }

        Connection *edge;
        int parentColor,endpointColor,finalColor;
    };

    struct TravellingSalesmanStep{
        TravellingSalesmanStep(int size,float **matrix = nullptr,int *rowsNum = nullptr,int *collumNum = nullptr);

        TravellingSalesmanStep *parent;
        int size;
        int *rowsNum;
        int *collumnNum;
        float *bringingRows;
        float *bringingColumn;
        float **matrix;
        float **zeroAlternative;
        int *selectEdge;
        float commonEstimation =0;
        int selectedFork;
        QList<int *> *deletedEdges;
        bool isBringedByRow();
        bool isBringedByCollumn();

        QList<QPoint> biggestZero;
        QList<TravellingSalesmanStep *> *forks;
        int getColumnPos(int pos){
            for(int i=0;i<size;i++)
            {
                if(collumnNum[i] == pos)
                    return i;
            }
            return -1;
        }
        int getRowPos(int pos){
            for(int i=0;i<size;i++)
            {
                if(rowsNum[i] == pos)
                    return i;
            }
            return -1;
        }

    };

    struct PrimaKraskelResult{
        QList<GraphVertex *> *graph = new QList<GraphVertex *>();
        QList<PrimaKraskelStep *> *steps = new QList<PrimaKraskelStep *>();
    };

    class myNum : public QObject{
    public:
        myNum(int v){
            value = v;
        }

        int value;
    };

    enum class VengerStepType{
        BASIC,BRINING,EGERVARY
    };

    struct VengerStep{
        VengerStep(){
            type  = VengerStepType::BASIC;
        }

        VengerStepType type ;
        float **result;
        int size=0;
    };

    struct VengerStepBringing : VengerStep{
        VengerStepBringing(float **input,int size){
            this->result = copy(input,size);
            this->size = size;
            koef = new float[size];
            type  = VengerStepType::BRINING;
        }

        bool isByCollumn;
        float *koef;
        float kSum =0;
    };
    struct VengerStepEgervary : VengerStep{
        VengerStepEgervary(float **matrix, int size){
            this->result = copy(matrix,size);
            this->sourceMatrix = copy(matrix,size);
            this->size = size;
            selectedZeroInRows = new int[size];
            checkZeroCollumn = new int[size];
            checkZeroRow = new int[size];
            for(int i=0;i<size;i++)
            {
                selectedZeroInRows[i] = -1;
                checkZeroCollumn[i] = -1;
                checkZeroRow[i] = -1;
            }
            type  = VengerStepType::EGERVARY;
            isFinal = false;
        }

        int *selectedZeroInRows;
        int minValuePos[2];
        float **sourceMatrix;
        bool isFinal;
        int *checkZeroCollumn;
        int *checkZeroRow;
    };

    struct EvengaryBestZero{
        EvengaryBestZero(float **matrix, int size,int lenth=0,int startRow=0,EvengaryBestZero *parent = nullptr,bool *selectedCollumn = nullptr){
            this->matrix = matrix;
            this->size = size;
            this->selectedCollumn = new bool[size];
            for(int i=0;i<size;i++)
                this->selectedCollumn[i] = (selectedCollumn == nullptr?false:selectedCollumn[i]);
            this->lenth = lenth;
            this->parent = parent;
            this->startRow = startRow;
            selectedZero = -1;
        }

        bool *selectedCollumn;
        float **matrix;
        int size;
        int lenth;
        int startRow;
        EvengaryBestZero *parent;
        int selectedZero;
    };

    explicit Graph(QObject *parent = 0,QList<GraphVertex *> *graph = new QList<GraphVertex *>());
    explicit Graph(QObject *parent = 0,QString str="");
    ~Graph();

    QList<Connection *> *getAllEdges();
    QList<EdgesByLenth *> *getSortedEdges();
    QList<GraphVertex *> *getAsList(){   return vertexes;}
    PrimaKraskelResult *primaKraskel(QList<Graph::EdgesByLenth *> *);
    PrimaKraskelResult *primaKraskel();
    TravellingSalesmanStep *TravellingSalesmanAlgaritm();
    QList<VengerStep *> *VengerAlgoritm();



    /**
     * @brief floid
     * @param infinity пареметр который будет использоваться для опеределения бесконечности
     * @return
     */
    QList<float **> *floid(int inf = infinity);

    QString siralizeToString();

signals:

public slots:

protected :
    QList<GraphVertex *> *vertexes;
    QList<Connection *> *edges = NULL;
    static const int infinity = 1000;

    float **toMatrix(bool isSimetric = false,int inf = infinity);
    static float **copy(float **source,int size);
    QStringList matrixToString(float **matrix,int size);
    void printToDebufMatrix(float ** matrix,int size);
    QList<QPoint> maxPoint(float **matrix,int size);

    //коммивояжор
    void bringingMatrix(TravellingSalesmanStep *currentStep);
    void estimateZero(TravellingSalesmanStep *currentStep);
    QList<QList<int> *> * getPathesForTravalinMan(QList<GraphVertex *> *grapjh);
    void selectBestWay(QList<QPoint>const &bestZeroPoint,TravellingSalesmanStep *currentStep,QList<GraphVertex *> *newGraph);

    //назначения
    float **removeInfinityCollumn(float **source,int &size);
    void bringByRow(float **matrix, float *koef,int size);
    void bringByCollumn(float **matrix, float *koef,int size);
    /**
     * @brief egervary приводить матрицю за алгоритмом Єгерварія
     * @param matrix матриця для роботи
     * @param collumnZero імена колонок в яких є нулі
     * @param rowZero імена строк в яких є нулі
     * @param minPos массив з 2 елементів для зберігання позиції мінімального елементу
     * @param size розмір матриці
     */
    bool egervary(float **matrix, int *minPos, int *selectedZero, int *checkZeroRow, int *checkZeroCollumn, int size);
};

#endif // GRAPH_H
