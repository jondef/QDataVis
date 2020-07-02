//
// Created by John on 12/06/2019.
//


#include "MainWindow.hpp"

#include <utility>

#define ASYNC 0
#define POINT_DENSITY 1000
static std::mutex graphMutex;

// use QSharedPointer...
Q_DECLARE_METATYPE(std::shared_ptr<QCPGraph>)

static const QList<QColor> colors = {
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


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::uiMainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon(":/images/QDataVis"));

	plotWindow->setUpAxesPageConnections();
	plotWindow->setUpGeneralPageConnections();
	plotWindow->setUpTitlePageConnections();

	connect(ui->pushButton_centerPlot, &QPushButton::clicked, this, [this]() {
		ui->customPlot->xAxis->setRange(-10, 10);
		ui->customPlot->yAxis->setRange(-10, 10);
		ui->customPlot->replot();
	});

	connect(ui->QPushButton_AddPointGraph, &QPushButton::clicked, this, [this]() {
		QListWidgetItem *pListWidgetItem = new QListWidgetItem();
		pListWidgetItem->setText(QString("Graph #%1").arg(ui->listWidget_PointGraphList->count() + 1));

		QCPGraph *graph = new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
		QVariant var;
		var.setValue(graph);
		pListWidgetItem->setData(Qt::UserRole, var);
		ui->listWidget_PointGraphList->addItem(pListWidgetItem);
	});

	connect(ui->QPushButton_RemovePointGraph, &QPushButton::clicked, this, [this]() {
		QListWidgetItem *selectedItem = ui->listWidget_PointGraphList->currentItem();
		if (selectedItem) {
			ui->customPlot->removeGraph(selectedItem->data(Qt::UserRole).value<QCPGraph *>());
			delete selectedItem;
			ui->customPlot->replot();
		}
	});

	connect(ui->listWidget_PointGraphList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
		pointGraphDialog->setGraph(item, item->data(Qt::UserRole).value<QCPGraph *>());
		pointGraphDialog->show();
	});

	connect(ui->QListWidget_functionList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
		pointGraphDialog->setGraph(item, item->data(Qt::UserRole).value<QCPGraph *>());
		pointGraphDialog->show();
	});

	connect(pointGraphDialog, &PointWindow::currentGraphChanged, this, [this]() {
		ui->customPlot->replot();
	});


//	connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(replotGraphsOnRangeChange(QMouseEvent * )));
//	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(replotGraphsOnRangeChange()));
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replotGraphsOnRangeChange(QCPRange)));
	// todo: hide the tracing things when user clicks, not when he moves mouse
//	connect(ui->customPlot, &QCustomPlot::mousePress, ui->customPlot, &MainWindow::traceGraph);
//	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(Test(QMouseEvent * )));

	connect(ui->checkBox_settingsDarkMode, &QCheckBox::toggled, this, &MainWindow::updateColors);

//	connect(ui->QPushButton_PlotPoints, &QPushButton::clicked, this, &MainWindow::QPushButton_PlotPoints_clicked);
	connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::QPushButton_deleteFunction_clicked);
	connect(ui->QLineEdit_addFunction, &QLineEdit::returnPressed, this, &MainWindow::QLineEdit_addFunction_returnPressed);

	connect(ui->spinBox_setGlobalPointDensity, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::globalPointDensityChanged);

	// * menubar connections
	connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
	connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);
	connect(ui->actionProperties, &QAction::triggered, this, [this]() {
		plotWindow->show();
		plotWindow->raise(); // bring it to front
		plotWindow->activateWindow(); // select it
		plotWindow->setWindowState(plotWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive); // set to active
	});
	connect(ui->actionHelp, &QAction::triggered, this, [this]() {
		QMessageBox::information(this, "Help", "Not implemented");
	});
	connect(ui->actionAbout, &QAction::triggered, this, [this]() {
		QMessageBox::information(this, "About",
								 "Version: " VERSION "\n"
								 "Build date: " BUILDDATE "\n"
								 "git commit date: " GIT_COMMIT_DATE "\n"
								 "git commit hash: " GIT_COMMIT_HASH "\n"
								 "Git branch: " GIT_BRANCH);
	});

	// emit the signal of the checkbox to update the plot colors
	emit(ui->checkBox_settingsDarkMode->toggled(false));
}


