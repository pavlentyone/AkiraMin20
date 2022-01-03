#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    maxX = 1;
    minX = -1;
    chromosomeAmount = 20;
    geneAmount = 20;
}

MainWindow::~MainWindow()
{
    delete ui;
}

double MainWindow::Function(double x){
    double y = 3 + pow(x, 2) + 3 * cos(2 * 3.14 * x);
    return y;
}

int MainWindow::IntFromBoolVector(QVector<bool> vec){
    int retval = 0;
    int i =0;
    for (QVector<bool>::iterator it = vec.begin(); it != vec.end() - 1; it++,i++){
        if(*it){
            retval |= 1<<i;
        }
    }
    if(vec.length() > 0 && vec.last() == 0)
        retval *= -1;
    return retval;
}

double MainWindow::IntNormalize(int val){
    double x;
    x = double(val + pow(2, geneAmount - 1))*(maxX - minX)/pow(2, geneAmount - 1)/double(2) + minX ;
    return x;
}

QVector<QVector<bool>> MainWindow::OnePointMethod(QVector<QVector<bool>> parentGeneration){
    QVector<QVector<bool>> childGeneration;
    childGeneration.resize(parentGeneration.length());

    for(int i = 0; i < parentGeneration.length(); i++){
        int fatherNum = rand() % parentGeneration.length();
        int motherNum = rand() % parentGeneration.length();

        int crossoverPlace = rand() % parentGeneration[i].length();

        childGeneration[i].resize(parentGeneration[i].length());
        for(int j = 0; j < crossoverPlace; j++)
            childGeneration[i][j] = parentGeneration[fatherNum][j];
        for(int j = crossoverPlace; j < parentGeneration[i].length(); j++)
            childGeneration[i][j] = parentGeneration[motherNum][j];

    }
    return childGeneration;
}

QVector<QVector<bool>> MainWindow::MultyPointMethod(QVector<QVector<bool>> parentGeneration){
    QVector<QVector<bool>> childGeneration;
    childGeneration.resize(parentGeneration.length());

    for(int i = 0 ; i < parentGeneration.length(); i++){
        int fatherNum = rand() % parentGeneration.length();
        int motherNum = rand() % parentGeneration.length();

        //creating crossover places
        QVector<int> crossoverPlaces;
        crossoverPlaces.resize(1 + rand() % (parentGeneration[i].length() - 1));
        for(int j = 0; j < crossoverPlaces.length(); j++)
            crossoverPlaces[j] = rand() % parentGeneration[i].length();

        //sorting crossover places
        for(int f = 0 ; f < crossoverPlaces.length(); f++){
            bool check = true;
            for(int s = 1; s < crossoverPlaces.length(); s++)
                if(crossoverPlaces[s] < crossoverPlaces[s - 1]){
                    int temp = crossoverPlaces[s];
                    crossoverPlaces[s] = crossoverPlaces[s - 1];
                    crossoverPlaces[s - 1] = temp;
                    check = false;
                }
            if(check)
                break;
        }

        //add 0 number in crossover places vector to be able start from first(0) gene. add length of crossover places to finish crossovering correctly
        crossoverPlaces.push_front(0);
        crossoverPlaces.push_back(parentGeneration[i].length());

        //remove repeated crossover places
        for(int j = 1; j < crossoverPlaces.length(); j++)
            if(crossoverPlaces[j] == crossoverPlaces[j - 1]){
                crossoverPlaces.removeAt(j);
                j--;
            }

        //crossovering father and mother to get a child
        childGeneration[i].resize(parentGeneration[i].length());
        for(int c = 1; c < crossoverPlaces.length(); c++)
            for(int j = crossoverPlaces[c - 1]; j < crossoverPlaces[c]; j++)
                if(c % 2)
                    childGeneration[i][j] = parentGeneration[fatherNum][j];
                else
                    childGeneration[i][j] = parentGeneration[motherNum][j];
    }
    return childGeneration;
}

