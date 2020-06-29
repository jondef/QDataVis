//
// Created by John on 12/06/2019.
//


#include "MainWindow.h"

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


//	connect(ui->QPushButton_FormattingHelp, &QPushButton::clicked, this, [this]() {
//
//		QElapsedTimer timer;
//		timer.start();
//
//		qDebug() << "The operation took" << timer.nsecsElapsed() << "nanoseconds";
//		qDebug() << "The operation took" << timer.elapsed() << "milliseconds";
//	});

//	connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(replotGraphsOnRangeChange(QMouseEvent * )));
//	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(replotGraphsOnRangeChange()));
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(replotGraphsOnRangeChange(QCPRange)));
	// todo: hide the tracing things when user clicks, not when he moves mouse
//	connect(ui->customPlot, &QCustomPlot::mousePress, ui->customPlot, &MainWindow::traceGraph);
//	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(Test(QMouseEvent * )));

	connect(ui->checkBox_settingsDarkMode, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) { // enable dark mode
			// from: https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle
			// set style
			qApp->setStyle(QStyleFactory::create("Fusion"));
			// increase font size for better reading
			QFont defaultFont = QApplication::font();
			defaultFont.setPointSize(defaultFont.pointSize());
			qApp->setFont(defaultFont);
			// modify palette to dark
			QPalette darkPalette;
			darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
			darkPalette.setColor(QPalette::WindowText, Qt::white);
			darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
			darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
			darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
			darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
			darkPalette.setColor(QPalette::ToolTipText, Qt::white);
			darkPalette.setColor(QPalette::Text, Qt::white);
			darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
			darkPalette.setColor(QPalette::Dark, QColor(35, 35, 35));
			darkPalette.setColor(QPalette::Shadow, QColor(20, 20, 20));
			darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
			darkPalette.setColor(QPalette::ButtonText, Qt::white);
			darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));
			darkPalette.setColor(QPalette::BrightText, Qt::red);
			darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
			darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
			darkPalette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(80, 80, 80));
			darkPalette.setColor(QPalette::HighlightedText, Qt::white);
			darkPalette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(127, 127, 127));
			qApp->setPalette(darkPalette);

			// set plot colors
			ui->customPlot->setBackground(QBrush(QColor(53, 53, 53)));
			QPen pen = ui->customPlot->xAxis->tickPen();
			QColor color = QColor(255, 255, 255);
			pen.setColor(QColor(255, 255, 255));
			QVector<QCPAxis *> axes = {ui->customPlot->xAxis, ui->customPlot->xAxis2, ui->customPlot->yAxis, ui->customPlot->yAxis2};
			for (auto &i : axes) {
				i->setTickPen(pen);
//				i->grid()->setPen(pen);
//				i->grid()->setSubGridPen(pen);
				i->setSubTickPen(pen);
				i->setBasePen(pen);
				i->setLabelColor(color);
				i->setTickLabelColor(color);
			}

			ui->customPlot->replot();
		} else {
			qApp->setPalette(QApplication::style()->standardPalette());
			QFont defaultFont = QApplication::font();
			defaultFont.setFamily(defaultFont.defaultFamily());
			defaultFont.setPointSize(8); // 8 is the default font size
			qApp->setFont(defaultFont);

			ui->customPlot->setBackground(QBrush(QColor(255, 255, 255)));
			QPen pen = ui->customPlot->xAxis->tickPen();
			QColor color = QColor(0, 0, 0);
			pen.setColor(QColor(0, 0, 0));
			QVector<QCPAxis *> axes = {ui->customPlot->xAxis, ui->customPlot->xAxis2, ui->customPlot->yAxis, ui->customPlot->yAxis2};
			for (auto &i : axes) {
				i->setTickPen(pen);
//				i->grid()->setPen(pen);
//				i->grid()->setSubGridPen(pen);
				i->setSubTickPen(pen);
				i->setBasePen(pen);
				i->setLabelColor(color);
				i->setTickLabelColor(color);
			}

			ui->customPlot->replot();
		}
	});

