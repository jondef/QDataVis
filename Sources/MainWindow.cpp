//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"

#define NUMBER_OF_POINTS 1000000


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
    // HD 1280 x 720
    setBaseSize(1280, 720);
    setMinimumSize(1280, 720);
    //setFixedSize(500, 500);
    setWindowTitle("GUI");

    pQWidget_centralWidget->setLayout(pQGridLayout_mainLayout);
    this->setCentralWidget(pQWidget_centralWidget);

	pQTabWidget_graphPlottables->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);
	progressBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Ignored);

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


    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setGeometry(10, 10, 180, 30);


    // CONNECTIONS
    connect(pQPushButton_Quit, SIGNAL(clicked()), QApplication::instance(), SLOT(quit()));
    connect(pQPushButton_PlotAllPoints, SIGNAL(clicked()), this, SLOT(QPushButton_PlotAllPoints_clicked()));
    connect(pQLineEdit_addFunction, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));

    pQGridLayout_mainLayout->addWidget(pQPushButton_Quit, 4, 0, 1, 2);

    pQGridLayout_mainLayout->addWidget(progressBar);

    pQGridLayout_mainLayout->addWidget(customPlot, 0, 0, 1, 2);

    initGraph();
}


MainWindow::~MainWindow() {
	delete ui;
}


void MainWindow::initGraph() {

    customPlot->setOpenGl(true, 16); // enable openGL
    customPlot->openGl() ? qDebug() << "OPENGL enabled" : qDebug() << "OPENGL disabled";
    customPlot->addGraph();

    // give the axes some labels:
    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    // set axes ranges, so we see all data:
	customPlot->xAxis->setRange(-10, 10);
	customPlot->yAxis->setRange(-10, 10);

	// Set interations
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);

    customPlot->replot();
}


void MainWindow::statusBarMsg(const char msg[], int time) {
    statusBar()->showMessage(msg, time);
}


