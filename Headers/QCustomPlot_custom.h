//
// Created by John on 19.10.2019.
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
private slots:

	void mouseMove(QMouseEvent *);

public:
	QCustomPlot_custom(QWidget *parent = NULL);

	~QCustomPlot_custom() {}

private:
	QCPLayer *cursorLayer;
	QCPCursor cursor;

	void manageCursor(double x, double y);

public:
	void init(QVector<double> xdata, QVector<double> ydata);
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