//	connect(ui->QPushButton_PlotPoints, &QPushButton::clicked, this, &MainWindow::QPushButton_PlotPoints_clicked);
	connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::QPushButton_deleteFunction_clicked);
	connect(ui->QLineEdit_addFunction, &QLineEdit::returnPressed, this, &MainWindow::QLineEdit_addFunction_returnPressed);

	connect(ui->spinBox_setGlobalPointDensity, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::GraphParametersChanged);

	connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
	connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);

	connect(ui->actionProperies, &QAction::triggered, this, [this]() {
		plotWindow->show();
		plotWindow->raise(); // bring it to front
		plotWindow->activateWindow(); // select it
		plotWindow->setWindowState(plotWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive); // set to active
	});

	// todo: add ui->customPlot->xAxis->antialiased();
	// todo: add theme https://github.com/Jorgen-VikingGod/Qt-Frameless-Window-DarkStyle
	// todo: add theme https://stackoverflow.com/questions/15035767/is-the-qt-5-dark-fusion-theme-available-for-windows


	//ui->customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	//ui->customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"

	setUpAxesPageConnections();
	setUpGeneralPageConnections();
	setUpTitlePageConnections();


	connect(plotWindow, &PlotPropertiesWindow::windowClosed, this, [this]() {
		qDebug() << "saved";
	});
//	dumpObjectInfo();
//	qDebug() << Q_FUNC_INFO << "Item too large for memory, setting invisible";
//	qDebug() << __FILE__ <<  __LINE__ << Q_FUNC_INFO;
//	qDebug() << __FILE__ <<  __LINE__ << Q_FUNC_INFO;
	// Connect networkManager response to the handler
	//connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onResult);
	// We get the data, namely JSON file from a site on a particular url
	//networkManager->get(QNetworkRequest(QUrl("https://poloniex.com/public?command=returnChartData&currencyPair=USDT_BTC&start=1405699200&end=9999999999&period=14400")));
//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));
}


