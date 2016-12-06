#include "discretcaprinter.h"
#include <QFont>
#include <QPrinter>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextTable>
#include <QTextTableCell>
#include <QDebug>
#include <QTextDocumentWriter>
#include "./graph/graph.h"
#include "printutil.h"


DiscretcaPrinter::DiscretcaPrinter(QObject *parent, QList<GraphVertex *> *graph) : QObject(parent)
{
    setGraph(graph);
    printer = new QPrinter(QPrinter::PrinterResolution);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setPaperSize(QPrinter::A4);
    printer->setPageMargins(20, 20, 10, 20, QPrinter::Millimeter);

    doc = new QTextDocument;
    doc->setPageSize(printer->pageRect().size());

    cursor = new QTextCursor(doc);
    taskNum = 1;
    imageNum = 1;
    txtformat = QTextCharFormat();
     lableFont = QFont("Times New Roman",  12, QFont::Light);
     normalFont = QFont("Times New Roman",  14, QFont::Normal);
     titleFont = QFont("Times New Roman", 14, QFont::Bold);
}

void DiscretcaPrinter::print()
{


    printPrimaKraskel();
    TravellingSalesman();
    Venger();
}

void DiscretcaPrinter::printFloyd(QString filePath)
{
    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName(filePath);
    printer.setPageMargins(QMarginsF(30, 30, 30, 30));

    QTextDocument doc;
    doc.setPageSize(printer.pageRect().size());

    QTextCursor* cursor = new QTextCursor(&doc);

    printFloyd(cursor,printer);

    doc.print(&printer);
}

QString numOrInf(float num){
    if(num>100) return "∞";
    return QString::number(num);
}

void DiscretcaPrinter::printFloyd(QTextCursor *cursor, QPrinter &printer)
{

    QTextCharFormat txtformat = QTextCharFormat();
    QFont lableFont("Times New Roman",  12, QFont::Light);
    txtformat.setFont(lableFont);
    QList<float **> *result = graph->floid(1000);
    int size=graph->getAsList()->size();
    QImage *image  = generateImageFromMatrix(size,size,result->at(0),1000);
    cursor->insertImage((*image));

    for(int k=1;k<result->size();k++){
        for(int i=0;i<size;i++){
            if(i == k-1) continue;
            for(int j=i+1;j<size;j++){
                if(i == k-1) continue;
                if(result->at(k)[i][j]<100){
                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText("\n",txtformat);
                    insertFlodIndex(cursor,txtformat,k,i+1,j+1);
                    // result->last()[i][j] =  qMin(result->last()[i][j], result->last()[i][k] + result->last()[k][j]);

                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText("= min {",txtformat);

                    insertFlodIndex(cursor,txtformat,k-1,i+1,k);
                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText(" + ",txtformat);
                    insertFlodIndex(cursor,txtformat,k-1,k,j+1);

                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText("; ",txtformat);

                    insertFlodIndex(cursor,txtformat,k-1,i+1,j+1);

                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText("}",txtformat);

                    QString d1 = numOrInf(result->at(k-1)[i][k-1]);
                    QString d2 = numOrInf(result->at(k-1)[k-1][j]);
                    QString d3 = numOrInf(result->at(k-1)[i][j]);
                    QString d4 = numOrInf(result->at(k)[i][j]);
                    cursor->insertText(" = {"+d1+"+"+d2+";"+d3+"} = "+d4);
                }

            }
        }

        cursor->insertText("\n");
        for(int i=0;i<size;i++){
            if(i == k-1) continue;
            for(int j=i+1;j<size;j++){
                if(i == k-1) continue;
                if(result->at(k)[i][j]>100){
                    insertFlodIndex(cursor,txtformat,k,i+1,j+1);
                    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
                    cursor->insertText(" = ",txtformat);
                }
            }
        }
        txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
        cursor->insertText("∞",txtformat);
        cursor->insertText("\n");
        image  = generateImageFromMatrix(size,size,result->at(k),1000);
        cursor->insertImage((*image));
    }
}

void DiscretcaPrinter::insertFlodIndex(QTextCursor *cursor, QTextCharFormat &txtformat, int top, int midle1, int midle2)
{
    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
    cursor->insertText("d",txtformat);

    txtformat.setVerticalAlignment(QTextCharFormat::AlignSuperScript);
    cursor->insertText(QString::number(top),txtformat);

    txtformat.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    cursor->insertText(QString::number(midle1),txtformat);
    cursor->insertText(";",txtformat);
    cursor->insertText(QString::number(midle2),txtformat);
}

