//
// Created by John on 12/06/2019.
//

#include "MainWindow.h"
#include "Resources/icon.xpm" // import icon as static const array *


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	// save icon in exe // https://convertio.co/png-xpm/
	// comes from the xpm icon file
	setWindowIcon(QIcon(QPixmap(window_icon_xpm)));

	//auto pixmap = QPixmap(16, 16);
	//pixmap.fill(QColorDialog::getColor());
	//ui->customPlot->setBackground(pixmap);

	initGraph();

	connect(ui->QPushButton_PlotPoints, &QPushButton::clicked, this, &MainWindow::QPushButton_PlotPoints_clicked);
	connect(ui->QPushButton_deleteFunction, &QPushButton::clicked, this, &MainWindow::QPushButton_deleteFunction_clicked);
	connect(ui->QLineEdit_addFunction, &QLineEdit::returnPressed, this, &MainWindow::QLineEdit_addFunction_returnPressed);
	connect(ui->QLineEdit_functionParam, &QLineEdit::returnPressed, this, &MainWindow::QLineEdit_addFunction_returnPressed);
	connect(ui->actionQuit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
	connect(ui->actionSave_as, &QAction::triggered, this, &MainWindow::savePlotImage);
	connect(ui->QPushButton_FormattingHelp, &QPushButton::clicked, this, &MainWindow::Test);
	connect(ui->actionProperies, &QAction::triggered, this, [=]() {
		plotWindow->show();
		plotWindow->raise(); // bring it to front
		plotWindow->activateWindow(); // select it
		plotWindow->setWindowState(plotWindow->windowState() & ~Qt::WindowMinimized | Qt::WindowActive); // set to active
	});


	QPen graphPen;
	graphPen.setColor(QColor(qRgb(0, 0, 0)));
	graphPen.setWidth(1);

//	ui->customPlot->yAxis->grid()->setSubGridVisible(true);
//	ui->customPlot->yAxis->grid()->setZeroLinePen(graphPen);
//	ui->customPlot->xAxis->grid()->setZeroLinePen(graphPen);
//
//	graphPen.setColor(QColor(qRgb(0, 0, 0)));
//	ui->customPlot->yAxis->grid()->setPen(graphPen);
//	ui->customPlot->xAxis->grid()->setPen(graphPen);
//	graphPen.setColor(QColor(qRgb(0, 0, 0)));
//	ui->customPlot->xAxis->grid()->setSubGridPen(graphPen);
//	ui->customPlot->yAxis->grid()->setSubGridPen(graphPen);
	// todo: add more features like tickside and padding (label and tick)

	//ui->customPlot->xAxis->grid()->setVisible(false);
	//ui->customPlot->yAxis->grid()->setVisible(false);
	ui->customPlot->xAxis2->grid()->setSubGridVisible(true);
	ui->customPlot->xAxis2->grid()->setVisible(true);

	ui->customPlot->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
	ui->customPlot->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"


	setUpAxesPageConnections();
	setUpInteractionsPageConnections();
	setUpTitlePageConnections();
	setUpGeneralPageConnections();


	connect(plotWindow, &PlotPropertiesWindow::windowClosed, this, [=]() {
		qDebug() << "saved";
	});
//	dumpObjectInfo();
//	qDebug() << Q_FUNC_INFO << "Item too large for memory, setting invisible";

	// Connect networkManager response to the handler
	//connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::onResult);
	// We get the data, namely JSON file from a site on a particular url
	//networkManager->get(QNetworkRequest(QUrl("https://poloniex.com/public?command=returnChartData&currencyPair=USDT_BTC&start=1405699200&end=9999999999&period=14400")));
//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));
}


MainWindow::~MainWindow() {
	delete ui;
}