MainWindow::~MainWindow() {
	delete ui;
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


inline void MainWindow::setUpAxesPageConnections() {
	// * set axis visible
	connect(plotWindow->ui->groupBox_xAxis_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->setVisible(checked);
		ui->customPlot->replot();
	});

	// * set label text
	connect(plotWindow->ui->lineEdit_xAxis_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		ui->customPlot->xAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		ui->customPlot->yAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_xAxis2_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		ui->customPlot->xAxis2->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis2_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		ui->customPlot->yAxis2->setLabel(text);
		ui->customPlot->replot();
	});

	// * set label color
	connect(plotWindow->ui->pushButton_xAxis_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->xAxis->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->yAxis->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->xAxis2->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->yAxis2->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});

	// * set label font
	connect(plotWindow->ui->pushButton_xAxis_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->xAxis->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->yAxis->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->xAxis2->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->yAxis2->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});

	// * set label padding
	connect(plotWindow->ui->spinBox_xAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});

	// ! TICK LABEL //
	// * set ticks visible
	connect(plotWindow->ui->groupBox_xAxis_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});

	// * set tick label color
	connect(plotWindow->ui->pushButton_xAxis_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->xAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->yAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->xAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->yAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});

	// * set tick label font
	connect(plotWindow->ui->pushButton_xAxis_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->xAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->yAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->xAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			ui->customPlot->yAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});

	// * set tick label rotation
	connect(plotWindow->ui->doubleSpinBox_xAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		ui->customPlot->xAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		ui->customPlot->yAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_xAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		ui->customPlot->xAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		ui->customPlot->yAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});

	// * set tick label padding
	connect(plotWindow->ui->spinBox_xAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setTickLabelPadding(value);
		ui->customPlot->replot();
	});

	// * set tick label position
	connect(plotWindow->ui->comboBox_xAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_xAxis_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_xAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->xAxis->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_yAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_yAxis_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_yAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->yAxis->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_xAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_xAxis2_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_xAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->xAxis2->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_yAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_yAxis2_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_yAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->yAxis2->setTickLabelSide(x);
		ui->customPlot->replot();
	});


	// ! TICK //
	// * set tick visible
	connect(plotWindow->ui->groupBox_xAxis_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->setTicks(checked);
		ui->customPlot->replot();
	});


	// * set tick pen
	connect(plotWindow->ui->pushButton_xAxis_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set tick length in
	connect(plotWindow->ui->spinBox_xAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setTickLengthIn(value);
		ui->customPlot->replot();
	});

	// * set tick length out
	connect(plotWindow->ui->spinBox_xAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setTickLengthOut(value);
		ui->customPlot->replot();
	});

	// * set sub-ticks visible
	connect(plotWindow->ui->groupBox_xAxis_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->setSubTicks(checked);
		ui->customPlot->replot();
	});

	// * set sub-tick pen
	connect(plotWindow->ui->pushButton_xAxis_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set sub-tick length in
	connect(plotWindow->ui->spinBox_xAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});

	// * set sub-tick length out
	connect(plotWindow->ui->spinBox_xAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->xAxis2->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		ui->customPlot->yAxis2->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});

	// * set grid visible
	connect(plotWindow->ui->groupBox_xAxis_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->grid()->setVisible(checked);
		ui->customPlot->replot();
	});

	// * set grid pen
	connect(plotWindow->ui->pushButton_xAxis_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set zero line pen
	connect(plotWindow->ui->pushButton_xAxis_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	// todo: check antialiasing boxes at init?
	// * set grid antialiasing
	connect(plotWindow->ui->checkBox_xAxis_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});

	// * set zero line antialisaing
	connect(plotWindow->ui->checkBox_xAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});

	// * set sub-grid visible
	connect(plotWindow->ui->groupBox_xAxis_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});

	// * set sub-grid pen
	connect(plotWindow->ui->pushButton_xAxis_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set sub-grid antialiasing
	connect(plotWindow->ui->checkBox_xAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});

	// * select axis ticker type (pi, date, etc...)
	connect(plotWindow->ui->comboBox_xAxis_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis, value);
		plotWindow->ui->checkBox_xAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_xAxis2_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis2, value);
		plotWindow->ui->checkBox_xAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis, value);
		plotWindow->ui->checkBox_yAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis2_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis2, value);
		plotWindow->ui->checkBox_yAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});


	// * set logarithmic scale
	connect(plotWindow->ui->checkBox_xAxis_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});

	// * set base pen
	connect(plotWindow->ui->pushButton_xAxis_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->xAxis2->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			ui->customPlot->yAxis2->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	plotWindow->ui->groupBox_xAxis_setGridVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setGridVisible->setChecked(true);

	plotWindow->ui->groupBox_xAxis_setVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setVisible->setChecked(true);

	plotWindow->ui->groupBox_xAxis_setTickLabelVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setTickLabelVisible->setChecked(true);
	plotWindow->ui->groupBox_xAxis2_setTickLabelVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis2_setTickLabelVisible->setChecked(true);

	plotWindow->ui->groupBox_xAxis_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_xAxis2_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis2_setTicksVisible->setChecked(true);

	plotWindow->ui->groupBox_xAxis_setSubTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setSubTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_xAxis2_setSubTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis2_setSubTicksVisible->setChecked(true);
}


void MainWindow::changeAxisTicker(QCPAxis *axis, const QString &value) {
	if (value == "QCPAxisTicker") {
		QSharedPointer<QCPAxisTicker> fixedTicker(new QCPAxisTicker);
		axis->setTicker(fixedTicker);
	} else if (value == "QCPAxisTickerPi") {
		QSharedPointer<QCPAxisTickerPi> fixedTicker(new QCPAxisTickerPi);
		axis->setTicker(fixedTicker);
	} else if (value == "QCPAxisTickerDateTime") {
		QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//		dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
		axis->setTicker(dateTicker);
//		double now = QDateTime::currentDateTime().toTime_t();
//		axis->setRange(now - now / 10, now);
//		axis->setRange(0, 10000);
	} else if (value == "QCPAxisTickerFixed") {
		QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
		axis->setTicker(fixedTicker);
	} else if (value == "QCPAxisTickerLog") {
		QSharedPointer<QCPAxisTickerLog> fixedTicker(new QCPAxisTickerLog);
		axis->setTicker(fixedTicker);
	} else if (value == "QCPAxisTickerText") {
		QSharedPointer<QCPAxisTickerText> fixedTicker(new QCPAxisTickerText);
		axis->setTicker(fixedTicker);
	} else if (value == "QCPAxisTickerTime") {
		QSharedPointer<QCPAxisTickerTime> fixedTicker(new QCPAxisTickerTime);
		axis->setTicker(fixedTicker);
	}
	ui->customPlot->replot();
}


