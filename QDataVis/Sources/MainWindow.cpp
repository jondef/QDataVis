//
// Created by John on 12/06/2019.
//


#include "MainWindow.hpp"

#include <utility>

#define ASYNC 0
static std::mutex graphMutex;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::uiMainWindow) {
	ui->setupUi(this);

	setWindowIcon(QIcon(":/images/QDataVis"));

	plotWindow->setUpAxesPageConnections();
	plotWindow->setUpGeneralPageConnections();
	plotWindow->setUpTitlePageConnections();

	// * graph doubled clicked
	connect(ui->listWidget_PointGraphList, &QListWidget::itemDoubleClicked, this, &MainWindow::graphDoubleClicked);
	connect(ui->QListWidget_functionList, &QListWidget::itemDoubleClicked, this, &MainWindow::graphDoubleClicked);

	connect(pointGraphDialog, &PointWindow::currentGraphChanged, this, [this]() {
		ui->customPlot->replot();
	});
	connect(ui->pushButton_centerPlot, &QPushButton::clicked, ui->customPlot, &QCustomPlot_custom::centerPlot);

	// * points tab
	connect(ui->QPushButton_AddPointGraph, &QPushButton::clicked, this, &MainWindow::addPointsGraph);
	connect(ui->QPushButton_RemovePointGraph, &QPushButton::clicked, this, &MainWindow::removePointsGraph);

	// * function tab
	connect(ui->QLineEdit_addFunction, &QLineEdit::returnPressed, this, &MainWindow::addFunctionGraph);
	connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::removeFunctionGraph);
	connect(ui->spinBox_setGlobalPointDensity, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::globalPointDensityChanged);

	// * settings tab
	connect(ui->checkBox_settingsDarkMode, &QCheckBox::toggled, this, &MainWindow::updateColors);

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
								 "Git commit date: " GIT_COMMIT_DATE "\n"
								 "Git commit hash: " GIT_COMMIT_HASH "\n"
								 "Git branch: " GIT_BRANCH);
	});

	// emit the signal of the checkbox to update the plot colors
	emit(ui->checkBox_settingsDarkMode->toggled(false));
}


void MainWindow::graphDoubleClicked(QListWidgetItem *item) {
	pointGraphDialog->setGraph(item, item->data(Qt::UserRole).value<QCPGraph *>());
	pointGraphDialog->show();
}

MainWindow::~MainWindow() {
	delete ui;
	delete plotWindow;
	delete pointGraphDialog;
	delete mFunctionGraph;
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

inline void MainWindow::statusBarMsg(const QString &msg, int time) {
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

void MainWindow::addFunctionGraph() {
	QListWidgetItem *listWidgetItem = new QListWidgetItem();
	QString graphName = ui->QLineEdit_addFunction->text();

	ui->customPlot->addFunctionGraph(graphName, listWidgetItem);
	ui->QListWidget_functionList->addItem(listWidgetItem);
}

void MainWindow::removeFunctionGraph() {
	QListWidgetItem *selectedItem = ui->QListWidget_functionList->currentItem();
	if (selectedItem) {
		ui->customPlot->removeFunctionGraph(selectedItem->data(Qt::UserRole).value<Graph *>());
		delete selectedItem;
	}
}

void MainWindow::addPointsGraph() {
	QListWidgetItem *listWidgetItem = new QListWidgetItem();
	QString graphName = QString("Graph #%1").arg(ui->listWidget_PointGraphList->count() + 1);

	ui->customPlot->addPointsGraph(graphName, listWidgetItem);
	ui->listWidget_PointGraphList->addItem(listWidgetItem);
}

void MainWindow::removePointsGraph() {
	QListWidgetItem *selectedItem = ui->listWidget_PointGraphList->currentItem();
	if (selectedItem) {
		ui->customPlot->removePointsGraph(selectedItem->data(Qt::UserRole).value<Graph *>());
		delete selectedItem;
	}
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


static void PlotFunctions(QCPGraph *graphPointer, QString function, QVector<double> xArray) {
	BinaryTree tree(std::move(function));

	QVector<double> yArray = tree.calculateTree(xArray);


	std::lock_guard<std::mutex> lock(graphMutex);
	graphPointer->setData(xArray, yArray, true);
}
