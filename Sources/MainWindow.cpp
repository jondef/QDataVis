//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	initGraph();

	// ! CONNECTIONS
	connect(ui->QPushButton_PlotPoints, SIGNAL(clicked()), this, SLOT(QPushButton_PlotPoints_clicked()));
	connect(ui->QPushButton_deleteFunction, SIGNAL(clicked()), this, SLOT(QPushButton_deleteFunction_clicked()));
	connect(ui->QLineEdit_addFunction, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));
	connect(ui->QLineEdit_functionParam, SIGNAL(returnPressed()), this, SLOT(QLineEdit_addFunction_returnPressed()));
	connect(ui->actionQuit, SIGNAL(triggered()), QApplication::instance(), SLOT(quit()));
	connect(ui->actionSave_as, SIGNAL(triggered()), this, SLOT(savePlotImage()));
}


MainWindow::~MainWindow() {
	delete ui;
}


void MainWindow::initGraph() {
	// enable openGL
	ui->customPlot->setOpenGl(true, 16); // enable openGL
	statusBarMsg(ui->customPlot->openGl() ? "openGL enabled" : "openGL disabled", 5000);

	// configure tle title
	ui->customPlot->plotLayout()->insertRow(0);
	QCPTextElement *title = new QCPTextElement(ui->customPlot, "Plot", QFont("sans", 17, QFont::Bold));
	ui->customPlot->plotLayout()->addElement(0, 0, title);

	// axes configuration
	ui->customPlot->xAxis->setLabel("x Axis");
	ui->customPlot->yAxis->setLabel("y Axis");
	// set axes ranges
	ui->customPlot->xAxis->setRange(-10, 10);
	ui->customPlot->yAxis->setRange(-10, 10);
	// configure right and top axis to show ticks but no labels:
	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
	ui->customPlot->xAxis2->setVisible(true);
	ui->customPlot->xAxis2->setTickLabels(false); // show numbers
	ui->customPlot->yAxis2->setVisible(true);
	ui->customPlot->yAxis2->setTickLabels(false); // show numbers

	// legend initialization
	ui->customPlot->legend->setVisible(true);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui->customPlot->legend->setFont(legendFont);
	ui->customPlot->legend->setSelectedFont(legendFont);
	ui->customPlot->legend->setSelectableParts(
			QCPLegend::spItems); // legend box shall not be selectable, only legend items

	// Set interations
	ui->customPlot->setInteractions(
			QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);


	// ! GRAPH RELATED CONNECTIONS
	// * axes configuration
	// connect slot that ties some axis selections together (especially opposite axes):
	connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(plotOppositeAxesConnection()));
	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(plotAxisLockDrag()));
	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(plotAxisLockZoom()));
	// make bottom and left axes transfer their ranges to top and right axes:
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	// * click interaction
	// axis label double click
	connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis * , QCPAxis::SelectablePart, QMouseEvent * )), this,
			SLOT(plotAxisLabelDoubleClick(QCPAxis * , QCPAxis::SelectablePart)));
	// legend double click
	connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend * , QCPAbstractLegendItem * , QMouseEvent * )), this,
			SLOT(plotLegendGraphDoubleClick(QCPLegend * , QCPAbstractLegendItem * )));
	// title double click
	connect(title, SIGNAL(doubleClicked(QMouseEvent * )), this, SLOT(plotTitleDoubleClicked(QMouseEvent * )));
	// graph clicked statusbar message
	connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable * , int, QMouseEvent * )), this,
			SLOT(plotGraphClicked(QCPAbstractPlottable * , int)));

	// * context menu
	// setup policy and connect slot for context menu popup:
	ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(plotContextMenuRequest(QPoint)));


	ui->customPlot->replot();
}


void MainWindow::statusBarMsg(QString msg, int time) {
	ui->statusBar->showMessage(msg, time);
}