void DiscretcaPrinter::insertVertext(QTextCursor *cursor, QTextCharFormat &txtformat, QString name)
{
    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
    cursor->insertText(" V",txtformat);
    txtformat.setVerticalAlignment(QTextCharFormat::AlignSubScript);
    cursor->insertText(name,txtformat);
    txtformat.setVerticalAlignment(QTextCharFormat::AlignNormal);
}

void DiscretcaPrinter::prepareToPrint()
{
    QTextFrame * root = doc->rootFrame();
    QTextFrame::iterator it;
    for( it = root->begin(); !(it.atEnd()); ++it )
    {
        QTextBlock block = it.currentBlock();
        block.blockFormat().setLineHeight(21, QTextBlockFormat::LineDistanceHeight);
    }
}

void DiscretcaPrinter::setGraph(QList<GraphVertex *> *graph)
{
    if(graph)
        this->graph = new Graph(this,graph);
}

void DiscretcaPrinter::saveToFile(QString filePath)
{
 prepareToPrint();
    printer->setOutputFileName(filePath);
    printer->setOutputFormat(QPrinter::PdfFormat);
    doc->print(printer);
}

void DiscretcaPrinter::saveToODTFile(QString filePath)
{
    prepareToPrint();
    QTextDocumentWriter writer(filePath);
    writer.setFormat("ODF");
    writer.write(doc);
}

QTextDocument *DiscretcaPrinter::getDoc()
{
    return doc;
}


