//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#include "QCustomPlot_custom.h"


QCustomPlot_custom::QCustomPlot_custom(QWidget *parent) {
	// enable openGL
	setOpenGl(true, 16); // enable openGL

	// * cursor stuff
	this->addLayer("cursorLayer", nullptr, QCustomPlot::limAbove);
	this->cursorLayer = this->layer("cursorLayer");
	this->cursorLayer->setMode(QCPLayer::lmBuffered);
	//Cursor:
	QPen qpen = QPen(Qt::DashDotLine);
	this->cursor.hLine = new QCPItemLine(this);
	this->cursor.hLine->setPen(qpen);
	this->cursor.vLine = new QCPItemLine(this);
	this->cursor.vLine->setPen(qpen);
	this->cursor.cursorText = new QCPItemText(this);
	this->cursor.cursorText->setFont(QFont(font().family(), 8));
	//Add to layer
	this->cursor.hLine->setLayer("cursorLayer");
	this->cursor.vLine->setLayer("cursorLayer");
	this->cursor.cursorText->setLayer("cursorLayer");

	// * graph tracer stuff
	textLabel->setLayer(this->cursorLayer);
	textLabel->setFont(QFont("MS Shell Dlg 2", -1, QFont::Bold));
	textLabel->setSelectable(false);
	textLabel->setBrush(QBrush(QColor(Qt::white)));
	textLabel->setAntialiased(true);
	textLabel->position->setType(QCPItemPosition::ptPlotCoords);
	textLabel->setVisible(false);
	graphTracer->setLayer(this->cursorLayer);
	graphTracer->setInterpolating(true);
	graphTracer->setStyle(QCPItemTracer::tsCircle);
	graphTracer->setPen(QPen(Qt::red));
	graphTracer->setBrush(Qt::red);
	graphTracer->setSize(7);
	graphTracer->setSelectable(false);
	graphTracer->setVisible(false);


	connect(this, SIGNAL(mouseMove(QMouseEvent * )), this, SLOT(traceGraph(QMouseEvent * )));
	initGraph();
}

void QCustomPlot_custom::initGraph() {


	// set axes ranges
	xAxis->setRange(-10, 10);
	yAxis->setRange(-10, 10);
	// configure right and top axis to show ticks but no labels:
	// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)


	// legend initialization
	legend->setVisible(false);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	legend->setFont(legendFont);
	legend->setSelectedFont(legendFont);
	legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items


	// ! GRAPH RELATED CONNECTIONS
	// * axes configuration
	// connect slot that ties some axis selections together (especially opposite axes):
	connect(this, SIGNAL(selectionChangedByUser()), this, SLOT(plotOppositeAxesConnection()));
	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(this, SIGNAL(mousePress(QMouseEvent * )), this, SLOT(plotAxisLockDrag()));
	connect(this, SIGNAL(mouseWheel(QWheelEvent * )), this, SLOT(plotAxisLockZoom()));
	// make bottom and left axes transfer their ranges to top and right axes:
	connect(this->xAxis, SIGNAL(rangeChanged(QCPRange)), this->xAxis2, SLOT(setRange(QCPRange)));
	connect(this->yAxis, SIGNAL(rangeChanged(QCPRange)), this->yAxis2, SLOT(setRange(QCPRange)));

	// * click interaction
	// axis label double click
	connect(this, SIGNAL(axisDoubleClick(QCPAxis * , QCPAxis::SelectablePart, QMouseEvent * )), this,
	        SLOT(plotAxisLabelDoubleClick(QCPAxis * , QCPAxis::SelectablePart)));
	// legend double click
	connect(this, SIGNAL(legendDoubleClick(QCPLegend * , QCPAbstractLegendItem * , QMouseEvent * )), this,
	        SLOT(plotLegendGraphDoubleClick(QCPLegend * , QCPAbstractLegendItem * )));

	// * context menu
	// setup policy and connect slot for context menu popup:
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(plotContextMenuRequest(QPoint)));


//	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//	dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
//	ui->customPlot->xAxis->setTicker(dateTicker);
//	double now = QDateTime::currentDateTime().toTime_t();
//	ui->customPlot->xAxis->setRange(now - now / 10, now);
//	ui->customPlot->yAxis->setRange(0, 10000);

	this->replot();
}

void QCustomPlot_custom::onMouseMoveReplotCursor(QMouseEvent *event) {
	double x = this->xAxis->pixelToCoord(event->pos().x());
	double y = this->yAxis->pixelToCoord(event->pos().y());

	this->manageCursor(x, y);

	this->layer("cursorLayer")->replot();
}


void QCustomPlot_custom::stickAxisToZeroLines() {
	this->axisRect()->setAutoMargins(QCP::msNone);

	int pxx = this->yAxis->coordToPixel(0);
	int pxy = this->xAxis->coordToPixel(0);
	this->xAxis->setOffset(-this->axisRect()->height() - this->axisRect()->top() + pxx);
	this->yAxis->setOffset(this->axisRect()->left() - pxy);
}


