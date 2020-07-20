//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#ifndef GUI_APP_QCUSTOMPLOT_CUSTOM_H
#define GUI_APP_QCUSTOMPLOT_CUSTOM_H

#define QCUSTOMPLOT_USE_OPENGL // use openGL

#include "qcustomplot.h"
#include "DataSet.hpp"

Q_DECLARE_METATYPE(DataSet *)

struct QCPCursor {
	QCPItemLine *hLine;
	QCPItemLine *vLine;
	QCPItemText *cursorText;
};

static const QList<QColor> colors = {
		QColor(31, 119, 180),
		QColor(255, 127, 14),
		QColor(44, 160, 44),
		QColor(214, 39, 40),
		QColor(148, 103, 189),
		QColor(140, 86, 75),
		QColor(244, 119, 194),
		QColor(127, 127, 127),
		QColor(188, 189, 34),
		QColor(23, 190, 207)
};

class QCustomPlotCustom : public QCustomPlot {
Q_OBJECT

public:
	explicit QCustomPlotCustom(QWidget *parent = nullptr);

	~QCustomPlotCustom() override;

public:
	QCPLayer *cursorLayer;
	QCPCursor cursor;

	QCPItemText *textLabel = new QCPItemText(this);
	QCPItemTracer *graphTracer = new QCPItemTracer(this);

	QList<DataSet *> mDataSets;

	QCPGraph *selectedGraph = nullptr;

	int mGlobalPointDensity = 1000;

public slots:

	void onMouseMoveReplotCursor(QMouseEvent *event);

	void traceGraph(QMouseEvent *event);

	void stickAxisToZeroLines();

	void initGraph();

	void centerPlot();

	void replotGraphsOnRangeChange(QCPRange range);

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

	void showHideGraphTracer(QMouseEvent *event = nullptr);

	static QColor getGraphColor(int colorIndex);

	void addFunctionGraph(const QString &functionString, QListWidgetItem *listWidgetItem);

	void addPointsGraph(const QString &functionString, QListWidgetItem *listWidgetItem);

	void deleteGraph(DataSet *graph);

	void globalPointDensityChanged(int density);
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
