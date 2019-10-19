//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#include "QCustomPlot_custom.h"


QCustomPlot_custom::QCustomPlot_custom(QWidget *parent) {

}


void QCustomPlot_custom::manageCursor(double x, double y) {
	cursor.hLine->start->setCoords(-QCPRange::maxRange, y);
	cursor.hLine->end->setCoords(QCPRange::maxRange, y);

	cursor.vLine->start->setCoords(x, -QCPRange::maxRange);
	cursor.vLine->end->setCoords(x, QCPRange::maxRange);

	cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
	cursor.cursorText->position->setCoords(QPointF(x, y));
	QPointF pp = cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
	cursor.cursorText->position->setPixelPosition(pp);
}