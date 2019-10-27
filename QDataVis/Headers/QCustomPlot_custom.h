//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#ifndef GUI_APP_QCUSTOMPLOT_CUSTOM_H
#define GUI_APP_QCUSTOMPLOT_CUSTOM_H

#include "qcustomplot.h"

#define USING_LAYER true

struct QCPCursor {
	QCPItemLine *hLine;
	QCPItemLine *vLine;
	QCPItemText *cursorText;
};

class QCustomPlot_custom : public QCustomPlot {
Q_OBJECT

public:
	explicit QCustomPlot_custom(QWidget *parent = nullptr);

	~QCustomPlot_custom() = default;

public:
	void manageCursor(double x, double y);

	QCPLayer *cursorLayer;
	QCPCursor cursor;
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
