//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);


	// ! CONNECTIONS
	connect(ui->QPushButton_PlotPoints, SIGNAL(clicked()), this, SLOT(QPushButton_PlotPoints_clicked()));
	connect(ui->QPushButton_deleteFunction, SIGNAL(clicked()), this, SLOT(QPushButton_deleteFunction_clicked()));
	connect(ui->QLineEdit_addFunction, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));
	connect(ui->QLineEdit_functionParam, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));


	/* ! GRAPH INITIALIZATION */
	ui->customPlot->setOpenGl(true, 16); // enable openGL
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
	text.remove(" ");

    BinaryTree tree(text);

	QStringList xConfig = ui->QLineEdit_functionParam->text().split(",");
	int min, max, len;
	if (xConfig.length() == 3) {
		min = xConfig.at(0).toInt();
		max = xConfig.at(1).toInt();
		len = xConfig.at(2).toInt();

		// * check if values failed to convert
		// * if failed, they'll be 0
		min == 0 ? min = -10 : min;
		max == 0 ? max = 10 : max;
		len == 0 ? len = 1000 : len;
	} else { // default values
		min = -10;
		max = 10;
		len = 1000;
	}
	QVector<double> xArray = generateXArray(min, max, len);
	QVector<double> yArray = tree.calculateTree(xArray, ui->progressBar);

	//////////////////////////////////
	//	PLOTTING AND ADDING
	//////////////////////////////////
	QList<QColor> colors = {
			QColor(qRgb(31, 119, 180)),
			QColor(qRgb(255, 127, 14)),
			QColor(qRgb(44, 160, 44)),
			QColor(qRgb(214, 39, 40)),
			QColor(qRgb(148, 103, 189)),
			QColor(qRgb(140, 86, 75)),
			QColor(qRgb(244, 119, 194)),
			QColor(qRgb(127, 127, 127)),
			QColor(qRgb(188, 189, 34)),
			QColor(qRgb(23, 190, 207))
	};

	functionGraphList->append(new QCPCurve(ui->customPlot->xAxis, ui->customPlot->yAxis));
	functionGraphList->last()->setData(xArray, yArray);

	int colorIndex = functionGraphList->length() - 1;

	// only take the last number of the index if bigger than 10
	if (colorIndex > 9) {
		QString str = QString::number(colorIndex);
		colorIndex = str.mid(str.length() - 1).toInt();
	}

	QPen graphPen;
	graphPen.setColor(QColor(colors.at(colorIndex)));
	graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)
	functionGraphList->last()->setPen(graphPen); // apply color to graph
	ui->customPlot->replot();


	// * add item to widget and set the appropriate icon color
	auto pixmap = QPixmap(16, 16);
	pixmap.fill(QColor(colors.at(colorIndex)));

	auto *item = new QListWidgetItem();
	item->setText(text);
	item->setIcon(QIcon(pixmap));
	ui->QListWidget_functionList->addItem(item);
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
//    graphPen.setColor(QColor(rand() % 245 + 10, rand() % 245 + 10, rand() % 245 + 10));
	graphPen.setColor(QColor(255, 255, 0));
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
	for (auto i : *pointsGraphList) {
		// delete previously plotted graphs
		ui->customPlot->removeGraph(i);
	}
	// remove the pointers
	pointsGraphList->clear();

	pointsGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));

	QString text = ui->QTextEdit_pointsList->toPlainText();
	QStringList arrayLine = text.split("\n", QString::SkipEmptyParts);

	for (int line = 0; line < arrayLine.length(); line++) { // * iterate over lines
		QString currentLine = arrayLine.at(line);

		if (currentLine.at(0) == "[") { // * configuration line
			// ! create a new graph to configure it
			pointsGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
			// * set defaults for graph
			pointsGraphList->last()->setAdaptiveSampling(false);
			pointsGraphList->last()->removeFromLegend();
			pointsGraphList->last()->setLineStyle(QCPGraph::lsNone);
			pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssDot);
			pointsGraphList->last()->setPen(QPen(QBrush(Qt::red), 3));

			QList<QString> parameterArray = currentLine.remove("[").remove("]").split(",", QString::SkipEmptyParts);

			for (const auto &parameter : parameterArray) {
				QString param = parameter.split("=").at(0);
				QString value = parameter.split("=").at(1);

				qDebug() << "Setting graph param " << param << " to " << value;

				if (param == "LineStyle") {
					if (value == "None") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsNone);
					} else if (value == "Line") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsLine);
					} else if (value == "IsImpulse") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsImpulse);
					} else if (value == "StepLeft") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsStepLeft);
					} else if (value == "StepCenter") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsStepCenter);
					} else if (value == "StepRight") {
						pointsGraphList->last()->setLineStyle(QCPGraph::lsStepRight);
					}

				} else if (param == "Pen") {
					// ? example: Pen=red;3
					QString color = value.split(";").at(0);
					QString width = value.split(";").at(1);

					if (color == "black") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::black), width.toInt()));
					} else if (color == "white") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::white), width.toInt()));
					} else if (color == "darkGray") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkGray), width.toInt()));
					} else if (color == "gray") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::gray), width.toInt()));
					} else if (color == "lightGray") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::lightGray), width.toInt()));
					} else if (color == "red") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::red), width.toInt()));
					} else if (color == "green") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::green), width.toInt()));
					} else if (color == "blue") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::blue), width.toInt()));
					} else if (color == "cyan") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::cyan), width.toInt()));
					} else if (color == "magenta") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::magenta), width.toInt()));
					} else if (color == "yellow") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::yellow), width.toInt()));
					} else if (color == "darkRed") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkRed), width.toInt()));
					} else if (color == "darkGreen") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkGreen), width.toInt()));
					} else if (color == "darkBlue") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkBlue), width.toInt()));
					} else if (color == "darkCyan") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkCyan), width.toInt()));
					} else if (color == "darkMagenta") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkMagenta), width.toInt()));
					} else if (color == "darkYellow") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::darkYellow), width.toInt()));
					} else if (color == "transparent") {
						pointsGraphList->last()->setPen(QPen(QBrush(Qt::transparent), width.toInt()));
					}
				} else if (param == "ScatterStyle") {
					if (value == "None") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssNone);
					} else if (value == "Dot") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssDot);
					} else if (value == "Cross") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssCross);
					} else if (value == "Circle") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssCircle);
					} else if (value == "CrossCircle") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssCrossCircle);
					} else if (value == "CrossSquare") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssCrossSquare);
					} else if (value == "Custom") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssCustom);
					} else if (value == "Diamond") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssDiamond);
					} else if (value == "Disc") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssDisc);
					} else if (value == "Peace") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssPeace);
					} else if (value == "Pixmap") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssPixmap);
					} else if (value == "Plus") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssPlus);
					} else if (value == "PlusCircle") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssPlusCircle);
					} else if (value == "PlusSquare") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssPlusSquare);
					} else if (value == "Square") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssSquare);
					} else if (value == "Star") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssStar);
					} else if (value == "Triangle") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssTriangle);
					} else if (value == "TriangleInverted") {
						pointsGraphList->last()->setScatterStyle(QCPScatterStyle::ssTriangleInverted);
					}

				} else if (param == "RemoveFromLegend") {
					if (value == "True") {
						pointsGraphList->last()->removeFromLegend();
					}

				} else if (param == "AdaptiveSampling") {
					if (value == "True") {
						pointsGraphList->last()->setAdaptiveSampling(true);
					} else if (value == "False") {
						pointsGraphList->last()->setAdaptiveSampling(false);
					}
				}
			} // iteration over parameter and value

		} else { // * append point to last graph
			QStringList coords = currentLine.split(",", QString::SkipEmptyParts);
			if (coords.length() == 2) {
				pointsGraphList->last()->addData(coords.at(0).toDouble(), coords.at(1).toDouble());
			}
		}
	}
	ui->customPlot->replot();
}

void MainWindow::QPushButton_deleteFunction_clicked() {
	QModelIndex selectedIndex = ui->QListWidget_functionList->currentIndex();

	// row is essentially the index of the selected item
	if (selectedIndex.row() != -1) {
		// delete from widget
		ui->QListWidget_functionList->takeItem(selectedIndex.row());
		// use pointer stored in list to it delete from the graph
		ui->customPlot->removeGraph((QCPGraph *) functionGraphList->at(selectedIndex.row()));
		functionGraphList->removeAt(selectedIndex.row()); // remove from list
		ui->customPlot->replot();
	} else {
		statusBarMsg("Please select a function");
	}
}
