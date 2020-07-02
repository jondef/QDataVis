//
// Created by John on 29.09.2019.
//

#include "MainWindow.hpp"
#include "PlotPropertiesWindow.hpp"
#include "QCustomPlot_custom.hpp"

PlotPropertiesWindow::PlotPropertiesWindow(QWidget *parent) : QWidget(parent), ui(new Ui::uiPlotPropertiesWindow) {
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &PlotPropertiesWindow::okButtonPressed);

	setWindowFlag(Qt::Window);

	ui->comboBox_xAxis_setTickLabelPosition->addItem(tr("Outside"), static_cast<int>(QCPAxis::lsOutside));
	ui->comboBox_xAxis_setTickLabelPosition->addItem(tr("Inside"), static_cast<int>(QCPAxis::lsInside));
	ui->comboBox_yAxis_setTickLabelPosition->addItem(tr("Outside"), static_cast<int>(QCPAxis::lsOutside));
	ui->comboBox_yAxis_setTickLabelPosition->addItem(tr("Inside"), static_cast<int>(QCPAxis::lsInside));
	ui->comboBox_xAxis2_setTickLabelPosition->addItem(tr("Outside"), static_cast<int>(QCPAxis::lsOutside));
	ui->comboBox_xAxis2_setTickLabelPosition->addItem(tr("Inside"), static_cast<int>(QCPAxis::lsInside));
	ui->comboBox_yAxis2_setTickLabelPosition->addItem(tr("Outside"), static_cast<int>(QCPAxis::lsOutside));
	ui->comboBox_yAxis2_setTickLabelPosition->addItem(tr("Inside"), static_cast<int>(QCPAxis::lsInside));

	titleFontDialog->setOption(QFontDialog::NoButtons);
	titleFontDialog->setOption(QFontDialog::ProportionalFonts);
	titleFontDialog->setOption(QFontDialog::MonospacedFonts);
	titleFontDialog->setOption(QFontDialog::NonScalableFonts);
	titleFontDialog->setOption(QFontDialog::ScalableFonts);
	titleFontDialog->setWindowFlags(Qt::Widget);
	titleFontDialog->setSizeGripEnabled(false);
	QGridLayout *gridLayoutFont = new QGridLayout(ui->widget_titleFont);
	gridLayoutFont->addWidget(titleFontDialog);

	titleColorDialog->setOption(QColorDialog::NoButtons);
	titleColorDialog->setOption(QColorDialog::ShowAlphaChannel);
	titleColorDialog->setWindowFlags(Qt::Widget);
	titleColorDialog->setSizeGripEnabled(false);
	QGridLayout *gridLayoutColor = new QGridLayout(ui->widget_titleColor);
	gridLayoutColor->addWidget(titleColorDialog);
}

PlotPropertiesWindow::~PlotPropertiesWindow() {
	delete ui;
}

void PlotPropertiesWindow::showEvent(QShowEvent *event) {
	QWidget::showEvent(event);
	setUpAxesPageConnections();
	setUpGeneralPageConnections();
	setUpTitlePageConnections();
}

void PlotPropertiesWindow::okButtonPressed() {
	emit windowClosed();
	close();
}

void PlotPropertiesWindow::changeAxisTicker(QCPAxis *axis, const QString &value) {
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
	dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
}