MainWindow::~MainWindow() {
	delete ui;
}

/**
 * This function enables or disables dark mode
 * @param checked: checkbox checked?
 */
void MainWindow::updateColors(bool checked) {
	setAutoFillBackground(true);
	if (checked) { // enabled dark mode // window = background, windowText = foreground
		QPalette pal = QApplication::palette();
		pal.setColor(QPalette::Window, QApplication::palette().color(foregroundRole()));
		pal.setColor(QPalette::WindowText, QApplication::palette().color(backgroundRole()));
		QApplication::setPalette(pal);
	} else { // dark mode disabled
		QApplication::setPalette(QApplication::style()->standardPalette());
	}
	ui->customPlot->updateColors();
	qDebug() << "Dark mode enabled:" << checked;
}

/**
 * This method iterates over plotted functions and checks if they are overriding
 * global point density. If they're not, re-plot them with the new density
 */
void MainWindow::globalPointDensityChanged(int density) {
//	auto functions = QList<QString>();
//	// save the functions in the added order
//	for (int i = 0; i < mFunctionGraph->length(); ++i) {
//		functions.append(mFunctionGraph->at(i)->property("Function string").toString());
//	}
//	ui->customPlot->clearGraphs();
//	mFunctionGraph->clear();
//
//	for (int j = 0; j < functions.length(); ++j) {
//		calculateAndDrawFunction(const_cast<QString &>(functions.at(j)));
//	}
//	ui->customPlot->replot();
}


QColor MainWindow::getGraphColor(int colorIndex) {
	// only take the last digit of the index
	return colors.at(colorIndex % 10);
}


void MainWindow::replotGraphsOnRangeChange(QCPRange range) {
	QVector<double> xArray = generateXArray(range.lower, range.upper, POINT_DENSITY);
	static QVector<double> yArray(xArray.length());

	for (QHash<QCPGraph *, BinaryTree *>::iterator i = mFunctionGraph->begin(); i != mFunctionGraph->end(); ++i) {
		yArray = i.value()->calculateTree(xArray);
		i.key()->setData(xArray, yArray);
	}
	ui->customPlot->replot();
}

void MainWindow::statusBarMsg(const QString &msg, int time) {
	ui->statusBar->showMessage(msg, time);
}

void MainWindow::savePlotImage() {
	// fixme: save as jpg doesn't correctly save in release mode
	QString savePathFilename = QFileDialog::getSaveFileName(this, tr("Save plot"), "", tr("*.jpg;;*.png;;*.bmp;;*.pdf"));

	if (savePathFilename.isEmpty()) {
		return;
	}
	QFile file(savePathFilename);

	if (!file.open(QIODevice::WriteOnly)) {
		QMessageBox::warning(nullptr, "Error", tr("\n Could not create image file on disk"));
	}
	QString ext = savePathFilename.mid(savePathFilename.length() - 4);
	bool savedOk = false;
	if (ext == ".png") {
		savedOk = ui->customPlot->savePng(savePathFilename, 0, 0, 3.0, 100);
	} else if (ext == ".jpg") {
		savedOk = ui->customPlot->saveJpg(savePathFilename, 0, 0, 3.0, 100);
	} else if (ext == ".bmp") {
		savedOk = ui->customPlot->saveBmp(savePathFilename, 0, 0, 3.0);
	} else if (ext == ".pdf") {
		savedOk = ui->customPlot->savePdf(savePathFilename, 0, 0, QCP::epAllowCosmetic, QString(""), QString("Title"));
	}
	if (savedOk) {
		statusBarMsg(QString("Successfully saved plot as a %1 file").arg(ext), 5000);
	}
}