inline void MainWindow::setUpTitlePageConnections() {
	// * add title button clicked
	connect(plotWindow->ui->pushButton_addTitle, &QPushButton::clicked, this, [this]() {
		plotWindow->ui->listWidget_titleList->addItem("Title");

		graphTextElements->insert(plotWindow->ui->listWidget_titleList->item(plotWindow->ui->listWidget_titleList->count() - 1),
								  new QCPTextElement(ui->customPlot, "Title", plotWindow->titleFontDialog->currentFont()));

		// make each item editable by double clicking
		QListWidgetItem *item = plotWindow->ui->listWidget_titleList->item(plotWindow->ui->listWidget_titleList->count() - 1);
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		// add title instantly
		ui->customPlot->plotLayout()->insertRow(graphTextElements->count());
		ui->customPlot->plotLayout()->addElement(graphTextElements->count(), 0, graphTextElements->value(
				plotWindow->ui->listWidget_titleList->item(plotWindow->ui->listWidget_titleList->count() - 1)));

		ui->customPlot->replot();
	});

	// * when user renamed listWidgetItem
	connect(plotWindow->ui->listWidget_titleList, &QListWidget::itemChanged, this, [this](QListWidgetItem *changedItem) {
		graphTextElements->value(changedItem)->setText(changedItem->text());
		ui->customPlot->replot();
	});

	// * delete title
	connect(plotWindow->ui->pushButton_removeTitle, &QPushButton::clicked, this, [this]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			ui->customPlot->plotLayout()->remove(graphTextElements->value(selectedItem));
			ui->customPlot->plotLayout()->simplify();

			graphTextElements->remove(selectedItem);
			delete selectedItem;

			ui->customPlot->replot();
		}
	});

	// * update the widgets on the right when a title is clicked
	connect(plotWindow->ui->listWidget_titleList, &QListWidget::itemClicked, this, [this]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			QCPTextElement *titleElement = graphTextElements->value(selectedItem);
			// update QFontWidget
			plotWindow->titleFontDialog->setCurrentFont(titleElement->font());
			// update QColorWidget
			plotWindow->titleColorDialog->setCurrentColor(titleElement->textColor());
		}
	});

	// * when user changed font in QFontDialog - any font option
	connect(plotWindow->titleFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setFont(plotWindow->titleFontDialog->currentFont());
			ui->customPlot->replot();
		}
	});

	// * when user changed the color in QColorDialog
	connect(plotWindow->titleColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setTextColor(plotWindow->titleColorDialog->currentColor());
			//graphTextElements->value(plotWindow->ui->listWidget_titleList->item(plotWindow->ui->listWidget_titleList->count() - 1))->setTextColor(QColorDialog::getColor());
			ui->customPlot->replot();
		}
	});

	// * Title order changed
	connect(plotWindow->ui->listWidget_titleList->model(), &QAbstractItemModel::rowsMoved, this, [this]() {
		// remove all of the titles except the plot
		for (int j = 0; j < graphTextElements->count() + 1; ++j) {
			if (plotWindow->ui->listWidget_titleList->item(j)->text() == "Plot") {
				continue;
			}
			ui->customPlot->plotLayout()->take(graphTextElements->value(plotWindow->ui->listWidget_titleList->item(j)));
		}
		ui->customPlot->plotLayout()->simplify();

		int index = 0;

		// ! count() + 1 because we need to account for the plot item.
		for (int i = 0; i < graphTextElements->count() + 1; ++i) {
			// check if current item is the plot item
			if (plotWindow->ui->listWidget_titleList->item(i)->text() == "Plot") {
				index++;
				continue;
			}
			ui->customPlot->plotLayout()->insertRow(index);
			ui->customPlot->plotLayout()->addElement(index, 0, graphTextElements->value(
					plotWindow->ui->listWidget_titleList->item(i)));
			index++;
		}
		ui->customPlot->replot();
	});
}