QVector<QVector<bool>> MainWindow::UniformMethod(QVector<QVector<bool>> parentGeneration){
    QVector<QVector<bool>> childGeneration;
    childGeneration.resize(parentGeneration.length());

    for(int i = 0; i < parentGeneration.length(); i++){
        int fatherNum = rand() % parentGeneration.length();
        int motherNum = rand() % parentGeneration.length();

        childGeneration[i].resize(parentGeneration[i].length());
        for(int j = 0; j < parentGeneration[i].length(); j++)
            if(rand() % 2)
                childGeneration[i][j] = parentGeneration[fatherNum][j];
            else
                childGeneration[i][j] = parentGeneration[motherNum][j];
    }
    return childGeneration;
}

QVector<QVector<bool>> MainWindow::GetNextGeneration(QVector<QVector<bool>> parentGeneration, QVector<QVector<bool>> childGeneration){
    QVector<QVector<bool>> twoGenerations;
    twoGenerations.resize(parentGeneration.length() + childGeneration.length());

    //copy from parent generation
    for(int i = 0; i < parentGeneration.length(); i++){
        twoGenerations[i].resize(parentGeneration[i].length());
        for(int j = 0; j < parentGeneration[i].length(); j++)
            twoGenerations[i][j] = parentGeneration[i][j];
    }

    //copy from child generation
    for(int i = 0; i < childGeneration.length(); i++){
        twoGenerations[i + parentGeneration.length()].resize(childGeneration[i].length());
        for(int j = 0; j < childGeneration[i].length(); j++)
            twoGenerations[i + parentGeneration.length()][j] = childGeneration[i][j];
    }

    //sorting
    for(int i = 0; i < twoGenerations.length(); i++){
        bool check = true;
        for(int j = 1; j < twoGenerations.length(); j++)
            if(Function(IntNormalize(IntFromBoolVector(twoGenerations[j]))) <
               Function(IntNormalize(IntFromBoolVector(twoGenerations[j - 1])))){
                QVector<bool> temp = twoGenerations[j];
                twoGenerations[j] = twoGenerations[j - 1];
                twoGenerations[j -1] = temp;
                check = false;
            }
        if(check)
            break;
    }

    //take first chromosomes
    QVector<QVector<bool>> nextGeneration;
    nextGeneration.resize(parentGeneration.length());
    for(int i = 0; i < parentGeneration.length(); i++){
        nextGeneration[i] = twoGenerations[i];
    }

    return nextGeneration;
}

