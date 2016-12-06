#include "graphvertex.h"
#include <QDebug>

GraphVertex::GraphVertex(QObject *parent) : QObject(parent)
{
    connection = new QList<Connection *>();
    pos = NULL;
    name = "";
}

GraphVertex::GraphVertex(QObject *parent, double xPos, double yPos): QObject(parent)
{
    connection = new QList<Connection *>();
    pos = new QPointF(xPos,yPos);
}

GraphVertex::~GraphVertex()
{
    for(int i=0;i<connection->size();i++) delete connection->at(i);
    delete connection;
}

void GraphVertex::addConnection(GraphVertex *item, float w )
{
    Connection *con = new Connection(this,item,w);
    connection->append(con);
}

int GraphVertex::getChildCount()
{
    return connection->size();
}

Connection *GraphVertex::getChildAt(int pos)
{
    return connection->at(pos);
}

Connection *GraphVertex::getLastConnection()
{
    if(connection->size() == 0)
        return NULL;
    return connection->last();
}

QPointF *GraphVertex::getPos()
{
    return pos;
}

void GraphVertex::setName(QString name)
{
    this->name = name;
}

QString GraphVertex::getName()
{
    return name;
}

void GraphVertex::removeConnection(Connection *connection)
{
    for(int i=0;i<this->connection->size();i++){
        if(this->connection->at(i) == connection){
            this->connection->removeAt(i);
            return;
        }
    }
}

void GraphVertex::removeConnection(GraphVertex *connection)
{
    for(int i=0;i<this->connection->size();i++){
        if(this->connection->at(i)->getEndpoint() == connection){
            this->connection->removeAt(i);
            return;
        }
    }
}

GraphVertex *GraphVertex::copy()
{
    GraphVertex *result = new GraphVertex(parent(),pos->x(),pos->y());
    result->setName(name);
    result->setValue(value);
    return result;
}

Connection *GraphVertex::getConnectionFor(GraphVertex *vertex)
{
    for(int i=0;i<connection->size();i++){
        if(connection->at(i)->getEndpoint() == vertex)
            return connection->at(i);
    }
    return nullptr;
}

Connection::Connection(GraphVertex *parent, GraphVertex *endpoint, float weight)
{
    this->parent = parent;
    this->endpoint = endpoint;
    this->weight = weight;
}