void QCustomPlot_custom::traceGraph(QMouseEvent *event) {
	if (this->selectedGraphs().size() == 1) {
		QCPGraphDataContainer::const_iterator it = this->selectedGraphs().first()->data()->constEnd();
		QVariant details;

		if (this->selectedGraphs().first()->selectTest(event->pos(), false, &details)) {
			QCPDataSelection dataPoints = details.value<QCPDataSelection>();

			if (dataPoints.dataPointCount() < 1) {
				// abort if event position is invalid
				return;
			}
			if (this->selectedGraphs().first()->data()->at(dataPoints.dataRange().begin())->value <
			    this->selectedGraphs().first()->data()->at(dataPoints.dataRange().end())->value) {
				textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignRight);
			} else if (this->selectedGraphs().first()->data()->at(dataPoints.dataRange().begin())->value >
			           this->selectedGraphs().first()->data()->at(dataPoints.dataRange().end())->value) {
				textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignLeft);
			}
			it = this->selectedGraphs().first()->data()->at(dataPoints.dataRange().begin());

		}
		graphTracer->setVisible(true);
		graphTracer->setGraph(this->selectedGraphs().first());
		graphTracer->setGraphKey(it->key);

		textLabel->setText(QString("(%1, %2)").arg(QString::number(it->key, 'f', 3)).arg(QString::number(it->value, 'f', 3)));
		textLabel->setVisible(true);
		textLabel->position->setCoords(it->key, it->value + (this->yAxis->range().upper - this->yAxis->range().lower) * 0.01);
	} else { // no graph or more than one graphs selected
		textLabel->setVisible(false);
		graphTracer->setVisible(false);
	}
	this->layer("cursorLayer")->replot();
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


void QCustomPlot_custom::plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part) {
	// Set an axis label by double clicking on it
	if (part ==
	    QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal,
		                                         axis->label(), &ok);
		if (ok) {
			axis->setLabel(newLabel);
			this->replot();
		}
	}
}

void QCustomPlot_custom::plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item) {
	// Rename a graph by double clicking on its legend item
	Q_UNUSED(legend)
	if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
	{
		QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem *>(item);
		bool ok;
		QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal,
		                                        plItem->plottable()->name(), &ok);
		if (ok) {
			plItem->plottable()->setName(newName);
			this->replot();
		}
	}
}

void QCustomPlot_custom::plotMoveLegend() {
	if (QAction *contextAction = qobject_cast<QAction *>(
			sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok) {
			this->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment) dataInt);
			this->replot();
		}
	}
}

void QCustomPlot_custom::plotContextMenuRemoveAllGraphs() {
	this->clearGraphs();
	this->replot();
}

void QCustomPlot_custom::plotContextMenuRemoveSelectedGraph() {
	if (this->selectedGraphs().size() > 0) {
		this->removeGraph(this->selectedGraphs().first());
		this->replot();
	}
}

void QCustomPlot_custom::plotAxisLockDrag() {
	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, both directions may be dragged

	if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		this->axisRect()->setRangeDrag(this->xAxis->orientation());
	else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		this->axisRect()->setRangeDrag(this->yAxis->orientation());
	else
		this->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
}


void QCustomPlot_custom::plotAxisLockZoom() {
	// if an axis is selected, only allow the direction of that axis to be zoomed
	// if no axis is selected, both directions may be zoomed

	if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		this->axisRect()->setRangeZoom(this->xAxis->orientation());
	else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		this->axisRect()->setRangeZoom(this->yAxis->orientation());
	else
		this->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}


void QCustomPlot_custom::plotContextMenuRequest(QPoint pos) {
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (this->legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{
		menu->addAction("Move to top left", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(plotMoveLegend()))->setData(
				(int) (Qt::AlignBottom | Qt::AlignLeft));
	} else  // general context menu on graphs requested
	{
		menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
		if (this->selectedGraphs().size() > 0)
			menu->addAction("Remove selected graph", this, SLOT(plotContextMenuRemoveSelectedGraph()));
		if (this->graphCount() > 0)
			menu->addAction("Remove all graphs", this, SLOT(plotContextMenuRemoveAllGraphs()));
	}

	menu->popup(this->mapToGlobal(pos));
}


void QCustomPlot_custom::plotOppositeAxesConnection() {
	/*
	 normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
	 the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
	 and the axis base line together. However, the axis label shall be selectable individually.

	 The selection state of the left and right axes shall be synchronized as well as the state of the
	 bottom and top axes.

	 Further, we want to synchronize the selection of the graphs with the selection state of the respective
	 legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
	 or on its legend item.
	*/

	// make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	    this->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
	    this->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	    this->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		this->xAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		this->xAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}
	// make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis) ||
	    this->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
	    this->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) ||
	    this->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels)) {
		this->yAxis2->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
		this->yAxis->setSelectedParts(QCPAxis::spAxis | QCPAxis::spTickLabels);
	}

	// synchronize selection of graphs with selection of corresponding legend items:
	for (int i = 0; i < this->graphCount(); ++i) {
		QCPGraph *graph = this->graph(i);
		QCPPlottableLegendItem *item = this->legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected()) {
			item->setSelected(true);
			graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
		}
	}
}