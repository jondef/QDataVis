//
// Created by John on 29.09.2019.
//


#include "PlotPropertiesWindow.h"

PlotPropertiesWindow::PlotPropertiesWindow(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
	ui->setupUi(this);

	connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(close()));

}

PlotPropertiesWindow::~PlotPropertiesWindow() {
	delete ui;
}


