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
			delete selectedItem->data(Qt::UserRole).value<QCPGraph *>();
			delete ui->listWidget_PointGraphList->currentItem();
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

	connect(ui->spinBox_setGlobalPointDensity, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::GraphParametersChanged);

	connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
	connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);

	connect(ui->actionProperties, &QAction::triggered, this, [this]() {
		plotWindow->show();
		plotWindow->raise(); // bring it to front
		plotWindow->activateWindow(); // select it
		plotWindow->setWindowState(plotWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive); // set to active
	});



	//ui->customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	//ui->customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

//	dumpObjectInfo();
//	qDebug() << Q_FUNC_INFO << "Item too large for memory, setting invisible";
//	qDebug() << __FILE__ <<  __LINE__ << Q_FUNC_INFO;
//	qDebug() << __FILE__ <<  __LINE__ << Q_FUNC_INFO;
	// Connect networkManager response to the handler
	//connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onResult);
	// We get the data, namely JSON file from a site on a particular url
	//networkManager->get(QNetworkRequest(QUrl("https://poloniex.com/public?command=returnChartData&currencyPair=USDT_BTC&start=1405699200&end=9999999999&period=14400")));
//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));

	// emit the signal of the checkbox to update the plot colors
	emit(ui->checkBox_settingsDarkMode->toggled(false));
}


MainWindow::~MainWindow() {
	delete ui;
}

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

void MainWindow::GraphParametersChanged() {
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


void MainWindow::onResult(QNetworkReply *reply) {
	// If there are no errors
	if (!reply->error()) {

		// So create an object Json Document, by reading into it all the data from the response
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
		QJsonArray array = document.array();

		QVector<double> date;
		QVector<double> avg;
		QVector<double> low;
		QVector<double> high;

		// Taking from the document root object
		QJsonObject root = document.object();
		for (auto &&transactions : array) {
			date.append(transactions.toObject().take("date").toInt());
			avg.append(transactions.toObject().take("weightedAverage").toDouble());
			high.append(transactions.toObject().take("high").toDouble());
			low.append(transactions.toObject().take("low").toDouble());
		}
		qDebug() << array.size();

		QPen graphPen;
		graphPen.setColor(QColor(colors.at(0)));
		graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)

		// * high
//		mFunctionGraph->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
//		mFunctionGraph->lastKey()->setData(date, high);
//		graphPen.setColor(QColor(colors.at(2)));
//		mFunctionGraph->lastKey()->setPen(graphPen); // apply color to graph
//		// * low
//		mFunctionGraph->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
//		mFunctionGraph->lastKey()->setData(date, low);
//		graphPen.setColor(QColor(colors.at(1)));
//		mFunctionGraph->lastKey()->setPen(graphPen); // apply color to graph
//		// * avg
//		mFunctionGraph->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
//		mFunctionGraph->lastKey()->setData(date, avg);
//		graphPen.setColor(QColor(colors.at(0)));
//		mFunctionGraph->lastKey()->setPen(graphPen); // apply color to graph

		ui->customPlot->replot();

		/* We find the object "departament", which is the very first in the root object.
		 * Use the keys() method gets a list of all objects and the first index
		 * Take away the name of the object on which we obtain its value
		 * */
//		ui->textEdit->append(root.keys().at(0) + ": " + root.value(root.keys().at(0)).toString());
//
//		// The second value prescribe line
//		QJsonValue jv = root.value("employees");
//		// If the value is an array, ...
//		if (jv.isArray()) {
//			// ... then pick from an array of properties
//			QJsonArray ja = jv.toArray();
//			// Going through all the elements of the array ...
//			for (int i = 0; i < ja.count(); i++) {
//				QJsonObject subtree = ja.at(i).toObject();
//				// Taking the values of the properties and last name by adding them to textEdit
//				ui->textEdit->append(subtree.value("firstName").toString() +
//									 " " +
//									 subtree.value("lastName").toString());
//			}
//		}
//		// At the end we take away the property of the number of employees of the department and also to output textEdit
//		ui->textEdit->append(QString::number(root.value("number").toInt()));
	}
	reply->deleteLater();
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


//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));
//	networkManager->get(QNetworkRequest(QUrl("http://www.evileg.ru/it_example.json")));



//	QNetworkRequest request(QUrl("https://httpbin.org/get"));
//	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//	connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(replyFini(QNetworkReply * )));
//	connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
//	manager->get(request);

void MainWindow::replyFini(QNetworkReply *reply) {
	QString answer = QString::fromUtf8(reply->readAll());
	qDebug() << "answer------------>" << answer;
	qDebug() << QSslSocket::supportsSsl();
	reply->deleteLater();
}
// todo: implement ui->customPlot->xAxis->setPadding(value);

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
