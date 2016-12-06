#ifndef GRAPHVERTEX_H
#define GRAPHVERTEX_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QString>

class GraphVertex;

class Connection : public QObject{
        Q_OBJECT

public:
    Connection(GraphVertex *parent,GraphVertex *endpoint,float weight );

    inline GraphVertex * getParent(){return parent;}
    inline GraphVertex * getEndpoint(){return endpoint;}
    inline float getWeigth(){return weight;}
    inline void setWeigth(float w){weight = w;}
private:
    GraphVertex *parent = NULL;
    GraphVertex *endpoint = NULL;
    float weight =  0;
};

class GraphVertex : public QObject
{
    Q_OBJECT
public:
    explicit GraphVertex(QObject *parent = 0);
    explicit GraphVertex(QObject *parent = 0,double xPos = 0,double yPos = 0);
    virtual ~GraphVertex();
    void addConnection(GraphVertex *item, float w=0);
    int getChildCount();
    Connection *getChildAt(int pos);
    Connection *getLastConnection();
    QPointF *getPos();

    void setName(QString name);
    QString getName();
    void removeConnection(Connection *connection);
    void removeConnection(GraphVertex *connection);
    void setValue(QObject *value){
        this->value= value;
    }
    QObject * getValue(){
        return value;
    }
    GraphVertex *copy();
    Connection *getConnectionFor(GraphVertex *vertex);



signals:

public slots:

protected:
    QList<Connection *> *connection;
    QPointF *pos;
    QString name;
    QObject *value = NULL;

};

#endif // GRAPHVERTEX_H
