//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#ifndef GUI_APP_QCUSTOMPLOT_CUSTOM_H
#define GUI_APP_QCUSTOMPLOT_CUSTOM_H

#include "dependencies/qcustomplot/include/qcustomplot.h"

#define USING_LAYER true

struct QCPCursor {
	QCPItemLine *hLine;
	QCPItemLine *vLine;
	QCPItemText *cursorText;
};

class QCustomPlot_custom : public QCustomPlot {
Q_OBJECT
public slots:

	void mouseMove(QMouseEvent *);

public:
	QCustomPlot_custom(QWidget *parent = NULL);

	~QCustomPlot_custom() {}

public:
	void init(QVector<double> xdata, QVector<double> ydata);

	void manageCursor(double x, double y);

	QCPLayer *cursorLayer;
	QCPCursor cursor;
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
