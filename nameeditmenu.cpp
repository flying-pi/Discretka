#include "nameeditmenu.h"
#include "ui_nameeditmenu.h"

NameEditMenu::NameEditMenu(QWidget *parent,GraphVertex *vertex) :
    QWidget(parent),
    ui(new Ui::NameEditMenu)
{
    ui->setupUi(this);
    if(vertex){
        this->vertex = vertex;
        ui->pointName->setText(vertex->getName());
        connect(ui->pointName,SIGNAL(textChanged()),this,SLOT(onTextChanged()));
    }
}

NameEditMenu::~NameEditMenu()
{
    delete ui;
}

void NameEditMenu::onTextChanged()
{
    if(vertex)
    {
        vertex->setName(ui->pointName->toPlainText().trimmed());
    }
    QWidget *w = this;
    while (w->parentWidget()) {
        w = w->parentWidget();
    }
    w->update();
}
