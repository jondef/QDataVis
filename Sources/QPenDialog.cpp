//
// Created by John on 14.10.2019.
//


#include "QPenDialog.h"

QPenDialog::QPenDialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog) {
	ui->setupUi(this);

}

void QPenDialog::setCurrentPen(const QPen &pen) {

}

QPen QPenDialog::currentPen() const {
	return QPen(QColor(qRgb(0, 0, 0)));
}

QPenDialog::~QPenDialog() {
	delete ui;
}
