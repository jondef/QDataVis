//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"

#define NUMBER_OF_POINTS 1000000


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);


	// ! CONNECTIONS
	connect(ui->QPushButton_PlotPoints, SIGNAL(clicked()), this, SLOT(QPushButton_PlotPoints_clicked()));
	connect(ui->QLineEdit_addFunction, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));


	/* ! GRAPH INITIALIZATION */
	ui->customPlot->setOpenGl(true, 16); // enable openGL
	ui->customPlot->openGl() ? qDebug() << "OPENGL enabled" : qDebug() << "OPENGL disabled";
	if (ui->customPlot->openGl()) {
		qDebug() << "OPENGL enabled";
		statusBarMsg("openGL enabled", 5000);
	} else {
		qDebug() << "OPENGL disabled";
		statusBarMsg("openGL disabled", 5000);
	}
	ui->customPlot->addGraph();

	// give the axes some labels:
	ui->customPlot->xAxis->setLabel("x");
	ui->customPlot->yAxis->setLabel("y");
	// set axes ranges, so we see all data:
	ui->customPlot->xAxis->setRange(-10, 10);
	ui->customPlot->yAxis->setRange(-10, 10);

	// Set interations
	ui->customPlot->setInteraction(QCP::iRangeDrag, true);
	ui->customPlot->setInteraction(QCP::iRangeZoom, true);

	ui->customPlot->replot();
}


MainWindow::~MainWindow() {
	delete ui;
}


void MainWindow::statusBarMsg(const char msg[], int time) {
	ui->statusbar->showMessage(msg, time);
}


void MainWindow::QLineEdit_addFunction_returnPressed() {
	QString text = ui->QLineEdit_addFunction->text();

    BinaryTree tree(text);

	QVector<double> x_arr = generateXArray(-1000, 1000, NUMBER_OF_POINTS);
	QVector<double> y_arr = tree.calculateTree(x_arr, ui->progressBar);
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
	ui->customPlot->addGraph();
	ui->customPlot->graph()->setName(QString("New graph %1").arg(ui->customPlot->graphCount() - 1));
	ui->customPlot->graph()->setData(xArray, yArray);
	// ! QCPCurve *plotCurves = new QCPCurve(customPlot->xAxis, customPlot->yAxis);

	//customPlot->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
    //if (rand()%100 > 50)
    //    customPlot->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%14+1)));
    QPen graphPen;
    graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
    graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)
	ui->customPlot->graph()->setPen(graphPen);
	ui->customPlot->replot();
}


void MainWindow::QPushButton_PlotPoints_clicked() {
	// * Function is connected to the plot all points button
	// * It clears the points graphs, then replots everything
	// * in the above text edit
	// todo: save plotting parameters, so it doesn't always use
	// todo: the default ones, and not required to type them in again every time
	for (auto i : *graphList) {
		// delete previously plotted graphs
		ui->customPlot->removeGraph(i);
	}
	// remove the pointers
	graphList->clear();

	graphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));

	QString text = ui->QTextEdit_pointsList->toPlainText();
	QStringList arrayLine = text.split("\n", QString::SkipEmptyParts);

	for (int line = 0; line < arrayLine.length(); line++) { // * iterate over lines
		QString currentLine = arrayLine.at(line);

		if (currentLine.at(0) == "[") { // * configuration line
			// ! create a new graph to configure it
			graphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
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
	ui->customPlot->replot();
}