inline void MainWindow::setUpAxesPageConnections() {
	// * set axis visible
	connect(plotWindow->ui->groupBox_xAxis_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setVisible(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setVisible(checked);
		ui->customPlot->replot();
	});

	QPen pen;  // creates a default pen


	pen.setWidth(10);
	pen.setBrush(Qt::red);
//	pen.setStyle(Qt::DashDotLine);
//	pen.setCapStyle(Qt::RoundCap);
	pen.setJoinStyle(Qt::RoundJoin);
	ui->customPlot->xAxis->setBasePen(pen);
	ui->customPlot->xAxis->setTickPen(pen);
	ui->customPlot->xAxis->setSubTickPen(pen);

	// * set ticks visible
	connect(plotWindow->ui->groupBox_xAxis_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_xAxis2_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->xAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis->setTickLabels(checked);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->groupBox_yAxis2_setTicksVisible, &QGroupBox::toggled, this, [=](bool checked) {
		ui->customPlot->yAxis2->setTickLabels(checked);
		ui->customPlot->replot();
	});

	// * set tick base pen
	connect(plotWindow->ui->pushButton, &QPushButton::clicked, this, [=]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(ui->customPlot->xAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [=]() {
			ui->customPlot->xAxis->setBasePen(popUpPenDialog->currentPen());
			ui->customPlot->replot();
		});
	});


	// * set tick color
	connect(plotWindow->ui->pushButton_xAxis_setTickColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			ui->customPlot->replot();
		});
	});

	// * set tick rotation
	connect(plotWindow->ui->doubleSpinBox_xAxis_setTickRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->xAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_xAxis2_setTickRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->xAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis_setTickRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->yAxis->setTickLabelRotation(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->doubleSpinBox_yAxis2_setTickRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [=](double value) {
		ui->customPlot->yAxis2->setTickLabelRotation(value);
		ui->customPlot->replot();
	});

	// * set tick font
	connect(plotWindow->ui->pushButton_xAxis_setTickFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_xAxis2_setTickFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis_setTickFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});
	connect(plotWindow->ui->pushButton_yAxis2_setTickFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});


	// * select axis ticker type (pi, date, etc...)
	connect(plotWindow->ui->comboBox_xAxis_QCPAxisTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis, value);
		plotWindow->ui->checkBox_xAxis_LogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_xAxis2_QCPAxisTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->xAxis2, value);
		plotWindow->ui->checkBox_xAxis2_LogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis_QCPAxisTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis, value);
		plotWindow->ui->checkBox_yAxis_LogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(plotWindow->ui->comboBox_yAxis2_QCPAxisTicker, &QComboBox::currentTextChanged, this, [=](const QString &value) {
		changeAxisTicker(ui->customPlot->yAxis2, value);
		plotWindow->ui->checkBox_yAxis2_LogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});

	// * set logarithmic scale
	connect(plotWindow->ui->checkBox_xAxis_LogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_xAxis2_LogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->xAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis_LogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->checkBox_yAxis2_LogarithmicScale, &QCheckBox::toggled, this, [=](bool checked) {
		if (checked) {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			ui->customPlot->yAxis2->setScaleType(QCPAxis::stLinear);
		}
		ui->customPlot->replot();
	});

	// * show label
	connect(plotWindow->ui->lineEdit_xAxis_label, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->xAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_xAxis2_label, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->xAxis2->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis_label, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->yAxis->setLabel(text);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->lineEdit_yAxis2_label, &QLineEdit::textChanged, this, [=](const QString &text) {
		ui->customPlot->yAxis2->setLabel(text);
		ui->customPlot->replot();
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
	connect(plotWindow->ui->pushButton_xAxis2_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->xAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->xAxis2->setLabelFont(popUpFontDialog->currentFont());
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
	connect(plotWindow->ui->pushButton_yAxis2_setLabelFont, &QPushButton::clicked, this, [=]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(ui->customPlot->yAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [=]() {
			ui->customPlot->yAxis2->setLabelFont(popUpFontDialog->currentFont());
			ui->customPlot->replot();
		});
	});

	// * change label color
	connect(plotWindow->ui->pushButton_xAxis_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->xAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->xAxis->setLabelColor(popUpColorDialog->currentColor());
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
	connect(plotWindow->ui->pushButton_yAxis_setLabelColor, &QPushButton::clicked, this, [=]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(ui->customPlot->yAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [=]() {
			ui->customPlot->yAxis->setLabelColor(popUpColorDialog->currentColor());
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

	// * set label padding
	connect(plotWindow->ui->spinBox_xAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_xAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->xAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis->setLabelPadding(value);
		ui->customPlot->replot();
	});
	connect(plotWindow->ui->spinBox_yAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [=](int value) {
		ui->customPlot->yAxis2->setLabelPadding(value);
		ui->customPlot->replot();
	});


	plotWindow->ui->groupBox_xAxis_setVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setVisible->setChecked(true);

	plotWindow->ui->groupBox_xAxis_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_xAxis2_setTicksVisible->setChecked(true);
	plotWindow->ui->groupBox_yAxis2_setTicksVisible->setChecked(true);
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
	connect(ui->customPlot, &QCustomPlot::beforeReplot, this, [=]() {
		int pxx = ui->customPlot->yAxis->coordToPixel(0);
		int pxy = ui->customPlot->xAxis->coordToPixel(0);
		ui->customPlot->xAxis->setOffset(-ui->customPlot->axisRect()->height() - ui->customPlot->axisRect()->top() + pxx);
		ui->customPlot->yAxis->setOffset(ui->customPlot->axisRect()->left() - pxy);
	});



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


inline void MainWindow::setUpInteractionsPageConnections() {
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

	plotWindow->ui->checkBox_RangeDrag->setChecked(true);
	plotWindow->ui->checkBox_RangeZoom->setChecked(true);
	plotWindow->ui->checkBox_MultiSelect->setChecked(true);
	plotWindow->ui->checkBox_SelectPlottables->setChecked(true);
	plotWindow->ui->checkBox_SelectAxes->setChecked(true);
	plotWindow->ui->checkBox_SelectLegend->setChecked(true);
	plotWindow->ui->checkBox_SelectItems->setChecked(true);
	plotWindow->ui->checkBox_SelectOther->setChecked(true);
}


inline void MainWindow::setUpGeneralPageConnections() {
	connect(plotWindow->generalTabColorDialog, &QColorDialog::currentColorChanged, this, [=](QColor color) {
		QListWidgetItem *selectedItem = plotWindow->ui->listWidget_generalTab->currentItem();
		if (selectedItem != nullptr) {
			if (selectedItem->text() == "Background") {
				QPixmap pixmap = QPixmap(16, 16);
				pixmap.fill(color);
				selectedItem->setIcon(QIcon(pixmap));
				ui->customPlot->setBackground(pixmap);
				ui->customPlot->replot();
			}
		}

		//graphTextElements->value(selectedItem)->setTextColor(plotWindow->titleColorDialog->currentColor());
		//graphTextElements->value(p
	});
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
		functionGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
		functionGraphList->last()->setData(date, high);
		graphPen.setColor(QColor(colors.at(2)));
		functionGraphList->last()->setPen(graphPen); // apply color to graph
		// * low
		functionGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
		functionGraphList->last()->setData(date, low);
		graphPen.setColor(QColor(colors.at(1)));
		functionGraphList->last()->setPen(graphPen); // apply color to graph
		// * avg
		functionGraphList->append(new QCPGraph(ui->customPlot->xAxis, ui->customPlot->yAxis));
		functionGraphList->last()->setData(date, avg);
		graphPen.setColor(QColor(colors.at(0)));
		functionGraphList->last()->setPen(graphPen); // apply color to graph

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

void MainWindow::Test() {
//	networkManager->get(QNetworkRequest(QUrl("https://hacker-news.firebaseio.com/v0/newstories.json")));
//	networkManager->get(QNetworkRequest(QUrl("http://www.evileg.ru/it_example.json")));

	QNetworkRequest request(QUrl("https://httpbin.org/get"));
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply * )), this, SLOT(replyFini(QNetworkReply * )));
	connect(manager, &QNetworkAccessManager::finished, manager, &QNetworkAccessManager::deleteLater);
	manager->get(request);
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


	// axes configuration

	// set axes ranges
	ui->customPlot->xAxis2->setRange(-10, 10);
	ui->customPlot->yAxis2->setRange(-10, 10);
	// configure right and top axis to show ticks but no labels:
	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)


	// legend initialization
	ui->customPlot->legend->setVisible(true);
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
		savedOk = ui->customPlot->savePdf(savePathFilename, 0, 0, QCP::epAllowCosmetic, QString(""),
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

	// ! GCPCurve has performance issues
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
