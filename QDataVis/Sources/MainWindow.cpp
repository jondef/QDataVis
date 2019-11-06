//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"
#include "exprtk.hpp"
#include "Resources/icon.xpm" // import icon as static const array *

#define ASYNC 0
static std::mutex graphMutex;


static void LoadMeshes(QCPGraph *graphPointer, QString function, QVector<double> xArray) {
	BinaryTree tree(function);

	QVector<double> *yArray = new QVector<double>();
	*yArray = tree.calculateTree(xArray);

	std::lock_guard<std::mutex> lock(graphMutex);
	graphPointer->setData(xArray, *yArray);
}

const QList<QColor> colors = {
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


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	// save icon in exe // https://convertio.co/png-xpm/
	// comes from the xpm icon file
	setWindowIcon(QIcon(QPixmap(window_icon_xpm)));

	initGraph();

	connect(ui->pushButton_centerPlot, &QPushButton::clicked, this, [=]() {
		ui->customPlot->xAxis->setRange(-10, 10);
		ui->customPlot->yAxis->setRange(-10, 10);
		ui->customPlot->replot();
	});


	connect(ui->QPushButton_FormattingHelp, &QPushButton::clicked, this, [=]() {

		QElapsedTimer timer;
		timer.start();

		qDebug() << "The operation took" << timer.nsecsElapsed() << "nanoseconds";
		qDebug() << "The operation took" << timer.elapsed() << "milliseconds";
	});

	connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(Test(QMouseEvent * )));
	connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(replotGraphsOnMouseMove(QMouseEvent * )));
	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(replotGraphsOnMouseMove()));
	// todo: hide the tracing things when user clicks, not when he moves mouse
	connect(ui->customPlot, &QCustomPlot::mousePress, this, &MainWindow::Test);
//	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(Test(QMouseEvent * )));

	connect(ui->checkBox_settingsDarkMode, &QCheckBox::toggled, this, [=](bool checked) {
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

	connect(ui->QPushButton_PlotPoints, &QPushButton::clicked, this, &MainWindow::QPushButton_PlotPoints_clicked);
	connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::QPushButton_deleteFunction_clicked);
	connect(ui->QLineEdit_addFunction, &QLineEdit::returnPressed, this, &MainWindow::QLineEdit_addFunction_returnPressed);

	connect(ui->spinBox_setGraphMinimum, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::GraphParametersChanged);
	connect(ui->spinBox_setGraphMaximum, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::GraphParametersChanged);
	connect(ui->spinBox_setGraphLength, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::GraphParametersChanged);

	connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
	connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);
