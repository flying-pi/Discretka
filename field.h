#ifndef FIELD_H
#define FIELD_H

#include <QWidget>
#include <QList>
#include "./graph/graphvertex.h"
#include <QPoint>
#include <QPointF>
#include "const.h"

namespace Ui {
class Field;
}

enum class FieldMode
{
    ADD_NEW_ITEM_MODE,
    SELECT_ITEM_MODE
};

class Field : public QWidget
{
    Q_OBJECT

public:
    explicit Field(QWidget *parent = 0);
    ~Field();
    void setMode(FieldMode mode);

    QList<GraphVertex *>  *getGraph(){
        return allItem;
    }
    void setGraph( QList<GraphVertex *>  * graph);

private:
    Ui::Field *ui;
    QList<GraphVertex *> *allItem;
    QPainter *painter = NULL;

signals:
    void onPointClicked(GraphVertex *point);
    void onPointSelect(GraphVertex *point);
    void onCancelSelection();

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);

    FieldMode mode = FieldMode::ADD_NEW_ITEM_MODE;
};

#endif // FIELD_H