void DiscretcaPrinter::printPrimaKraskel()
{

    txtformat.setFont(titleFont);
    cursor->insertText(QString::number(taskNum++),txtformat);
    cursor->insertText(". Пошук остовного дерева алгоритмом Прима-Краскела", txtformat);

    QTextBlockFormat centerFormat;
    //    centerFormat.setAlignment(Qt::AlignHCenter);
    cursor->insertBlock(centerFormat);
    QImage *image = drawGraph(graph->getAsList(),printer->pageRect().width());
    cursor->insertImage((*image));

    centerFormat = QTextBlockFormat();
    centerFormat.setAlignment(Qt::AlignLeft);
    cursor->insertBlock(centerFormat);
    txtformat.setFont(lableFont);
    cursor->insertText("Рисунок ",txtformat);
    cursor->insertText(QString::number(imageNum++));
    cursor->insertText(". Вихидний граф для розрахунків",txtformat);
    cursor->insertBlock();
    cursor->insertBlock();

    txtformat.setFont(normalFont);
    cursor->insertText("Виписуємо таблицю ваг ребер,  впорядковану за спаданням.",txtformat);

    txtformat.setFont(lableFont);
    cursor->movePosition(QTextCursor::End);
    cursor->insertBlock();

    QList<Graph::EdgesByLenth *> * edges = graph->getSortedEdges();
    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(4);
    tableFormat.setCellSpacing(-1);
    tableFormat.setBorderBrush(QBrush(Qt::SolidPattern));
    tableFormat.setBorder(1);
    tableFormat.setBorderStyle(QTextTableFormat::BorderStyle_Solid);
    QTextTable *table = cursor->insertTable(edges->size()+1,2,tableFormat);
    table->cellAt(0,0).firstCursorPosition().insertText("Вага");
    table->cellAt(0,1).firstCursorPosition().insertText("Ребра");
    for(int i=0;i<edges->size();i++){
        Graph::EdgesByLenth *edge = edges->at(i);
        table->cellAt(i+1,0).firstCursorPosition().insertText(QString::number(edge->weight)+"      ");
        QString value = "";
        for(int e = 0;e<edge->edges->size();e++){
            value += "("+edge->edges->at(e)->getParent()->getName()+";"
                    +edge->edges->at(e)->getEndpoint()->getName()+") ";
        }
        table->cellAt(i+1,1).firstCursorPosition().insertText(value);
    }


    txtformat.setFont(normalFont);
    cursor->movePosition(QTextCursor::End);
    cursor->insertBlock();
    txtformat.setFont(lableFont);
    cursor->insertText("Таблиця 1 - ваги ребер графа",txtformat);
    cursor->insertBlock();
    cursor->insertBlock();
    txtformat.setFont(normalFont);
    cursor->insertText("Послідовність кроків по вирішенню завдання Прима-Краскела:",txtformat);
    cursor->insertBlock();

    Graph::PrimaKraskelResult * primaKreskelResult =graph->primaKraskel(edges);
    QList<Graph::PrimaKraskelStep *> *steps = primaKreskelResult->steps;
    float weigth = 0;
    for(int i=0;i<steps->size();i++){
        Graph::PrimaKraskelStep *step = steps->at(i);
        weigth += step->edge->getWeigth();
        cursor->insertText(QString::number(i+1));
        QString fromName = step->edge->getParent()->getName();
        QString toName = step->edge->getEndpoint()->getName();
        cursor->insertText(QString(") обираємо ребро (%1,%2) ").arg(fromName,toName),txtformat);
        if(step->parentColor == -1 && step->endpointColor == -1){
            cursor->insertText("обидві вершини безбарвні, тож фарбуємо ",txtformat);
            insertVertext(cursor,txtformat,fromName);
            cursor->insertText(" і ",txtformat);
            insertVertext(cursor,txtformat,toName);
            cursor->insertText(QString(" в колір %1").arg(QString::number(step->finalColor)),txtformat);
        }else
            if(step->parentColor != -1 || step->endpointColor != -1)
            {
                cursor->insertText("вершина",txtformat);
                insertVertext(cursor,txtformat,(step->parentColor != -1)?fromName:toName);
                cursor->insertText(" має колір "+
                                   QString::number((step->parentColor != -1)?step->parentColor:step->endpointColor),txtformat);
                cursor->insertText(", a вершина",txtformat);
                insertVertext(cursor,txtformat,(step->parentColor != -1)?toName:fromName);
                if(step->parentColor == -1 || step->endpointColor == -1)
                    cursor->insertText(" безбарвна",txtformat);
                else{
                    cursor->insertText(" пофарбована в "+QString::number(step->endpointColor)+" колір",txtformat);
                }

                cursor->insertText(", тож фарбуємо",txtformat);
                insertVertext(cursor,txtformat,(step->parentColor!= step->finalColor)?fromName:toName);
                cursor->insertText("та всі інциндентні їй вершини з тим же кольором в колір "+QString::number(step->finalColor),txtformat);
                cursor->insertText(".",txtformat);

            }
        cursor->insertBlock();
    }
    cursor->insertText("Вага знайденого дерева - ",txtformat);
    int picNum = imageNum++;
    cursor->insertText(QString::number(weigth),txtformat);
    cursor->insertText(". Побудоване дерево приведене на рис. ",txtformat);
    cursor->insertText(QString::number(picNum));
    cursor->insertText(".",txtformat);

    image = drawGraph(primaKreskelResult->graph,printer->pageRect().width());
    cursor->insertImage((*image));
    cursor->insertBlock();
    txtformat.setFont(lableFont);
    cursor->insertText("Рисунок "+QString::number(picNum)+".",txtformat);
    cursor->insertBlock();
}

