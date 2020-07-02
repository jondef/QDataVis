//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#ifndef GUI_APP_QCUSTOMPLOT_CUSTOM_H
#define GUI_APP_QCUSTOMPLOT_CUSTOM_H

#include "qcustomplot.h"

struct QCPCursor {
	QCPItemLine *hLine;
	QCPItemLine *vLine;
	QCPItemText *cursorText;
};

class QCustomPlot_custom : public QCustomPlot {
Q_OBJECT

public:
	explicit QCustomPlot_custom(QWidget *parent = nullptr);

	~QCustomPlot_custom() override = default;

public:
	void manageCursor(double x, double y);

	QCPLayer *cursorLayer;
	QCPCursor cursor;

	QCPItemText *textLabel = new QCPItemText(this);
	QCPItemTracer *graphTracer = new QCPItemTracer(this);

public slots:

	void onMouseMoveReplotCursor(QMouseEvent *event);

	void traceGraph(QMouseEvent *event);

	void stickAxisToZeroLines();

	void initGraph();

public slots:

	void plotOppositeAxesConnection();

	void plotAxisLockDrag();

	void plotAxisLockZoom();

	void plotContextMenuRequest(QPoint pos);

	void plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

	void plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

	void plotContextMenuRemoveAllGraphs();

	void plotContextMenuRemoveSelectedGraph();

	void plotMoveLegend();

	void updateColors();
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
