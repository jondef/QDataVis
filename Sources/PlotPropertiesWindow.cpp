//
// Created by John on 29.09.2019.
//

#include <dependencies/qcustomplot/include/qcustomplot.h>
#include "PlotPropertiesWindow.h"


PlotPropertiesWindow::PlotPropertiesWindow(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
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

	titleColorDialog->setOption(QColorDialog::NoButtons);
	titleColorDialog->setOption(QColorDialog::ShowAlphaChannel);
	titleColorDialog->setWindowFlags(Qt::Widget);
	titleColorDialog->setSizeGripEnabled(false);

	QGridLayout *gridLayoutFont = new QGridLayout(ui->widget_titleFont);
	QGridLayout *gridLayoutColor = new QGridLayout(ui->widget_titleColor);
	gridLayoutFont->addWidget(titleFontDialog);
	gridLayoutColor->addWidget(titleColorDialog);
}

PlotPropertiesWindow::~PlotPropertiesWindow() {
	delete ui;
}

void PlotPropertiesWindow::okButtonPressed() {
	emit windowClosed();
	close();
}