void DiscretcaPrinter::TravellingSalesman()
{
    QString matrixName[23] ={"A","B","C","D","E","F","G","H","I","K","L","M","N","O","P","Q","R","S","T","V","X","Y","Z"};
    int matrixNamePos = 0;
    Graph::TravellingSalesmanStep * firstStep = graph->TravellingSalesmanAlgaritm();

    QList<GraphVertex *> *baseGraph = graph->getAsList();
    QList<GraphVertex *> *newGraph = new QList<GraphVertex *>();
    for(int i=0;i<baseGraph->size();i++){
        newGraph->append(baseGraph->at(i)->copy());
    }
    txtformat.setFont(titleFont);
    cursor->insertText(QString::number(taskNum++),txtformat);
    cursor->insertText(". Задача комівояжера",txtformat);
    cursor->insertBlock();
    QTextBlockFormat centerFormat;
    //    centerFormat.setAlignment(Qt::AlignHCenter);
    cursor->insertBlock(centerFormat);
    QImage *image = drawGraph(graph->getAsList(),printer->pageRect().width());
    cursor->insertImage((*image));

    centerFormat = QTextBlockFormat();
    centerFormat.setAlignment(Qt::AlignLeft);
    cursor->insertBlock(centerFormat);
    txtformat.setFont(lableFont);
    cursor->insertBlock();
    cursor->insertText("Рисунок "+QString::number(imageNum++)+". Вихидний граф для розрахунків",txtformat);
    cursor->insertBlock();
    cursor->insertBlock();

    bool isFirstIteration  = true;
    QImage *matrixImg;
    bool isNeedPrintNextStep = true;
    txtformat.setFont(normalFont);
    while(firstStep!=nullptr&&firstStep->size >=2){
        if(isNeedPrintNextStep)
            insertTravellingSalesmanStep(isFirstIteration,firstStep,matrixName[matrixNamePos]);
        isNeedPrintNextStep = true;
        cursor->insertText("проведемо оцінку по нулям:",txtformat);
        cursor->insertBlock();
        matrixImg = generateImageFromMatrix(firstStep,graph->getAsList(),matrixName[matrixNamePos],true,true,true);
        cursor->insertImage(*matrixImg );
        cursor->insertBlock();
        cursor->insertBlock();
        if(firstStep->biggestZero.size()>1){
            if(firstStep->forks->at(0)->size>=2){
                cursor->insertText("В матриці декілька нулів мають максимальну оцінку, тому для вибору оптимального скористаємось методом гілок та меж."
                                   " Для цього розглянемо, що було б якби вибрати кожен з нулів",txtformat);
                for(int i=0;i<firstStep->forks->size();i++){
                    cursor->insertBlock();
                    cursor->insertText("У випадку коли виберемо ребро ",txtformat);
                    cursor->insertText(graph->getAsList()->at(firstStep->collumnNum[firstStep->biggestZero.at(i).x()])->getName(),txtformat);
                    cursor->insertText(" - ",txtformat);
                    cursor->insertText(graph->getAsList()->at(firstStep->rowsNum[firstStep->biggestZero.at(i).y()])->getName(),txtformat);
                    cursor->insertText(" отримуємо наступне:",txtformat);
                    cursor->insertBlock();
                    insertTravellingSalesmanStep(isFirstIteration,firstStep->forks->at(i),matrixName[matrixNamePos+1]+QString::number(i+1));
                }
                cursor->insertText("Вибираємо ",txtformat);
                cursor->insertText(graph->getAsList()->at(firstStep->collumnNum[firstStep->biggestZero.at(firstStep->selectedFork).x()])->getName(),txtformat);
                cursor->insertText(" - ",txtformat);
                cursor->insertText(graph->getAsList()->at(firstStep->rowsNum[firstStep->biggestZero.at(firstStep->selectedFork).y()])->getName(),txtformat);
                cursor->insertText(", як варіант з мінімальною сумою коефіцієнтів приведення. ",txtformat);
                cursor->insertBlock();
                cursor->insertText("Викреслюємо строку та стовпець котрим належить вибраний елемент.",txtformat);
                cursor->insertBlock();
                matrixImg = generateImageFromMatrix(firstStep,graph->getAsList(),matrixName[matrixNamePos]+"''",true,true,true,new QPoint(firstStep->biggestZero.at(firstStep->selectedFork)));
                cursor->insertImage(*matrixImg );
                cursor->insertBlock();
                cursor->insertBlock();
                insertTravellingSalesmanDeletedEdge(firstStep);

                cursor->insertText("Елементи, що лишились переписуэмо в матрицю "+matrixName[matrixNamePos+1]+" (",txtformat);
                cursor->insertText(matrixName[matrixNamePos+1],txtformat);
                cursor->insertText(" = ",txtformat);
                cursor->insertText(matrixName[matrixNamePos+1]+QString::number(firstStep->selectedFork+1),txtformat);
                cursor->insertText("). Oтримуэмо наступну матрицю :",txtformat);
                cursor->insertBlock();
                matrixImg = generateImageFromMatrix(firstStep->forks->at(firstStep->selectedFork),graph->getAsList(),matrixName[matrixNamePos+1]);
                cursor->insertImage(*matrixImg );
                cursor->insertBlock();
                cursor->insertBlock();
                isNeedPrintNextStep = false;
            }
        }else{

            cursor->insertText("Вибираємо ",txtformat);
            cursor->insertText(graph->getAsList()->at(firstStep->collumnNum[firstStep->biggestZero.at(firstStep->selectedFork).x()])->getName(),txtformat);
            cursor->insertText(" - ",txtformat);
            cursor->insertText(graph->getAsList()->at(firstStep->rowsNum[firstStep->biggestZero.at(firstStep->selectedFork).y()])->getName(),txtformat);

            cursor->insertBlock();
            cursor->insertText("Викреслюємо строку та стовпець котрим належить вибраний елемент.",txtformat);
            cursor->insertBlock();
            matrixImg = generateImageFromMatrix(firstStep,graph->getAsList(),matrixName[matrixNamePos]+"''",true,true,true,new QPoint(firstStep->biggestZero.at(firstStep->selectedFork)));
            cursor->insertImage(*matrixImg );
            cursor->insertBlock();
            cursor->insertBlock();
            insertTravellingSalesmanDeletedEdge(firstStep);
            cursor->insertText("Елементи, що лишились переписуэмо в матрицю "+matrixName[matrixNamePos+1]+" (",txtformat);
            cursor->insertText(matrixName[matrixNamePos+1],txtformat);
            cursor->insertText(" = ",txtformat);
            cursor->insertText(matrixName[matrixNamePos+1]+QString::number(firstStep->selectedFork+1),txtformat);
            cursor->insertText(")",txtformat);
            cursor->insertBlock();
        }

        int from,to;

        from = firstStep->collumnNum[firstStep->biggestZero.at(firstStep->selectedFork).x()];
        to = firstStep->rowsNum[firstStep->biggestZero.at(firstStep->selectedFork).y()];
        Connection *con = baseGraph->at(from)->getConnectionFor(baseGraph->at(to));
        if(con== nullptr){
            con = baseGraph->at(to)->getConnectionFor(baseGraph->at(from));
        }
        int w = (con == nullptr?0:con->getWeigth());
        newGraph->at(from)->addConnection(newGraph->at(to),w);
        image = drawGraph(newGraph,printer->width()/2);
        cursor->insertText("Внаслідок вибору ребра отримаємо наступний граф : ",txtformat);
        cursor->insertBlock();
        cursor->insertImage(*image);
        cursor->insertBlock();
        txtformat.setFont(lableFont);
        cursor->insertText("Рисунок "+QString::number(imageNum++)+".",txtformat);
        cursor->insertBlock();
txtformat.setFont(normalFont);


        firstStep = firstStep->forks->at(firstStep->selectedFork);
        matrixNamePos++;
    }

    int from,to;
    from = firstStep->collumnNum[firstStep->biggestZero.at(firstStep->selectedFork).x()];
    to = firstStep->rowsNum[firstStep->biggestZero.at(firstStep->selectedFork).y()];
    Connection *con = baseGraph->at(from)->getConnectionFor(baseGraph->at(to));
    if(con== nullptr){
        con = baseGraph->at(to)->getConnectionFor(baseGraph->at(from));
    }
    int w = (con == nullptr?0:con->getWeigth());
    newGraph->at(from)->addConnection(newGraph->at(to),w);

    cursor->insertText("Побудуэмо отриманий граф: ",txtformat);
    cursor->insertBlock();
    image = drawGraph(newGraph,printer->width());
    cursor->insertImage(*image);
    cursor->insertBlock();
    txtformat.setFont(lableFont);
    cursor->insertText("Рисунок "+QString::number(imageNum++)+".",txtformat);
    cursor->insertBlock();
    txtformat.setFont(normalFont);
    cursor->insertText("Розрахунки виконанні вірно, оскілки сума коефіцієнтів приєднання рівна довжинi вибраного гамельтового шляху",txtformat);

}