void MainWindow::ShowInListView(QVector<QVector<bool>> vec, QListView *lv){
    QStringList stringList;
    for(int i = 0 ; i < vec.length(); i++){
        QString string;
        for(int j = vec[i].length() -1; j > -1; j--)
            string += vec[i][j] ? "1" : "0";
        stringList << QString::number(i + 1) + ". " + string + " (" + QString::number(IntNormalize(IntFromBoolVector(vec[i]))) + ")";
    }

    QStringListModel *stringListModel = new QStringListModel(this);
    stringListModel->setStringList(stringList);
    lv->setModel(stringListModel);
    lv->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_pbCreateGeneration_clicked()
{
    firstGeneration.resize(chromosomeAmount);
    for(int i = 0; i < firstGeneration.length(); i++){
        firstGeneration[i].resize(geneAmount);
        for(int j = 0; j < firstGeneration[i].length(); j++)
            firstGeneration[i][j] = rand() % 2;
    }

    //show first generation
    ShowInListView(firstGeneration, ui->lvFirstGeneration);
}

QVector<QVector<bool>> Mutation(QVector<QVector<bool>> generation){
    QVector<QVector<bool>> mutatedGeneration;
    int maxChance = 1000;
    int chance = 50;
    mutatedGeneration.resize(generation.length());
    for(int i = 0; i < generation.length(); i++){
        mutatedGeneration[i].resize(generation[i].length());
        for(int j = 0; j < generation[i].length(); j++)
            if(chance >= (rand() % maxChance))
                mutatedGeneration[i][j] = !generation[i][j];
            else
                mutatedGeneration[i][j] = generation[i][j];
    }
    return mutatedGeneration;
}

void MainWindow::on_pbProcess_clicked()
{
    QVector<QVector<bool>> childGeneration = UniformMethod(firstGeneration);

    minGeneration = GetNextGeneration(firstGeneration, childGeneration);
    minGeneration = Mutation(minGeneration);
    for(int i = 1 ; i < 10; i++){
        childGeneration = UniformMethod(minGeneration);
        minGeneration = GetNextGeneration(minGeneration, childGeneration);
        minGeneration = Mutation(minGeneration);
    }

    QVector<double> x;
    x.resize(100);
    double xi = minX;
    for(int i = 0; i < x.length(); i++){
        x[i] = xi;
        xi += (maxX-minX)/double(100);
    }

    QVector<double> y;
    y.resize(x.length());
    for(int i = 0; i < x.length(); i++)
        y[i] = Function(x[i]);

    QVector<double> firstX;
    firstX.resize(firstGeneration.length());
    for(int i = 0; i < firstGeneration.length(); i++)
        firstX[i] = IntNormalize(IntFromBoolVector(firstGeneration[i]));

    QVector<double> firstY;
    firstY.resize(firstX.length());
    for(int i = 0; i < firstX.length(); i++)
        firstY[i] = Function(firstX[i]);

    QVector<double> childX;
    childX.resize(childGeneration.length());
    for(int i = 0; i < childGeneration.length(); i++)
        childX[i] = IntNormalize(IntFromBoolVector(childGeneration[i]));

    QVector<double> childY;
    childY.resize(childX.length());
    for(int i = 0; i < childX.length(); i++)
        childY[i] = Function(childX[i]);

    QVector<double> minimX;
    minimX.resize(minGeneration.length());
    for(int i = 0; i < minGeneration.length(); i++)
        minimX[i] = IntNormalize(IntFromBoolVector(minGeneration[i]));

    QVector<double> minimY;
    minimY.resize(minimX.length());
    for(int i = 0; i < minimX.length(); i++)
        minimY[i] = Function(minimX[i]);

    ui->cpGraph->clearGraphs();
    ui->cpGraph->addGraph();
    ui->cpGraph->graph(0)->setData(x, y);
    ui->cpGraph->graph(0)->setPen(QColor(0,0,255,255));
    ui->cpGraph->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->cpGraph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(1)->setData(firstX, firstY);
    ui->cpGraph->graph(1)->setPen(QColor(0,255,0,255));
    ui->cpGraph->graph(1)->setLineStyle(QCPGraph::lsNone);
    ui->cpGraph->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross));

//    ui->cpGraph->addGraph();
//    ui->cpGraph->graph(2)->setData(childX, childY);
//    ui->cpGraph->graph(2)->setPen(QColor(150,150,0,255));
//    ui->cpGraph->graph(2)->setLineStyle(QCPGraph::lsNone);
//    ui->cpGraph->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross));

    ui->cpGraph->addGraph();
    ui->cpGraph->graph(2)->setData(minimX, minimY);
    ui->cpGraph->graph(2)->setPen(QColor(255,0,0,255));
    ui->cpGraph->graph(2)->setLineStyle(QCPGraph::lsNone);
    ui->cpGraph->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle));

    ui->cpGraph->xAxis->setLabel("x");
    ui->cpGraph->yAxis->setLabel("y");
    ui->cpGraph->xAxis->setRange(minX - 1, maxX + 1);//Для оси Ox
    ui->cpGraph->yAxis->setRange(-10, 20);//Для оси Oy
    ui->cpGraph->replot();

    //show child generation
    ShowInListView(childGeneration, ui->lvChildGeneration);

    ShowInListView(minGeneration, ui->lvNextGeneration);
}
