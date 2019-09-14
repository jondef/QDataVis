//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // HD 1280 x 720
    setBaseSize(1280, 720);
    setMinimumSize(1280, 720);
    //setFixedSize(500, 500);
    setWindowTitle("GUI");

    pQWidget_centralWidget->setLayout(pQGridLayout_mainLayout);
    this->setCentralWidget(pQWidget_centralWidget);

    QWidget *pQWidget_graphPlottables_functionTab = new QWidget;
    QGridLayout *pQGridLayout_graphPlottables_functionTab = new QGridLayout;
    pQGridLayout_graphPlottables_functionTab->addWidget(pQLabel_addFunction, 0, 0);
    pQGridLayout_graphPlottables_functionTab->addWidget(pQLineEdit_addFunction, 0, 1);

    pQWidget_graphPlottables_functionTab->setLayout(pQGridLayout_graphPlottables_functionTab);
    pQTabWidget_graphPlottables->addTab(pQWidget_graphPlottables_functionTab, "Functions");

    QWidget *pQWidget_graphPlottables_pointsTab = new QWidget;
    QGridLayout *pQGridLayout_graphPlottables_pointsTab = new QGridLayout;
    pQGridLayout_graphPlottables_pointsTab->addWidget(pQTextEdit_pointsList, 0, 0);
    pQTextEdit_pointsList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
    pQGridLayout_graphPlottables_pointsTab->addWidget(pQPushButton_PlotAllPoints, 1, 0);

    pQWidget_graphPlottables_pointsTab->setLayout(pQGridLayout_graphPlottables_pointsTab);
    pQTabWidget_graphPlottables->addTab(pQWidget_graphPlottables_pointsTab, "Points");

    pQGridLayout_mainLayout->addWidget(pQTabWidget_graphPlottables, 0, 2);


    QProgressBar *progressBar = new QProgressBar;
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setGeometry(10, 10, 180, 30);


    QSlider *slider = new QSlider;
    slider->setOrientation(Qt::Horizontal);
    slider->setRange(0, 100);
    slider->setValue(0);
    slider->setGeometry(10, 40, 180, 30);



    // CONNECTIONS
    QObject::connect(slider, SIGNAL(valueChanged(int)), progressBar, SLOT(setValue(int)));
    connect(pQPushButton_Quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));

    connect(pQPushButton_removePoint, SIGNAL(clicked()), this, SLOT(QPushButton_removePoint_clicked()));
    connect(pQPushButton_PlotAllPoints, SIGNAL(clicked()), this, SLOT(QPushButton_PlotAllPoints_clicked()));

    connect(pQLineEdit_addPoint, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addPoint_returnPressed()));
    connect(pQLineEdit_addFunction, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));


    pQGridLayout_mainLayout->addWidget(pQLabel_addPoint, 1, 0);
    pQGridLayout_mainLayout->addWidget(pQLineEdit_addPoint, 1, 1);

    pQGridLayout_mainLayout->addWidget(pQComboBox_deletePoint, 2, 0);
    pQGridLayout_mainLayout->addWidget(pQPushButton_removePoint, 2, 1);
    pQGridLayout_mainLayout->addWidget(pQPushButton_Quit, 4, 0, 1, 2);

    pQGridLayout_mainLayout->addWidget(progressBar);
    pQGridLayout_mainLayout->addWidget(slider);

    pQGridLayout_mainLayout->addWidget(customPlot, 0, 0, 1, 2);
    initGraph();
}


