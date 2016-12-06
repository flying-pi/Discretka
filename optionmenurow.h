#ifndef OPTIONMENUROW_H
#define OPTIONMENUROW_H

#include <QWidget>
#include "./graph/graphvertex.h"

namespace Ui {
class OptionMenuRow;
}

class OptionMenuRow : public QWidget
{
    Q_OBJECT

public:
    explicit OptionMenuRow(QWidget *parent = 0,Connection *con = NULL);

    ~OptionMenuRow();

private:
    Ui::OptionMenuRow *ui;
    Connection *item;

private slots:
    void onTextChanged();
    void onCloseBtnPress();
};

#endif // OPTIONMENUROW_H
