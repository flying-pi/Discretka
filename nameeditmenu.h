#ifndef NAMEEDITMENU_H
#define NAMEEDITMENU_H

#include <QWidget>
#include "./graph/graphvertex.h"

namespace Ui {
class NameEditMenu;
}

class NameEditMenu : public QWidget
{
    Q_OBJECT

public:
    explicit NameEditMenu(QWidget *parent = 0,GraphVertex *vertex=0);
    ~NameEditMenu();

private:
    Ui::NameEditMenu *ui;
    GraphVertex *vertex = NULL;

private slots:
    void onTextChanged();
};

#endif // NAMEEDITMENU_H