//	connect(ui->QPushButton_FormattingHelp, &QPushButton::clicked, this, &MainWindow::Test);
	connect(ui->actionProperies, &QAction::triggered, this, [=]() {
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


	connect(plotWindow, &PlotPropertiesWindow::windowClosed, this, [=]() {
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


void MainWindow::onMouseMoveReplotCursor(QMouseEvent *event) {
	// Cursor coordinates:
	double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
	double y = ui->customPlot->yAxis->pixelToCoord(event->pos().y());

	//double dataIndex = mFunctionGraph->lastKey()->findBegin(x);
//	qDebug() << dataIndex << mFunctionGraph->lastKey()->dataMainValue(dataIndex);

	ui->customPlot->manageCursor(x, y);
	if (USING_LAYER)
		ui->customPlot->layer("cursorLayer")->replot();
	else
		ui->customPlot->replot();

	// todo: add tracing option only on selected graph
//	double mouseX = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
//	//for the  graph values #####
//	for (int i = 0; i < ui->customPlot->graphCount(); ++i) {
//		int index = ui->customPlot->graph(i)->findBegin(mouseX);
//		double x = ui->customPlot->graph(i)->dataMainKey(index);
//		double y = ui->customPlot->graph(i)->dataMainValue(index);
//
//		if (mouseX >= 0) {
//			qDebug() << index << "X:" << x << "Y:" << y;
//		} else {
//			qDebug() << index << "X:" << 0 << "Y:" << 0;
//		}
//	}
	ui->customPlot->replot();


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


void MainWindow::calculateAndDrawFunction(QString &function) {
	int min = ui->spinBox_setGraphMinimum->value();
	int max = ui->spinBox_setGraphMaximum->value();
	int len = ui->spinBox_setGraphLength->value();

	QVector<double> xArray = generateXArray(min, max, len);
//	QVector<double> yArray = tree.calculateTree(xArray, ui->progressBar);

	// ! GCPCurve has performance issues
	/*Graphs are used to display single-valued data.
	 * Single-valued means that there should only be one data point per unique key coordinate.
	 * In other words, the graph can't have loops. If you do want to plot non-single-valued curves,
	 * rather use the QCPCurve plottable.*/

#if ASYNC
	// send it to another thread
	m_Futures.push_back(std::async(std::launch::async, LoadMeshes, mFunctionGraph->lastKey(), function, xArray));
#else

	BinaryTree *tree = new BinaryTree(function);

	mFunctionGraph->insert(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis), tree);

	QVector<double> yArray = tree->calculateTree(xArray);

	mFunctionGraph->lastKey()->setData(xArray, yArray);
#endif
//	for (QHash<QCPGraph *, BinaryTree *>::iterator i = mFunctionGraph->begin(); i != mFunctionGraph->end(); ++i)
//	mFunctionGraph->lastKey()->setData(xArray, yArray);
	mFunctionGraph->lastKey()->setName(function);
	mFunctionGraph->lastKey()->setProperty("Function string", function);
	// let the ranges scale themselves so graph 0 fits perfectly in the visible area:
	//mFunctionGraph->lastKey()->rescaleAxes(false);
	mFunctionGraph->lastKey()->addToLegend();

	QColor color = getGraphColor(mFunctionGraph->size() - 1);

	QPen graphPen;
	graphPen.setColor(color);
	graphPen.setWidthF(2); // between 1 and 2 acceptable (float/int)
	mFunctionGraph->lastKey()->setPen(graphPen); // apply color to graph
	//mFunctionGraph->lastKey()->setBrush(QBrush(QColor(0, 0, 255, 20))); // set background
}

QColor MainWindow::getGraphColor(int colorIndex) {
	// only take the last number of the index if bigger than 10
	if (colorIndex > 9) {
		QString str = QString::number(colorIndex);
		colorIndex = str.mid(str.length() - 1).toInt();
	}
	return QColor(colors.at(colorIndex));
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

void MainWindow::stickAxisToZeroLines() {
	ui->customPlot->axisRect()->setAutoMargins(QCP::msNone);

	int pxx = ui->customPlot->yAxis->coordToPixel(0);
	int pxy = ui->customPlot->xAxis->coordToPixel(0);
	ui->customPlot->xAxis->setOffset(-ui->customPlot->axisRect()->height() - ui->customPlot->axisRect()->top() + pxx);
	ui->customPlot->yAxis->setOffset(ui->customPlot->axisRect()->left() - pxy);
}

void MainWindow::replotGraphsOnMouseMove(QMouseEvent *event) {
	QVector<double> xArray = generateXArray(ui->customPlot->xAxis->range().lower, ui->customPlot->xAxis->range().upper, 1000);
	// mouse is moving because this function is connected to MouseMove() signal
	// and left button is held down => User is dragging the plot
	// if event == nullptr, it is called by mouseWheel() signal
	if (event == nullptr || event->buttons() == Qt::LeftButton) {
		for (QMap<QCPGraph *, BinaryTree *>::iterator i = mFunctionGraph->begin(); i != mFunctionGraph->end(); ++i) {
			QVector<double> yArray = i.value()->calculateTree(xArray);
			i.key()->setData(xArray, yArray);
		}
	}
}


void MainWindow::Test(QMouseEvent *event) {
	if (ui->customPlot->selectedGraphs().size() == 1) {
		double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
		double y = ui->customPlot->yAxis->pixelToCoord(event->pos().y());
//		QPointF pp = ui->customPlot->cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
//		double plottable = ui->customPlot->selectedGraphs().first()->selectTest(point, true);
		QCPGraphDataContainer::const_iterator it = ui->customPlot->selectedGraphs().first()->data()->constEnd();
		QVariant details;
		if (ui->customPlot->selectedGraphs().first()->selectTest(event->pos(), false, &details)) {
			QCPDataSelection dataPoints = details.value<QCPDataSelection>();

			if (dataPoints.dataPointCount() < 1) {
				// abort if event position is invalid
				return;
			}
			it = ui->customPlot->selectedGraphs().first()->data()->at(dataPoints.dataRange().begin());

		}

		graphTracer->setGraph(ui->customPlot->selectedGraphs().first());
		graphTracer->setGraphKey(it->key);
		graphTracer->setInterpolating(true);
		graphTracer->setStyle(QCPItemTracer::tsCircle);
		graphTracer->setPen(QPen(Qt::red));
		graphTracer->setBrush(Qt::red);
		graphTracer->setSize(7);
		graphTracer->setVisible(true);
		graphTracer->setSelectable(false);


		textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
		textLabel->position->setType(QCPItemPosition::ptPlotCoords);
		textLabel->position->setCoords(it->key + (ui->customPlot->xAxis->range().upper - ui->customPlot->xAxis->range().lower) * 0.01,
		                               it->value + (ui->customPlot->yAxis->range().upper - ui->customPlot->yAxis->range().lower) * 0.01);
		QFont fufa = textLabel->font();
		fufa.setPointSize(12);
		fufa.setBold(true);
		textLabel->setFont(fufa);
		textLabel->setSelectable(false);
		textLabel->setBrush(QBrush(QColor(Qt::white)));
		textLabel->setAntialiased(true);
		textLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
		textLabel->setText(QString("(%1, %2)").arg(QString::number(it->key, 'f', 3)).arg(QString::number(it->value, 'f', 3)));
		textLabel->setVisible(true);

		ui->customPlot->layer("cursorLayer")->replot();
//		qDebug() << it->key << it->value;


//		ui->customPlot->cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
//		ui->customPlot->cursor.cursorText->position->setCoords(QPointF(x, y));
//		QPointF pp = ui->customPlot->cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
//		ui->customPlot->cursor.cursorText->position->setPixelPosition(pp);
//		ui->customPlot->replot();
//		double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
//		QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
//		ui->statusBar->showMessage(message, 2500);

	} else { // no graph / multiple graphs selected
		textLabel->setVisible(false);
		graphTracer->setVisible(false);
		ui->customPlot->layer("cursorLayer")->replot();
	}

//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));
//	networkManager->get(QNetworkRequest(QUrl("http://www.evileg.ru/it_example.json")));



//	QNetworkRequest request(QUrl("https://httpbin.org/get"));
//	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
//	connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(replyFini(QNetworkReply * )));
//	connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
//	manager->get(request);
}

void MainWindow::replyFini(QNetworkReply *reply) {
	QString answer = QString::fromUtf8(reply->readAll());
	qDebug() << "answer------------>" << answer;
	qDebug() << QSslSocket::supportsSsl();
	reply->deleteLater();
}


void MainWindow::initGraph() {
	// enable openGL
	ui->customPlot->setOpenGl(true, 16); // enable openGL
	statusBarMsg(ui->customPlot->openGl() ? "openGL enabled" : "openGL disabled", 5000);

	if (USING_LAYER) {
		ui->customPlot->addLayer("cursorLayer", nullptr, QCustomPlot::limAbove);
		ui->customPlot->cursorLayer = ui->customPlot->layer("cursorLayer");
		//cursorLayer = new QCPLayer(this, "cursorLayer");
		ui->customPlot->cursorLayer->setMode(QCPLayer::lmBuffered);
	}

	textLabel = new QCPItemText(ui->customPlot);
	textLabel->setLayer(ui->customPlot->cursorLayer);
	graphTracer = new QCPItemTracer(ui->customPlot);
	graphTracer->setLayer(ui->customPlot->cursorLayer);

	//Cursor:
	QPen qpen = QPen(Qt::DashDotLine);
	ui->customPlot->cursor.hLine = new QCPItemLine(ui->customPlot);
	ui->customPlot->cursor.hLine->setPen(qpen);

	ui->customPlot->cursor.vLine = new QCPItemLine(ui->customPlot);
	ui->customPlot->cursor.vLine->setPen(qpen);

	ui->customPlot->cursor.cursorText = new QCPItemText(ui->customPlot);
	ui->customPlot->cursor.cursorText->setFont(QFont(font().family(), 8));

	//Add to layer:
	if (USING_LAYER) {
		ui->customPlot->cursor.hLine->setLayer("cursorLayer");  //"cursorLayer"
		ui->customPlot->cursor.vLine->setLayer("cursorLayer");
		ui->customPlot->cursor.cursorText->setLayer("cursorLayer");
	}


	// axes configuration
	// todo: implement ui->customPlot->xAxis->setPadding(value);

	// set axes ranges
	ui->customPlot->xAxis->setRange(-10, 10);
	ui->customPlot->yAxis->setRange(-10, 10);
	// configure right and top axis to show ticks but no labels:
	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)


	// legend initialization
	ui->customPlot->legend->setVisible(false);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui->customPlot->legend->setFont(legendFont);
	ui->customPlot->legend->setSelectedFont(legendFont);
	ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


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

	// graph clicked statusbar message
	connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable * , int, QMouseEvent * )), this,
	        SLOT(plotGraphClicked(QCPAbstractPlottable * , int)));

	// * context menu
	// setup policy and connect slot for context menu popup:
	ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(plotContextMenuRequest(QPoint)));


//	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//	dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
//	ui->customPlot->xAxis->setTicker(dateTicker);
//	double now = QDateTime::currentDateTime().toTime_t();
//	ui->customPlot->xAxis->setRange(now - now / 10, now);
//	ui->customPlot->yAxis->setRange(0, 10000);

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
	text.remove(" ");
	calculateAndDrawFunction(text);
	ui->customPlot->replot();

	// * add item to widget and set the appropriate icon color
	QColor color = getGraphColor(mFunctionGraph->size() - 1);

	auto pixmap = QPixmap(16, 16);
	pixmap.fill(color);

	auto *item = new QListWidgetItem();
	item->setText(text);
	item->setIcon(QIcon(pixmap));
	ui->QListWidget_functionList->addItem(item);
}