void MainWindow::QLineEdit_addFunction_returnPressed() {
    QString text = pQLineEdit_addFunction->text();

    BinaryTree tree(text);

	QVector<double> x_arr = generateXArray(-1000, 1000, NUMBER_OF_POINTS);
	QVector<double> y_arr = tree.calculateTree(x_arr, progressBar);
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


void MainWindow::QPushButton_PlotAllPoints_clicked() {
	// * Function is connected to the plot all points button
	// * It clears the points graphs, then replots everything
	// * in the above text edit
	// todo: save plotting parameters, so it doesn't always use
	// todo: the default ones, and not required to type them in again every time
	for (auto i : *graphList) {
		// delete previously plotted graphs
		customPlot->removeGraph(i);
	}
	// remove the pointers
	graphList->clear();

	graphList->append(new QCPGraph(customPlot->xAxis, customPlot->yAxis));

	QString text = pQTextEdit_pointsList->toPlainText();
	QStringList arrayLine = text.split("\n", QString::SkipEmptyParts);

	for (int line = 0; line < arrayLine.length(); line++) { // * iterate over lines
		QString currentLine = arrayLine.at(line);

		if (currentLine.at(0) == "[") { // * configuration line
			// ! create a new graph to configure it
			graphList->append(new QCPGraph(customPlot->xAxis, customPlot->yAxis));
			// * set defaults for graph
			graphList->last()->setAdaptiveSampling(false);
			graphList->last()->removeFromLegend();
			graphList->last()->setLineStyle(QCPGraph::lsNone);
			graphList->last()->setScatterStyle(QCPScatterStyle::ssDot);
			graphList->last()->setPen(QPen(QBrush(Qt::red), 3));

			QList<QString> parameterArray = currentLine.remove("[").remove("]").split(",", QString::SkipEmptyParts);

			for (const auto &parameter : parameterArray) {
				QString param = parameter.split("=").at(0);
				QString value = parameter.split("=").at(1);

				qDebug() << "Setting graph param " << param << " to " << value;

				if (param == "LineStyle") {
					if (value == "None") {
						graphList->last()->setLineStyle(QCPGraph::lsNone);
					} else if (value == "Line") {
						graphList->last()->setLineStyle(QCPGraph::lsLine);
					} else if (value == "IsImpulse") {
						graphList->last()->setLineStyle(QCPGraph::lsImpulse);
					} else if (value == "StepLeft") {
						graphList->last()->setLineStyle(QCPGraph::lsStepLeft);
					} else if (value == "StepCenter") {
						graphList->last()->setLineStyle(QCPGraph::lsStepCenter);
					} else if (value == "StepRight") {
						graphList->last()->setLineStyle(QCPGraph::lsStepRight);
					}

				} else if (param == "Pen") {
					// ? example: Pen=red;3
					QString color = value.split(";").at(0);
					QString width = value.split(";").at(1);

					if (color == "black") {
						graphList->last()->setPen(QPen(QBrush(Qt::black), width.toInt()));
					} else if (color == "white") {
						graphList->last()->setPen(QPen(QBrush(Qt::white), width.toInt()));
					} else if (color == "darkGray") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkGray), width.toInt()));
					} else if (color == "gray") {
						graphList->last()->setPen(QPen(QBrush(Qt::gray), width.toInt()));
					} else if (color == "lightGray") {
						graphList->last()->setPen(QPen(QBrush(Qt::lightGray), width.toInt()));
					} else if (color == "red") {
						graphList->last()->setPen(QPen(QBrush(Qt::red), width.toInt()));
					} else if (color == "green") {
						graphList->last()->setPen(QPen(QBrush(Qt::green), width.toInt()));
					} else if (color == "blue") {
						graphList->last()->setPen(QPen(QBrush(Qt::blue), width.toInt()));
					} else if (color == "cyan") {
						graphList->last()->setPen(QPen(QBrush(Qt::cyan), width.toInt()));
					} else if (color == "magenta") {
						graphList->last()->setPen(QPen(QBrush(Qt::magenta), width.toInt()));
					} else if (color == "yellow") {
						graphList->last()->setPen(QPen(QBrush(Qt::yellow), width.toInt()));
					} else if (color == "darkRed") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkRed), width.toInt()));
					} else if (color == "darkGreen") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkGreen), width.toInt()));
					} else if (color == "darkBlue") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkBlue), width.toInt()));
					} else if (color == "darkCyan") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkCyan), width.toInt()));
					} else if (color == "darkMagenta") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkMagenta), width.toInt()));
					} else if (color == "darkYellow") {
						graphList->last()->setPen(QPen(QBrush(Qt::darkYellow), width.toInt()));
					} else if (color == "transparent") {
						graphList->last()->setPen(QPen(QBrush(Qt::transparent), width.toInt()));
					}
				} else if (param == "ScatterStyle") {
					if (value == "None") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssNone);
					} else if (value == "Dot") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssDot);
					} else if (value == "Cross") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssCross);
					} else if (value == "Circle") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssCircle);
					} else if (value == "CrossCircle") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssCrossCircle);
					} else if (value == "CrossSquare") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssCrossSquare);
					} else if (value == "Custom") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssCustom);
					} else if (value == "Diamond") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssDiamond);
					} else if (value == "Disc") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssDisc);
					} else if (value == "Peace") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssPeace);
					} else if (value == "Pixmap") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssPixmap);
					} else if (value == "Plus") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssPlus);
					} else if (value == "PlusCircle") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssPlusCircle);
					} else if (value == "PlusSquare") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssPlusSquare);
					} else if (value == "Square") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssSquare);
					} else if (value == "Star") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssStar);
					} else if (value == "Triangle") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssTriangle);
					} else if (value == "TriangleInverted") {
						graphList->last()->setScatterStyle(QCPScatterStyle::ssTriangleInverted);
					}

				} else if (param == "RemoveFromLegend") {
					if (value == "True") {
						graphList->last()->removeFromLegend();
					}

				} else if (param == "AdaptiveSampling") {
					if (value == "True") {
						graphList->last()->setAdaptiveSampling(true);
					} else if (value == "False") {
						graphList->last()->setAdaptiveSampling(false);
					}
				}
			} // iteration over parameter and value

		} else { // * append point to last graph
			QStringList coords = currentLine.split(",", QString::SkipEmptyParts);
			if (coords.length() == 2) {
				graphList->last()->addData(coords.at(0).toDouble(), coords.at(1).toDouble());
			}
		}
	}
	customPlot->replot();
}