void MainWindow::QLineEdit_addFunction_returnPressed() {
	QString text = ui->QLineEdit_addFunction->text();

	QVector<double> xArray = generateXArray(ui->customPlot->xAxis->range().lower, ui->customPlot->xAxis->range().upper, POINT_DENSITY);

	// ! GCPCurve has performance issues
	/*Graphs are used to display single-valued data.
	 * Single-valued means that there should only be one data point per unique key coordinate.
	 * In other words, the graph can't have loops. If you do want to plot non-single-valued curves,
	 * rather use the QCPCurve plottable.*/

#if ASYNC
	// send it to another thread
	m_Futures.push_back(std::async(std::launch::async, LoadMeshes, mFunctionGraph->lastKey(), function, xArray));
#else
	QCPGraph *graph = new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis);
	BinaryTree *tree = new BinaryTree(text);
	mFunctionGraph->insert(graph, tree);

	QVector<double> yArray = tree->calculateTree(xArray);

	graph->setData(xArray, yArray);
#endif

	graph->setName(text);
	// let the ranges scale themselves so graph 0 fits perfectly in the visible area:
	//mFunctionGraph->lastKey()->rescaleAxes(false);
	graph->addToLegend();

	QColor color = getGraphColor(mFunctionGraph->size() - 1);

	QPen graphPen;
	graphPen.setColor(color);
	graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)
	graph->setPen(graphPen); // apply color to graph
//	graph->setBrush(QBrush(QColor(0, 0, 255, 20))); // set background

	ui->customPlot->replot();

	// * add item to widget and set the appropriate icon color
	auto pixmap = QPixmap(16, 16);
	pixmap.fill(color);

	auto *item = new QListWidgetItem();
	item->setText(text);
	item->setIcon(QIcon(pixmap));
	QVariant variant;
	variant.setValue(graph);
	item->setData(Qt::UserRole, variant);
	ui->QListWidget_functionList->addItem(item);
}

//void MainWindow::QPushButton_PlotPoints_clicked() {
//	// * Function is connected to the plot all points button
//	// * It clears the points graphs, then replots everything
//	// * in the above text edit
//	// todo: save plotting parameters, so it doesn't always use
//	// todo: the default ones, and not required to type them in again every time
//	for (auto i : *pointsGraphList) {
//		// delete previously plotted graphs
//		ui->customPlot->removeGraph(i);
//	}
//	// remove the pointers
//	pointsGraphList->clear();
//
//	pointsGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
//
//	QString text = ui->QTextEdit_pointsList->toPlainText();
//	QStringList arrayLine = text.split("\n", QString::SkipEmptyParts);
//
//	for (int line = 0; line < arrayLine.length(); ++line) { // * iterate over lines
//		const QString &currentLine = arrayLine.at(line);
//
//		QStringList coords = currentLine.split(",", QString::SkipEmptyParts);
//		if (coords.length() == 2) {
//			pointsGraphList->last()->addData(coords.at(0).toDouble(), coords.at(1).toDouble());
//		}
//	}
//	ui->customPlot->replot();
//}


void MainWindow::QPushButton_deleteFunction_clicked() {
	QListWidgetItem *selectedItem = ui->QListWidget_functionList->currentItem();

	if (selectedItem == nullptr) {
		statusBarMsg("Please select a function");
		return;
	}
	mFunctionGraph->remove(selectedItem->data(Qt::UserRole).value<QCPGraph *>());
	ui->customPlot->removeGraph(selectedItem->data(Qt::UserRole).value<QCPGraph *>());
	delete selectedItem;
	ui->customPlot->replot();
}


QVector<double> MainWindow::generateXArray(double lowerLim, double upperLim, unsigned int length) {
	QVector<double> finalArray(length);

	double difference = upperLim - lowerLim;
	double increment = difference / (length - 1);

	for (unsigned int i = 0; i < length; i++) {
		finalArray[i] = lowerLim + increment * i;
	}
	return finalArray;
}


static void PlotFunctions(QCPGraph *graphPointer, QString function, QVector<double> xArray) {
	BinaryTree tree(std::move(function));

	QVector<double> yArray = tree.calculateTree(xArray);


	std::lock_guard<std::mutex> lock(graphMutex);
	graphPointer->setData(xArray, yArray, true);
}