inline void MainWindow::setUpGeneralPageConnections() {
	connect(plotWindow->ui->checkBox_MultiSelect, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iMultiSelect);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iMultiSelect);
		}
	});

	connect(plotWindow->ui->checkBox_RangeDrag, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iRangeDrag);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iRangeDrag);
		}
	});

	connect(plotWindow->ui->checkBox_RangeZoom, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iRangeZoom);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iRangeZoom);
		}
	});

	connect(plotWindow->ui->checkBox_SelectPlottables, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectPlottables);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectPlottables);
		}
	});

	connect(plotWindow->ui->checkBox_SelectAxes, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectAxes);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectAxes);
		}
	});

	connect(plotWindow->ui->checkBox_SelectItems, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectItems);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectItems);
		}
	});

	connect(plotWindow->ui->checkBox_SelectLegend, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectLegend);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectLegend);
		}
	});

	connect(plotWindow->ui->checkBox_SelectOther, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectOther);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectOther);
		}
	});

	// * set background color
	connect(plotWindow->ui->pushButton_setBackgroundColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->background().toImage().pixelColor(0, 0)); // fixme: doesn't work
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			ui->customPlot->setBackground(QBrush(popUpColorDialog->currentColor()));
			ui->customPlot->replot();
		});
	});

	// * set cursor
	connect(plotWindow->ui->checkBox_enableCursor, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), ui->customPlot, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			ui->customPlot->cursor.cursorText->setVisible(true);
		} else {
			disconnect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), ui->customPlot, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			ui->customPlot->cursor.hLine->start->setCoords(0, 0);
			ui->customPlot->cursor.hLine->end->setCoords(0, 0);

			ui->customPlot->cursor.vLine->start->setCoords(0, 0);
			ui->customPlot->cursor.vLine->end->setCoords(0, 0);

			ui->customPlot->cursor.cursorText->setVisible(false);
			ui->customPlot->layer("cursorLayer")->replot();
		}
	});

	// * set stick axis to zero line
	connect(plotWindow->ui->checkBox_stickAxisToZeroLines, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			connect(ui->customPlot, SIGNAL(beforeReplot()), ui->customPlot, SLOT(stickAxisToZeroLines()));
			ui->customPlot->replot();
		} else {
			disconnect(ui->customPlot, SIGNAL(beforeReplot()), ui->customPlot, SLOT(stickAxisToZeroLines()));
			ui->customPlot->axisRect()->setAutoMargins(QCP::msAll);
			ui->customPlot->xAxis->setOffset(0);
			ui->customPlot->yAxis->setOffset(0);
			ui->customPlot->replot();
		}
	});

	emit(plotWindow->ui->checkBox_stickAxisToZeroLines->toggled(false));
	emit(plotWindow->ui->checkBox_enableCursor->toggled(false));
	plotWindow->ui->checkBox_enableCursor->setChecked(false);
	plotWindow->ui->checkBox_RangeDrag->setChecked(true);
	plotWindow->ui->checkBox_RangeZoom->setChecked(true);
	plotWindow->ui->checkBox_MultiSelect->setChecked(true);
	plotWindow->ui->checkBox_SelectPlottables->setChecked(true);
	plotWindow->ui->checkBox_SelectAxes->setChecked(true);
	plotWindow->ui->checkBox_SelectLegend->setChecked(true);
	plotWindow->ui->checkBox_SelectItems->setChecked(true);
	plotWindow->ui->checkBox_SelectOther->setChecked(true);
}