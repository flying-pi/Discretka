#include "optionmenurow.h"
#include "ui_optionmenurow.h"
#include <QString>

OptionMenuRow::OptionMenuRow(QWidget *parent,Connection *con) :
    QWidget(parent),
    ui(new Ui::OptionMenuRow)
{
    ui->setupUi(this);
    this->item = con;
    if(item){
        ui->pointName->setText(item->getEndpoint()->getName());
        ui->weidth->setText(QString::number(item->getWeigth()));
        connect(ui->weidth,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
        connect(ui->closeBtn,SIGNAL(clicked(bool)),this,SLOT(onCloseBtnPress()));
    }
}

OptionMenuRow::~OptionMenuRow()
{
    delete ui;
}

void OptionMenuRow::onTextChanged()
{
    QString str = ui->weidth->toPlainText();
    if(str.size() == 0){
        str = "0";
        ui->weidth->setPlainText(str);
        return;
    }
    for(int i=0;i<str.length();i++){
        if(str.at(i)<'0'||str.at(i)>'9')
        {
            str.remove(i,1);
            i--;
            ui->weidth->setPlainText(str);
            return;
        }
    }

    item->setWeigth(str.toFloat());
    QWidget *w = this;
    while (w->parentWidget()) {
        w = w->parentWidget();
    }
    w->update();
}

void OptionMenuRow::onCloseBtnPress()
{
    item->getParent()->removeConnection(item);
    QWidget *w = this;
    while (w->parentWidget()) {
        w = w->parentWidget();
    }
    w->update();
}
