//
// Created by John on 29.09.2019.
//

#include "PlotPropertiesWindow.h"


PlotPropertiesWindow::PlotPropertiesWindow(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
	ui->setupUi(this);

	connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(okButtonPressed())); // clicked ok

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

	generalTabColorDialog->setOption(QColorDialog::NoButtons);
	generalTabColorDialog->setOption(QColorDialog::ShowAlphaChannel);
	generalTabColorDialog->setWindowFlags(Qt::Widget);
	generalTabColorDialog->setSizeGripEnabled(false);

	QGridLayout *gridLayoutFont = new QGridLayout(ui->widget_titleFont);
	QGridLayout *gridLayoutColor = new QGridLayout(ui->widget_titleColor);
	QGridLayout *gridLayoutGeneralTabColor = new QGridLayout(ui->widget_generalTab);
	gridLayoutFont->addWidget(titleFontDialog);
	gridLayoutColor->addWidget(titleColorDialog);
	gridLayoutGeneralTabColor->addWidget(generalTabColorDialog);
}

PlotPropertiesWindow::~PlotPropertiesWindow() {
	delete ui;
}

void PlotPropertiesWindow::okButtonPressed() {
	emit windowClosed();
	close();
}