void MainWindow::savePlotImage() {
	QString save_path_filename = QFileDialog::getSaveFileName(this, tr("Save plot"), "",
															  tr("*.jpg;;*.png;;*.bmp;;*.pdf"));

	if (save_path_filename.isEmpty()) {
		return;
	}

	QFile file(save_path_filename);

	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(nullptr, "Error", tr("\n Could not create image file on disk"));
	}

	QString ext = save_path_filename.mid(save_path_filename.length() - 4);
	bool savedOk = false;
	if (ext == ".png") {
		savedOk = ui->customPlot->savePng(save_path_filename, 0, 0, 3.0, 100);
	} else if (ext == ".jpg") {
		savedOk = ui->customPlot->saveJpg(save_path_filename, 0, 0, 3.0, 100);
	} else if (ext == ".bmp") {
		savedOk = ui->customPlot->saveBmp(save_path_filename, 0, 0, 3.0);
	} else if (ext == ".pdf") {
		savedOk = ui->customPlot->savePdf(save_path_filename, 0, 0, QCP::epAllowCosmetic, QString(""),
										  QString("Title"));
	}

	if (savedOk) {
		statusBarMsg(QString("Successfully saved plot as a %1 file").arg(ext), 5000);
	}
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

	// ? GCPCurve has performance issues
	/*Graphs are used to display single-valued data.
	 * Single-valued means that there should only be one data point per unique key coordinate.
	 * In other words, the graph can't have loops. If you do want to plot non-single-valued curves,
	 * rather use the QCPCurve plottable.*/
	functionGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
	functionGraphList->last()->setData(xArray, yArray);
	functionGraphList->last()->setName(text);
	// let the ranges scale themselves so graph 0 fits perfectly in the visible area:
	functionGraphList->last()->rescaleAxes(true);
	functionGraphList->last()->addToLegend();

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
	//functionGraphList->last()->setBrush(QBrush(QColor(0, 0, 255, 20))); // set background
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

void MainWindow::plotTitleDoubleClicked(QMouseEvent *event) {
	Q_UNUSED(event)
	if (QCPTextElement *title = qobject_cast<QCPTextElement *>(sender())) {
		// Set the plot title by double clicking on it
		bool ok;
		QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal,
												 title->text(), &ok);
		if (ok) {
			title->setText(newTitle);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part) {
	// Set an axis label by double clicking on it
	if (part ==
		QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal,
												 axis->label(), &ok);
		if (ok) {
			axis->setLabel(newLabel);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item) {
	// Rename a graph by double clicking on its legend item
	Q_UNUSED(legend)
	if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
	{
		QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem *>(item);
		bool ok;
		QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal,
												plItem->plottable()->name(), &ok);
		if (ok) {
			plItem->plottable()->setName(newName);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::plotMoveLegend() {
	if (QAction *contextAction = qobject_cast<QAction *>(
			sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok) {
			ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment) dataInt);
			ui->customPlot->replot();
		}
	}
}

void MainWindow::plotContextMenuRemoveAllGraphs() {
	ui->customPlot->clearGraphs();
	ui->customPlot->replot();
}

void MainWindow::plotContextMenuRemoveSelectedGraph() {
	if (ui->customPlot->selectedGraphs().size() > 0) {
		ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
		ui->customPlot->replot();
	}
}

void MainWindow::plotGraphClicked(QCPAbstractPlottable *plottable, int dataIndex) {
	// since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
	// usually it's better to first check whether interface1D() returns non-zero, and only then use it.
	double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
	QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(
			dataIndex).arg(dataValue);
	ui->statusBar->showMessage(message, 2500);
}

void MainWindow::plotAxisLockDrag() {
	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, both directions may be dragged

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}


void MainWindow::plotAxisLockZoom() {
	// if an axis is selected, only allow the direction of that axis to be zoomed
	// if no axis is selected, both directions may be zoomed

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}


void MainWindow::plotContextMenuRequest(QPoint pos) {
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{
		menu->addAction("Move to top left", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignBottom | Qt::AlignLeft));
	} else  // general context menu on graphs requested
	{
		menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
		if (ui->customPlot->selectedGraphs().size() > 0)
			menu->addAction("Remove selected graph", this, SLOT(plotContextMenuRemoveSelectedGraph()));
		if (ui->customPlot->graphCount() > 0)
			menu->addAction("Remove all graphs", this, SLOT(plotContextMenuRemoveAllGraphs()));
	}

	menu->popup(ui->customPlot->mapToGlobal(pos));
}


void MainWindow::plotOppositeAxesConnection() {
	/*
	 normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
	 the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
	 and the axis base line together. However, the axis label shall be selectable individually.

	 The selection state of the left and right axes shall be synchronized as well as the state of the
	 bottom and top axes.

	 Further, we want to synchronize the selection of the graphs with the selection state of the respective
	 legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
	 or on its legend item.
	*/

	// make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
		ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
		ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}
	// make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
		ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
		ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	// synchronize selection of graphs with selection of corresponding legend items:
	for (int i = 0; i < ui->customPlot->graphCount(); ++i) {
		QCPGraph *graph = ui->customPlot->graph(i);
		QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected()) {
			item->setSelected(true);
			graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
		}
	}
}