inline void PlotPropertiesWindow::setUpAxesPageConnections() {
	// * set axis visible
	connect(ui->groupBox_xAxis_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set label text
	connect(ui->lineEdit_xAxis_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setLabel(text);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->lineEdit_yAxis_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setLabel(text);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->lineEdit_xAxis2_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setLabel(text);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->lineEdit_yAxis2_setLabelText, &QLineEdit::textChanged, this, [this](const QString &text) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setLabel(text);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set label color
	connect(ui->pushButton_xAxis_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->labelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set label font
	connect(ui->pushButton_xAxis_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->labelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set label padding
	connect(ui->spinBox_xAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// ! TICK LABEL //
	// * set ticks visible
	connect(ui->groupBox_xAxis_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabels(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabels(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabels(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setTickLabelVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabels(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set tick label color
	connect(ui->pushButton_xAxis_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setTickLabelColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->tickLabelColor());
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabelColor(popUpColorDialog->currentColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set tick label font
	connect(ui->pushButton_xAxis_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setTickLabelFont, &QPushButton::clicked, this, [this]() {
		disconnect(popUpFontDialog, &QFontDialog::currentFontChanged, nullptr, nullptr);
		popUpFontDialog->setCurrentFont(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->tickLabelFont());
		popUpFontDialog->open();
		connect(popUpFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabelFont(popUpFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set tick label rotation
	connect(ui->doubleSpinBox_xAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabelRotation(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->doubleSpinBox_yAxis_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabelRotation(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->doubleSpinBox_xAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabelRotation(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->doubleSpinBox_yAxis2_setTickLabelRotation, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabelRotation(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set tick label padding
	connect(ui->spinBox_xAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setTickLabelPadding, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabelPadding(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set tick label position
	connect(ui->comboBox_xAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(ui->comboBox_xAxis_setTickLabelPosition->itemData(
				ui->comboBox_xAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLabelSide(x);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->comboBox_yAxis_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(ui->comboBox_yAxis_setTickLabelPosition->itemData(
				ui->comboBox_yAxis_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLabelSide(x);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->comboBox_xAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(ui->comboBox_xAxis2_setTickLabelPosition->itemData(
				ui->comboBox_xAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLabelSide(x);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->comboBox_yAxis2_setTickLabelPosition, &QComboBox::currentTextChanged, this, [this]() {
		QCPAxis::LabelSide x = static_cast<QCPAxis::LabelSide>(ui->comboBox_yAxis2_setTickLabelPosition->itemData(
				ui->comboBox_yAxis2_setTickLabelPosition->currentIndex(), Qt::UserRole).toInt());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLabelSide(x);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});


	// ! TICK //
	// * set tick visible
	connect(ui->groupBox_xAxis_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});


	// * set tick pen
	connect(ui->pushButton_xAxis_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->tickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set tick length in
	connect(ui->spinBox_xAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set tick length out
	connect(ui->spinBox_xAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set sub-ticks visible
	connect(ui->groupBox_xAxis_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setSubTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setSubTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setSubTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setSubTicksVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setSubTicks(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set sub-tick pen
	connect(ui->pushButton_xAxis_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setSubTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setSubTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setSubTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setSubTickPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->subTickPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setSubTickPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set sub-tick length in
	connect(ui->spinBox_xAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setSubTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setSubTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setSubTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setSubTickLengthIn, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setSubTickLengthIn(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set sub-tick length out
	connect(ui->spinBox_xAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setSubTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setSubTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_xAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setSubTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->spinBox_yAxis2_setSubTickLengthOut, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setSubTickLengthOut(value);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set grid visible
	connect(ui->groupBox_xAxis_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set grid pen
	connect(ui->pushButton_xAxis_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->pen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set zero line pen
	connect(ui->pushButton_xAxis_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setZeroLinePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->zeroLinePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setZeroLinePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	// todo: check antialiasing boxes at init?
	// * set grid antialiasing
	connect(ui->checkBox_xAxis_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setAntialiased(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setAntialiased(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_xAxis2_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setAntialiased(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis2_setGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setAntialiased(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set zero line antialisaing
	connect(ui->checkBox_xAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setAntialiasedZeroLine(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setAntialiasedZeroLine(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_xAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setAntialiasedZeroLine(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis2_setZeroLineAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setAntialiasedZeroLine(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set sub-grid visible
	connect(ui->groupBox_xAxis_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setSubGridVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setSubGridVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_xAxis2_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setSubGridVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->groupBox_yAxis2_setSubGridVisible, &QGroupBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setSubGridVisible(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set sub-grid pen
	connect(ui->pushButton_xAxis_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->grid()->setSubGridPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setSubGridPen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->subGridPen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setSubGridPen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set sub-grid antialiasing
	connect(ui->checkBox_xAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->grid()->setAntialiasedSubGrid(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_xAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->grid()->setAntialiasedSubGrid(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis2_setSubGridAntialiased, &QCheckBox::toggled, this, [this](bool checked) {
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->grid()->setAntialiasedSubGrid(checked);
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * select axis ticker type (pi, date, etc...)
	connect(ui->comboBox_xAxis_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis, value);
		ui->checkBox_xAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(ui->comboBox_xAxis2_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2, value);
		ui->checkBox_xAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(ui->comboBox_yAxis_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis, value);
		ui->checkBox_yAxis_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});
	connect(ui->comboBox_yAxis2_setTicker, &QComboBox::currentTextChanged, this, [this](const QString &value) {
		changeAxisTicker(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2, value);
		ui->checkBox_yAxis2_setLogarithmicScale->setChecked(value == "QCPAxisTickerLog");
	});


	// * set logarithmic scale
	connect(ui->checkBox_xAxis_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setScaleType(QCPAxis::stLinear);
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_xAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setScaleType(QCPAxis::stLinear);
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setScaleType(QCPAxis::stLinear);
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
	connect(ui->checkBox_yAxis2_setLogarithmicScale, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setScaleType(QCPAxis::stLogarithmic);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setScaleType(QCPAxis::stLinear);
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * set base pen
	connect(ui->pushButton_xAxis_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setBasePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setBasePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_xAxis2_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis2->setBasePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});
	connect(ui->pushButton_yAxis2_setBasePen, &QPushButton::clicked, this, [this]() {
		disconnect(popUpPenDialog, &QPenDialog::currentPenChanged, nullptr, nullptr);
		popUpPenDialog->setCurrentPen(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->basePen());
		popUpPenDialog->open();
		connect(popUpPenDialog, &QPenDialog::currentPenChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis2->setBasePen(popUpPenDialog->currentPen());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	ui->groupBox_xAxis_setGridVisible->setChecked(true);
	ui->groupBox_yAxis_setGridVisible->setChecked(true);

	ui->groupBox_xAxis_setVisible->setChecked(true);
	ui->groupBox_yAxis_setVisible->setChecked(true);

	ui->groupBox_xAxis_setTickLabelVisible->setChecked(true);
	ui->groupBox_yAxis_setTickLabelVisible->setChecked(true);
	ui->groupBox_xAxis2_setTickLabelVisible->setChecked(true);
	ui->groupBox_yAxis2_setTickLabelVisible->setChecked(true);

	ui->groupBox_xAxis_setTicksVisible->setChecked(true);
	ui->groupBox_yAxis_setTicksVisible->setChecked(true);
	ui->groupBox_xAxis2_setTicksVisible->setChecked(true);
	ui->groupBox_yAxis2_setTicksVisible->setChecked(true);

	ui->groupBox_xAxis_setSubTicksVisible->setChecked(true);
	ui->groupBox_yAxis_setSubTicksVisible->setChecked(true);
	ui->groupBox_xAxis2_setSubTicksVisible->setChecked(true);
	ui->groupBox_yAxis2_setSubTicksVisible->setChecked(true);
}


inline void PlotPropertiesWindow::setUpTitlePageConnections() {
	// * add title button clicked
	connect(ui->pushButton_addTitle, &QPushButton::clicked, this, [this]() {
		ui->listWidget_titleList->addItem("Title");

		QListWidgetItem *listWidgetItem = ui->listWidget_titleList->item(ui->listWidget_titleList->count() - 1);
		QCPTextElement *textElement = new QCPTextElement(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, "Title",
														 titleFontDialog->currentFont());

		graphTextElements->insert(listWidgetItem, textElement);

		// make each item editable by double clicking
		listWidgetItem->setFlags(listWidgetItem->flags() | Qt::ItemIsEditable);

		// add title to the plot
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->insertRow(graphTextElements->count());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->addElement(graphTextElements->count(), 0, textElement);

		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * when user renamed listWidgetItem
	connect(ui->listWidget_titleList, &QListWidget::itemChanged, this, [this](QListWidgetItem *changedItem) {
		graphTextElements->value(changedItem)->setText(changedItem->text());
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});

	// * delete title
	connect(ui->pushButton_removeTitle, &QPushButton::clicked, this, [this]() {
		QListWidgetItem *selectedItem = ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->remove(graphTextElements->value(selectedItem));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->simplify();

			graphTextElements->remove(selectedItem);
			delete selectedItem;

			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		}
	});

	// * update the widgets on the right when a title is clicked
	connect(ui->listWidget_titleList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
		QCPTextElement *titleElement = graphTextElements->value(item);
		// update QFontWidget
		titleFontDialog->setCurrentFont(titleElement->font());
		// update QColorWidget
		titleColorDialog->setCurrentColor(titleElement->textColor());
	});

	// * when user changed font in QFontDialog - any font option
	connect(titleFontDialog, &QFontDialog::currentFontChanged, this, [this]() {
		QListWidgetItem *selectedItem = ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setFont(titleFontDialog->currentFont());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		}
	});

	// * when user changed the color in QColorDialog
	connect(titleColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
		QListWidgetItem *selectedItem = ui->listWidget_titleList->currentItem();
		if (selectedItem != nullptr) {
			graphTextElements->value(selectedItem)->setTextColor(titleColorDialog->currentColor());
			//graphTextElements->value(ui->listWidget_titleList->item(ui->listWidget_titleList->count() - 1))->setTextColor(QColorDialog::getColor());
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		}
	});

	// * Title order changed
	connect(ui->listWidget_titleList->model(), &QAbstractItemModel::rowsMoved, this, [this]() {
		// remove all of the titles except the plot
		for (int j = 0; j < graphTextElements->count() + 1; ++j) {
			if (ui->listWidget_titleList->item(j)->text() == "Plot") {
				continue;
			}
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->take(
					graphTextElements->value(ui->listWidget_titleList->item(j)));
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->simplify();

		int index = 0;

		// ! count() + 1 because we need to account for the plot item.
		for (int i = 0; i < graphTextElements->count() + 1; ++i) {
			// check if current item is the plot item
			if (ui->listWidget_titleList->item(i)->text() == "Plot") {
				index++;
				continue;
			}
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->insertRow(index);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->plotLayout()->addElement(
					index, 0, graphTextElements->value(ui->listWidget_titleList->item(i)));
			index++;
		}
		dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
	});
}


inline void PlotPropertiesWindow::setUpGeneralPageConnections() {
	connect(ui->checkBox_MultiSelect, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iMultiSelect);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iMultiSelect);
		}
	});

	connect(ui->checkBox_RangeDrag, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iRangeDrag);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iRangeDrag);
		}
	});

	connect(ui->checkBox_RangeZoom, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iRangeZoom);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iRangeZoom);
		}
	});

	connect(ui->checkBox_SelectPlottables, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iSelectPlottables);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iSelectPlottables);
		}
	});

	connect(ui->checkBox_SelectAxes, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iSelectAxes);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iSelectAxes);
		}
	});

	connect(ui->checkBox_SelectItems, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iSelectItems);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iSelectItems);
		}
	});

	connect(ui->checkBox_SelectLegend, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iSelectLegend);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iSelectLegend);
		}
	});

	connect(ui->checkBox_SelectOther, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() | QCP::iSelectOther);
		} else {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setInteractions(
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->interactions() & ~QCP::iSelectOther);
		}
	});

	// * set background color
	connect(ui->pushButton_setBackgroundColor, &QPushButton::clicked, this, [this]() {
		disconnect(popUpColorDialog, &QColorDialog::currentColorChanged, nullptr, nullptr);
		popUpColorDialog->setCurrentColor(
				dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->background().toImage().pixelColor(0, 0)); // fixme: doesn't work
		popUpColorDialog->open();
		connect(popUpColorDialog, &QColorDialog::currentColorChanged, this, [this]() {
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->setBackground(QBrush(popUpColorDialog->currentColor()));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		});
	});

	// * set cursor
	connect(ui->checkBox_enableCursor, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			connect(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )),
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.cursorText->setVisible(true);
		} else {
			disconnect(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SIGNAL(mouseMove(QMouseEvent * )),
					   dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SLOT(onMouseMoveReplotCursor(QMouseEvent * )));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.hLine->start->setCoords(0, 0);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.hLine->end->setCoords(0, 0);

			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.vLine->start->setCoords(0, 0);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.vLine->end->setCoords(0, 0);

			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->cursor.cursorText->setVisible(false);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->layer("cursorLayer")->replot();
		}
	});

	// * set stick axis to zero line
	connect(ui->checkBox_stickAxisToZeroLines, &QCheckBox::toggled, this, [this](bool checked) {
		if (checked) {
			connect(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SIGNAL(beforeReplot()),
					dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SLOT(stickAxisToZeroLines()));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		} else {
			disconnect(dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SIGNAL(beforeReplot()),
					   dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot, SLOT(stickAxisToZeroLines()));
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->axisRect()->setAutoMargins(QCP::msAll);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->xAxis->setOffset(0);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->yAxis->setOffset(0);
			dynamic_cast<MainWindow *>(parentWidget())->ui->customPlot->replot();
		}
	});

	emit(ui->checkBox_stickAxisToZeroLines->toggled(false));
	emit(ui->checkBox_enableCursor->toggled(false));
	ui->checkBox_enableCursor->setChecked(false);
	ui->checkBox_RangeDrag->setChecked(true);
	ui->checkBox_RangeZoom->setChecked(true);
	ui->checkBox_MultiSelect->setChecked(true);
	ui->checkBox_SelectPlottables->setChecked(true);
	ui->checkBox_SelectAxes->setChecked(true);
	ui->checkBox_SelectLegend->setChecked(true);
	ui->checkBox_SelectItems->setChecked(true);
	ui->checkBox_SelectOther->setChecked(true);
}
