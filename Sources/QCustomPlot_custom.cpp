//
// Created by John on 19.10.2019.
//

#include "QCustomPlot_custom.h"


QCustomPlot_custom::QCustomPlot_custom(QWidget *parent) {
	connect(this, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(mouseMove(QMouseEvent * )));
}

void QCustomPlot_custom::init(QVector<double> xdata, QVector<double> ydata) {
	this->addGraph();
	this->graph(0)->setData(xdata, ydata);

	QColor colorPen(10, 25, 180, 255);
	QPen pen;
	pen.setWidth(50);
	pen.setColor(colorPen);
	this->graph()->setLineStyle(QCPGraph::lsLine);
	this->graph()->setPen(QPen(colorPen));
	this->xAxis->setLabel("X-axis");
	this->yAxis->setLabel("Y-axis");
	this->rescaleAxes();
	this->replot();

	if (USING_LAYER) {
		this->addLayer("cursorLayer", 0, QCustomPlot::limAbove);
		cursorLayer = this->layer("cursorLayer");
		//cursorLayer = new QCPLayer(this, "cursorLayer");
		cursorLayer->setMode(QCPLayer::lmBuffered);
	}

	//Cursor:
	QPen qpen = QPen(Qt::DashDotLine);
	cursor.hLine = new QCPItemLine(this);
	cursor.hLine->setPen(qpen);
	cursor.vLine = new QCPItemLine(this);
	cursor.vLine->setPen(qpen);
	cursor.cursorText = new QCPItemText(this);
	cursor.cursorText->setFont(QFont(font().family(), 8));

	//Add to layer:
	if (USING_LAYER) {
		cursor.hLine->setLayer("cursorLayer");  //"cursorLayer"
		cursor.vLine->setLayer("cursorLayer");
		cursor.cursorText->setLayer("cursorLayer");
	}

}

void QCustomPlot_custom::mouseMove(QMouseEvent *event) {
	//Cursor coordinates:
	double x = this->xAxis->pixelToCoord(event->pos().x());
	double y = this->yAxis->pixelToCoord(event->pos().y());
	qDebug() << "fucK";
	manageCursor(x, y);
	if (USING_LAYER)
		this->layer("cursorLayer")->replot();
	else
		this->replot();
}

void QCustomPlot_custom::manageCursor(double x, double y) {
	cursor.hLine->start->setCoords(-QCPRange::maxRange, y);
	cursor.hLine->end->setCoords(QCPRange::maxRange, y);

	cursor.vLine->start->setCoords(x, -QCPRange::maxRange);
	cursor.vLine->end->setCoords(x, QCPRange::maxRange);

	cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
	cursor.cursorText->position->setCoords(QPointF(x, y));
	QPointF pp = cursor.cursorText->position->pixelPosition() + QPointF(50.0, -15.0);
	cursor.cursorText->position->setPixelPosition(pp);
}