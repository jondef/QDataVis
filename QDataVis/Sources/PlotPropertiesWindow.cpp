//
// Created by John on 29.09.2019.
//


#include "PlotPropertiesWindow.h"
//#include "MainWindow.h" // check https://stackoverflow.com/a/50205573/10450514 for more info


PlotPropertiesWindow::PlotPropertiesWindow(QWidget *parent) : QWidget(parent), ui(new Ui::uiPlotPropertiesWindow) {
	ui->setupUi(this);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okButtonPressed())); // clicked ok

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

void PlotPropertiesWindow::okButtonPressed() {
	emit windowClosed();
	close();
}