void DiscretcaPrinter::Venger()
{
    QList<Graph::VengerStep *> * steps=  graph->VengerAlgoritm();
    txtformat.setFont(titleFont);
    cursor->insertText(QString::number(taskNum++),txtformat);
    cursor->insertText(". Задача про призначення (мінімізація збитків)",txtformat);
    cursor->insertBlock();
    QImage *image = drawGraph(graph->getAsList(),printer->width());
    cursor->insertImage(*image);
    cursor->insertBlock();
    txtformat.setFont(lableFont);
    cursor->insertBlock();
    cursor->insertText("Рисунок "+QString::number(imageNum++)+". Зображення вихідного графу",txtformat);
    cursor->insertBlock();
    txtformat.setFont(normalFont);
    cursor->insertBlock();
    cursor->insertText("Згідно графу будуємо матрицю суміжності",txtformat);
    image = generateImageFromMatrix(steps->at(0),"A");
    cursor->insertBlock();
    cursor->insertImage(*image);
    cursor->insertBlock();
    cursor->insertBlock();
    Graph::VengerStepBringing *brStep;
    for(int i=1;i<steps->size();i++){
        Graph::VengerStep *step = steps->at(i);
        switch (step->type) {
        case Graph::VengerStepType::BASIC:
            qDebug()<<"Graph::VengerStepType::BASIC";
            break;
        case Graph::VengerStepType::BRINING:
            qDebug()<<"Graph::VengerStepType::BRINING";
            brStep = (Graph::VengerStepBringing *)step;
            if(brStep->kSum == 0)
                break;
            if(brStep->isByCollumn){
                cursor->insertText("Приведемо матрицю по стовпцю",txtformat);
            }else{
                cursor->insertText("Приведемо матрицю по строчці",txtformat);
            }
            image = generateImageFromMatrix(brStep,"A");
            cursor->insertBlock();
            cursor->insertImage(*image);
            cursor->insertBlock();
            cursor->insertBlock();
            break;
        case Graph::VengerStepType::EGERVARY:
            Graph::VengerStepEgervary *eStep = (Graph::VengerStepEgervary *)steps->at(i);
            for(int k=0;k<eStep->size;k++){
                qDebug()<<"test zero "<<k<<eStep->selectedZeroInRows[k];
            }
            cursor->insertText("Спробуємо вибрати нулі таким чином, аби в кожному стовпці та строчці був вибраний лише один",txtformat);
            cursor->insertBlock();
            image = generateImageFromMatrix(eStep,"A",true);
            cursor->insertImage(*image);
            cursor->insertBlock();
            cursor->insertBlock();
            if(eStep->isFinal)
            {
                cursor->insertText("Оскільки нам вдалось вибрати нуль в кожній строчці та стовпці "
                                   "це значить що призначення визначенні. Побудуємо відповідний граф",txtformat);
                QList<GraphVertex *>  *baseG = graph->getAsList();
                QList<GraphVertex *>  *newGraph = new QList<GraphVertex *> ();
                for(int i=0;i<baseG->size();i++){
                    newGraph->append(baseG->at(i)->copy());
                }
                for(int i=0;i<eStep->size;i++){
                    Connection *con  = baseG->at(i)->getConnectionFor(baseG->at(eStep->selectedZeroInRows[i]+eStep->size));
                    newGraph->at(i)->addConnection(newGraph->at(eStep->selectedZeroInRows[i]+eStep->size),con->getWeigth());
                }
                QImage *image = drawGraph(newGraph,printer->width());
                cursor->insertImage(*image);
                cursor->insertBlock();
                txtformat.setFont(lableFont);
                cursor->insertText("Рисунок "+QString::number(imageNum++)+". Кінцевий граф призначень",txtformat);
                cursor->insertBlock();
                txtformat.setFont(normalFont);
            }else{
                cursor->insertText("Оскільки такий вибір неможливий викреслюємо строки та ставпці,  котрі містять максимальну кількість нулів. ",txtformat);
                image = generateImageFromMatrix(eStep,"A",false,true);
                cursor->insertBlock();
                cursor->insertImage(*image);
                cursor->insertBlock();
                cursor->insertBlock();
                cursor->insertText("З елементів, котрі лишились формуємо матрицю і вибираємо мінімальний елемнт. Мінімальний елемент стоїть на позиції ",txtformat);

                cursor->insertText(QString::number(eStep->minValuePos[1]+1),txtformat);
                cursor->insertText("-",txtformat);
                cursor->insertText(QString::number(eStep->minValuePos[0]+1),txtformat);
                cursor->insertText(" і дорівнює ",txtformat);
                cursor->insertText(QString::number(eStep->sourceMatrix[eStep->minValuePos[0]][eStep->minValuePos[1]]),txtformat);
                cursor->insertText(". Вибраний елемент віднімаємо від членів сформованої матриці і додаємо до тих елементів котрі стоять"
                                   " на перетині викрислениї строк та стовпців. ",txtformat);
                cursor->insertText("Отримуємо наступну матрицю :",txtformat);
                image = generateImageFromMatrix(eStep,"A");
                cursor->insertBlock();
                cursor->insertImage(*image);
                cursor->insertBlock();
                cursor->insertBlock();
            }
            break;
        }

    }
}