QVector<double> MainWindow::generateXArray(double lowerLim, double upperLim, unsigned int length) {
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
		ui->customPlot->removeGraph((QCPGraph *) (mFunctionGraph->constBegin() + selectedIndex.row()).key());
		mFunctionGraph->remove((mFunctionGraph->constBegin() + selectedIndex.row()).key()); // remove from list
		ui->customPlot->replot();
	} else {
		statusBarMsg("Please select a function");
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

//	if (ui->customPlot->selectedGraphs().size() == 1) {
//		auto graph = ui->customPlot->selectedGraphs().first();
//		QCPItemTracer *tracer = new QCPItemTracer(ui->customPlot);
//		double key = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
//		tracer->setGraph(graph);
//		tracer->setGraphKey(key);
//
//		qDebug() << tracer->position->value();
//
//		ui->customPlot->replot();

//		auto plottable = ui->customPlot->selectedGraphs().first();
//		int dataIndex = ui->customPlot->selectedGraphs().;
////		int dataIndex = ui->customPlot->mMouseSignalLayerableDetails.value<QCPDataSelection>().dataRange().begin();
//		double x = ui->customPlot->xAxis->pixelToCoord(event->pos().x());
//		double y = ui->customPlot->yAxis->pixelToCoord(event->pos().y());
//		ui->customPlot->cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
//		ui->customPlot->cursor.cursorText->position->setCoords(QPointF(x, y));
//		QPointF pp = ui->customPlot->cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
//		ui->customPlot->cursor.cursorText->position->setPixelPosition(pp);
//		ui->customPlot->replot();
//		double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
//		QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(
//				dataIndex).arg(dataValue);
//		ui->statusBar->showMessage(message, 2500);

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


inline void MainWindow::setUpAxesPageConnections() {
	// * set axis visible
	connect(plotWindow->ui->groupBox_xAxis_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setVisible(checked);
		ui->customPlot->replot();
	});

	// * set label text
	connect(plotWindow->ui->lineEdit_xAxis_setLabelText, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->xAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis_setLabelText, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->yAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_xAxis2_setLabelText, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->xAxis2->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis2_setLabelText, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->yAxis2->setLabel(text);
		ui->customPlot->replot();
	});

	// * set label color
	connect(plotWindow->ui->pushButton_xAxis_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis2->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis2->setLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});

	// * set label font
	connect(plotWindow->ui->pushButton_xAxis_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis2->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis2->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});

	// * set label padding
	connect(plotWindow->ui->spinBox_xAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});

	// ! TICK LABEL //
	// * set ticks visible
	connect(plotWindow->ui->groupBox_xAxis_setTickLabelVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setTickLabelVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});

	// * set tick label color
	connect(plotWindow->ui->pushButton_xAxis_setTickLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});

	// * set tick label font
	connect(plotWindow->ui->pushButton_xAxis_setTickLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});

	// * set tick label rotation
	connect(plotWindow->ui->doubleSpinBox_xAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->xAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->yAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_xAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->xAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->yAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});

	// * set tick label padding
	connect(plotWindow->ui->spinBox_xAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setTickLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setTickLabelPadding(value);
		ui->customPlot->replot();
	});

	// * set tick label position
	connect(plotWindow->ui->comboBox_xAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [=]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_xAxis_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_xAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->xAxis->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_yAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [=]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_yAxis_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_yAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->yAxis->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_xAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [=]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_xAxis2_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_xAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->xAxis2->setTickLabelSide(x);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->comboBox_yAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [=]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(plotWindow->ui->comboBox_yAxis2_setTickLabelPosition->itemData(
				plotWindow->ui->comboBox_yAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		ui->customPlot->yAxis2->setTickLabelSide(x);
		ui->customPlot->replot();
	});


	// ! TICK //
	// * set tick visible
	connect(plotWindow->ui->groupBox_xAxis_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setTicks(checked);
		ui->customPlot->replot();
	});


	// * set tick pen
	connect(plotWindow->ui->pushButton_xAxis_setTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis2->setTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set tick length in
	connect(plotWindow->ui->spinBox_xAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setTickLengthIn(value);
		ui->customPlot->replot();
	});

	// * set tick length out
	connect(plotWindow->ui->spinBox_xAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setTickLengthOut(value);
		ui->customPlot->replot();
	});

	// * set sub-ticks visible
	connect(plotWindow->ui->groupBox_xAxis_setSubTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setSubTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setSubTicks(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setSubTicks(checked);
		ui->customPlot->replot();
	});

	// * set sub-tick pen
	connect(plotWindow->ui->pushButton_xAxis_setSubTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setSubTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setSubTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setSubTickPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis2->setSubTickPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set sub-tick length in
	connect(plotWindow->ui->spinBox_xAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setSubTickLengthIn(value);
		ui->customPlot->replot();
	});

	// * set sub-tick length out
	connect(plotWindow->ui->spinBox_xAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setSubTickLengthOut(value);
		ui->customPlot->replot();
	});

	// * set grid visible
	connect(plotWindow->ui->groupBox_xAxis_setGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->grid()->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->grid()->setVisible(checked);
		ui->customPlot->replot();
	});

	// * set grid pen
	connect(plotWindow->ui->pushButton_xAxis_setGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis2->grid()->setPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set zero line pen
	connect(plotWindow->ui->pushButton_xAxis_setZeroLinePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setZeroLinePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setZeroLinePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setZeroLinePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	// todo: check antialiasing boxes at init?
	// * set grid antialiasing
	connect(plotWindow->ui->checkBox_xAxis_setGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiased(checked);
		ui->customPlot->replot();
	});

	// * set zero line antialisaing
	connect(plotWindow->ui->checkBox_xAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiasedZeroLine(checked);
		ui->customPlot->replot();
	});

	// * set sub-grid visible
	connect(plotWindow->ui->groupBox_xAxis_setSubGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setSubGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setSubGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setSubGridVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->grid()->setSubGridVisible(checked);
		ui->customPlot->replot();
	});

	// * set sub-grid pen
	connect(plotWindow->ui->pushButton_xAxis_setSubGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setSubGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setSubGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setSubGridPen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});

	// * set sub-grid antialiasing
	connect(plotWindow->ui->checkBox_xAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->grid()->setAntialiasedSubGrid(checked);
		ui->customPlot->replot();
	});

	// * select axis ticker type (pi, date, etc...)
	connect(plotWindow->ui->comboBox_xAxis_setTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis, value);
		plotWindow->ui->checkBox_xAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_xAxis2_setTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis2, value);
		plotWindow->ui->checkBox_xAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis_setTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis, value);
		plotWindow->ui->checkBox_yAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis2_setTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis2, value);
		plotWindow->ui->checkBox_yAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});

	// * set logarithmic scale
	connect(plotWindow->ui->checkBox_xAxis_setLogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_setLogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});

	// * set base pen
	connect(plotWindow->ui->pushButton_xAxis_setBasePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setBasePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->yAxis->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setBasePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis2->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setBasePen, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->yAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
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


inline void MainWindow::setUpTitlePageConnections() {
	// * add title button clicked
	connect(plotWindow->ui->pushButton_addTitle, &QPushButton::clicked, this, [=]() {
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
	connect(plotWindow->ui->listWidget_titleList, &QListWidget::itemChanged, this, [=](QListWidgetItem *changedItem) {
		graphTextElements->value(changedItem)->setText(changedItem->text());
		ui->customPlot->replot();
	});


	// * delete title
	connect(plotWindow->ui->pushButton_removeTitle, &QPushButton::clicked, this, [=]() {
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
	connect(plotWindow->ui->listWidget_titleList, &QListWidget::itemClicked, this, [=]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			QCPTextElement *titleElement = graphTextElements->value(selectedItem);
			// update QFontWidget
			plotWindow->titleFontDialog->setCurrentFont(titleElement->font());
			// update QColorWidget
			plotWindow->titleColorDialog->setCurrentColor(titleElement->textColor());
		}
	});
	//connect(plot, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));
	//connect(ui->customPlot, &QCustomPlot::beforeReplot, this, [=]() {
	//	int pxx = ui->customPlot->yAxis->coordToPixel(0);
	//	int pxy = ui->customPlot->xAxis->coordToPixel(0);
	//	ui->customPlot->xAxis->setOffset(-ui->customPlot->axisRect()->height() - ui->customPlot->axisRect()->top() + pxx);
	//	ui->customPlot->yAxis->setOffset(ui->customPlot->axisRect()->left() - pxy);
	//});



	// * when user changed font in QFontDialog - any font option
	connect(plotWindow->titleFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setFont(plotWindow->titleFontDialog->currentFont());
			ui->customPlot->replot();
		}
	});

	// * when user changed the color in QColorDialog
	connect(plotWindow->titleColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setTextColor(plotWindow->titleColorDialog->currentColor());
			//graphTextElements->value(plotWindow->ui->listWidget_titleList->item(plotWindow->ui->listWidget_titleList->count() - 1))->setTextColor(QColorDialog::getColor());
			ui->customPlot->replot();
		}
	});


	// * Title order changed
	connect(plotWindow->ui->listWidget_titleList->model(), &QAbstractItemModel::rowsMoved, this, [=]() {
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
	connect(plotWindow->ui->checkBox_MultiSelect, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iMultiSelect);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iMultiSelect);
		}
	});

	connect(plotWindow->ui->checkBox_RangeDrag, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iRangeDrag);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iRangeDrag);
		}
	});

	connect(plotWindow->ui->checkBox_RangeZoom, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iRangeZoom);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iRangeZoom);
		}
	});

	connect(plotWindow->ui->checkBox_SelectPlottables, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectPlottables);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectPlottables);
		}
	});

	connect(plotWindow->ui->checkBox_SelectAxes, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectAxes);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectAxes);
		}
	});

	connect(plotWindow->ui->checkBox_SelectItems, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectItems);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectItems);
		}
	});

	connect(plotWindow->ui->checkBox_SelectLegend, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectLegend);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectLegend);
		}
	});

	connect(plotWindow->ui->checkBox_SelectOther, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->setInteractions(ui->customPlot->interactions() | QCP::iSelectOther);
		} else {
			ui->customPlot->setInteractions(ui->customPlot->interactions() & ~QCP::iSelectOther);
		}
	});

	// * set background color
	connect(plotWindow->ui->pushButton_setBackgroundColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->background().toImage().pixelColor(0, 0)); // fixme: doesn't work
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->setBackground(QBrush(popUpColorDialog->currentColor()));
			ui->customPlot->replot();
		});
	});

	// * set cursor
	connect(plotWindow->ui->checkBox_enableCursor, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			connect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			ui->customPlot->cursor.cursorText->setVisible(true);
		} else {
			disconnect(ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			ui->customPlot->cursor.hLine->start->setCoords(0, 0);
			ui->customPlot->cursor.hLine->end->setCoords(0, 0);

			ui->customPlot->cursor.vLine->start->setCoords(0, 0);
			ui->customPlot->cursor.vLine->end->setCoords(0, 0);

			ui->customPlot->cursor.cursorText->setVisible(false);
			ui->customPlot->layer("cursorLayer")->replot();
		}
	});

	// * set stick axis to zero line
	connect(plotWindow->ui->checkBox_stickAxisToZeroLines, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			connect(ui->customPlot, SIGNAL(beforeReplot()), this, SLOT(stickAxisToZeroLines()));
			ui->customPlot->replot();
		} else {
			disconnect(ui->customPlot, SIGNAL(beforeReplot()), this, SLOT(stickAxisToZeroLines()));
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