void MainWindow::initGraph() {

    customPlot->setOpenGl(true, 16); // enable openGL
    customPlot->openGl() ? qDebug() << "OPENGL enabled" : qDebug() << "OPENGL disabled";
    customPlot->addGraph();

    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
    customPlot->xAxis->setRange(-5, 5);
    customPlot->yAxis->setRange(-5, 5);

    // Set interations:
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);

    // configure the points graph
    pointsGraph->setAdaptiveSampling(false);
    pointsGraph->removeFromLegend();
    pointsGraph->setLineStyle(QCPGraph::lsNone);
    pointsGraph->setScatterStyle(QCPScatterStyle::ssDot);
    pointsGraph->setPen(QPen(QBrush(Qt::red), 3));
    //pointsGraph->setPen(QPen(QBrush(QColor(rand()%255, rand()%255, rand()%255)),2));

    customPlot->replot();
}


void MainWindow::QLineEdit_addPoint_returnPressed() {
    QString text = pQLineEdit_addPoint->text();
    QStringList stringList = text.split(",");

    if (stringList.length() == 2) {
        QString xCoord = stringList[0];
        QString yCoord = stringList[1];

        bool xOk, yOk;
        double xCoordD = xCoord.toDouble(&xOk);
        double yCoordD = yCoord.toDouble(&yOk);

        // if ok == true -> conversion successful
        if (xOk && yOk) {
            this->plot(xCoordD, yCoordD);
        } else {
            statusBarMsg("Invalid 2D coordinates");
        }
    } else {
        statusBarMsg("Invalid 2D coordinates");
    }
}


void MainWindow::statusBarMsg(const char msg[], int time) {
    statusBar()->showMessage(msg, time);
}


void MainWindow::QLineEdit_addFunction_returnPressed() {
    QString text = pQLineEdit_addFunction->text();

    BinaryTree tree(text);

    QVector<double> x_arr = generateXArray(-10, 10, 5000001);
    QVector<double> y_arr = tree.calculateTree(x_arr);

    plot(x_arr, y_arr);
}

QVector<double> MainWindow::generateXArray(int lowerLim, int upperLim, unsigned int length) {
    // If you want something that will stick around and cannot go out of scope, you should allocate it with new
    // remember to delete it with delete[] array;
    QVector<double> finalArray(length);
    //auto *finalArray = new float[length];
    double difference = upperLim - lowerLim;
    double increment = difference / (length - 1);

    for (unsigned int i = 0; i < length; i++) {
        finalArray[i] = lowerLim + increment * i;
    }

    return finalArray;
}


void MainWindow::plot(const QVector<double> &xArray, const QVector<double> &yArray) {
    customPlot->addGraph();
    customPlot->graph()->setName(QString("New graph %1").arg(customPlot->graphCount() - 1));
    customPlot->graph()->setData(xArray, yArray);
    //customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
    //if (rand()%100 > 50)
    //    customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
    QPen graphPen;
    graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
    graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)
    customPlot->graph()->setPen(graphPen);
    customPlot->replot();
}

void MainWindow::plot(double x, double y) {
    pointsGraph->addData(x, y);
    customPlot->replot();

    // add point to list and combobox
    pPointsList->append(qMakePair(x, y));
    pQComboBox_deletePoint->addItem(QString("(%1, %2)").arg(x).arg(y));
}

void MainWindow::QPushButton_removePoint_clicked() {
    // this function takes the point to
    // delete from the combobox, clears the graph and replots everything
    pPointsList->removeAt(pQComboBox_deletePoint->currentIndex());

    pQComboBox_deletePoint->removeItem(pQComboBox_deletePoint->currentIndex());

    // clear the graph
    pointsGraph->data()->clear();

    // replot the points
    for (auto i : *pPointsList) {
        pointsGraph->addData(i.first, i.second);
    }

    customPlot->replot();
}


void MainWindow::QPushButton_PlotAllPoints_clicked() {
    pointsGraph->data()->clear();
    QString text = pQTextEdit_pointsList->toPlainText();
    QStringList arrayLine = text.split("\n");

    for (int i = 0; i < arrayLine.length(); i++) {
        QStringList coords = arrayLine.at(i).split(",");
        pointsGraph->addData(coords.at(0).toDouble(), coords.at(1).toDouble());
    }
    customPlot->replot();
}