void DiscretcaPrinter::insertTravellingSalesmanStep(bool &isFirstIteration,Graph::TravellingSalesmanStep *firstStep,QString name)
{
    if(isFirstIteration){
        cursor->insertText("Складемо матрицю суміжності для вихідного графа",txtformat);
        cursor->insertBlock();
        isFirstIteration = false;
    }
    else{
        cursor->insertText("Розгялянемо матрицю суміжності для данної ітерації",txtformat);
        cursor->insertBlock();
    }
    QImage *matrixImg= generateImageFromMatrix(firstStep,graph->getAsList(),name);
    cursor->insertImage(*matrixImg);
    cursor->insertBlock();
    cursor->insertBlock();
    if(firstStep->isBringedByRow()){
        cursor->insertText("Приводимо вихідний граф по строкам:",txtformat);
        cursor->insertBlock();
        cursor->insertBlock();
        matrixImg = generateImageFromMatrix(firstStep,graph->getAsList(),name+"'",true);
        cursor->insertImage(*matrixImg );
        cursor->insertBlock();
        cursor->insertBlock();
    }else{
        cursor->insertText("Приводити матрицю по строкам не потрібно, адже в кожній строчці вже є нуль.",txtformat);
        cursor->insertBlock();
    }
    if(firstStep->isBringedByCollumn()){
        cursor->insertText("Приводимо вихідний граф по стовпцям:",txtformat);
    }else{
        cursor->insertText("Приводити граф по стовпцям не потрібно, адже в кожному стовпцю вже є нуль. ",txtformat);
        cursor->insertBlock();
        cursor->insertText("Виписуємо приведенну матрицю для "+name,txtformat);
    }
    cursor->insertBlock();
    cursor->insertBlock();
    matrixImg = generateImageFromMatrix(firstStep,graph->getAsList(),name+"''",true,true);
    cursor->insertImage(*matrixImg );
    cursor->insertBlock();
    cursor->insertBlock();
    QString kSum = "";
    float sum = 0;
    for(int i=0;i<firstStep->size;i++){
        if(firstStep->bringingRows[i]!=0){
            if(kSum.size()>0)
                kSum+=" + ";
            kSum+=QString::number(firstStep->bringingRows[i]);
            sum+=firstStep->bringingRows[i];
        }
    }

    for(int i=0;i<firstStep->size;i++){
        if(firstStep->bringingColumn[i]!=0){
            if(kSum.size()>0)
                kSum+=" + ";
            kSum+=QString::number(firstStep->bringingColumn[i]);
            sum+=firstStep->bringingColumn[i];
        }
    }

    if(firstStep->commonEstimation-sum>0){
        if(kSum.size()>0)
        {
            kSum = " + "+kSum;
        }
        kSum =QString::number(firstStep->commonEstimation-sum)+kSum;
    }
    kSum += " = ";
    kSum += QString::number(firstStep->commonEstimation);
    if(sum>0){
        cursor->insertText("Сума коефіцієнтів приведення : ∑ = ",txtformat);
        cursor->insertText(kSum,txtformat);
        cursor->insertBlock();
    }
    else{
        cursor->insertText("Сума коефіцієнтів приведення не змінилась і рівна",txtformat);
        cursor->insertText(QString::number(firstStep->commonEstimation));
    }
}

void DiscretcaPrinter::insertTravellingSalesmanDeletedEdge(Graph::TravellingSalesmanStep *currentStep)
{
    if(currentStep==nullptr)
        return;
    qDebug()<<"insert in ti text deleting edges "<< currentStep->deletedEdges->size();
    if(currentStep->deletedEdges->size()>0){
        cursor->insertText("Відкинемо ребра, при виборі яких відбудеться раннє повернення або раннє замикання: ");
        for(int i=0;i<currentStep->deletedEdges->size();i++){
            cursor->insertText(graph->getAsList()->at(currentStep->deletedEdges->at(i)[0])->getName());
            cursor->insertText(" - ");
            cursor->insertText(graph->getAsList()->at(currentStep->deletedEdges->at(i)[1])->getName());
            if(i!=currentStep->deletedEdges->size()-1){
                cursor->insertText(", ");
            }
        }
    }
    cursor->insertBlock();
}
