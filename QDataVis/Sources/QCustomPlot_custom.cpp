//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#include "QCustomPlot_custom.hpp"


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
	this->cursor.hLine->setSelectable(false);
	this->cursor.vLine = new QCPItemLine(this);
	this->cursor.vLine->setPen(qpen);
	this->cursor.vLine->setSelectable(false);
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
	graphTracer->setInterpolating(true);
//	graphTracer->setStyle(QCPItemTracer::tsCrosshair);

	connect(this, &QCustomPlot_custom::mouseMove, this, &QCustomPlot_custom::traceGraph);
	connect(this, &QCustomPlot_custom::mousePress, this, &QCustomPlot_custom::showHideGraphTracer);

	initGraph();
}

QCustomPlot_custom::~QCustomPlot_custom() {
//	delete cursorLayer;
	delete textLabel;
	delete graphTracer;
}

/**
 *
 * @param event
 */
void QCustomPlot_custom::showHideGraphTracer(QMouseEvent *event) {
	// if a plottable is clicked
	if (plottableAt(event->pos(), true)) {
		// if only one graph is selected
		if (selectedGraphs().size() == 1) {
			traceGraph(event);
			return;
		}
	}
	// if there is no plottable on click position
	textLabel->setVisible(false);
	graphTracer->setVisible(false);
}

void QCustomPlot_custom::updateColors() {
	setBackground(QBrush(QApplication::palette().color(backgroundRole())));
	QColor foregroundColor = QApplication::palette().color(foregroundRole());
	QPen foregroundPen = QPen(foregroundColor);

	for (QCPAxis *i : {xAxis, xAxis2, yAxis, yAxis2}) {
		i->setTickPen(foregroundPen);
		i->grid()->setPen(foregroundPen);
		i->grid()->setSubGridPen(foregroundPen);
		i->setSubTickPen(foregroundPen);
		i->setBasePen(foregroundPen);
		i->setLabelColor(foregroundColor);
		i->setTickLabelColor(foregroundColor);
	}
	replot();
}

void QCustomPlot_custom::initGraph() {
	xAxis->setRange(-10, 10);
	yAxis->setRange(-10, 10);

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
	connect(this, &QCustomPlot_custom::selectionChangedByUser, this, &QCustomPlot_custom::plotOppositeAxesConnection);
	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(this, &QCustomPlot_custom::mousePress, this, &QCustomPlot_custom::plotAxisLockDrag);
	connect(this, &QCustomPlot_custom::mouseWheel, this, &QCustomPlot_custom::plotAxisLockZoom);
	// make bottom and left axes transfer their ranges to top and right axes:
	connect(this->xAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this->xAxis2, QOverload<const QCPRange &>::of(&QCPAxis::setRange));
	connect(this->yAxis, QOverload<const QCPRange &>::of(&QCPAxis::rangeChanged), this->yAxis2, QOverload<const QCPRange &>::of(&QCPAxis::setRange));

	// * click interaction
	// axis label double click
	connect(this, &QCustomPlot_custom::axisDoubleClick, this, &QCustomPlot_custom::plotAxisLabelDoubleClick);
	// legend double click
	connect(this, &QCustomPlot_custom::legendDoubleClick, this, &QCustomPlot_custom::plotLegendGraphDoubleClick);

	// * context menu
	// setup policy and connect slot for context menu popup:
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &QCustomPlot_custom::customContextMenuRequested, this, &QCustomPlot_custom::plotContextMenuRequest);


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


void QCustomPlot_custom::centerPlot() {
	xAxis->setRange(-10, 10);
	yAxis->setRange(-10, 10);
	replot();
}

void QCustomPlot_custom::stickAxisToZeroLines() {
	this->axisRect()->setAutoMargins(QCP::msNone);

	int pxx = yAxis->coordToPixel(0);
	int pxy = xAxis->coordToPixel(0);
	xAxis->setOffset(-this->axisRect()->height() - this->axisRect()->top() + pxx);
	yAxis->setOffset(this->axisRect()->left() - pxy);
}

/**
 * todo: sqrt(-x^2-4*x+46)-4 -> fix tracer on this function
 * This function draws the text label and the graph tracer
 * at the nearest position to the mouse on the graph.
 */
void QCustomPlot_custom::traceGraph(QMouseEvent *event) {
	if (selectedGraphs().size() != 1) { return; }

	QCPGraphDataContainer::const_iterator it = this->selectedGraphs().first()->data()->constEnd();
	QVariant details;

	if (selectedGraphs().first()->selectTest(event->pos(), false, &details)) {
		QCPDataSelection dataPoints = details.value<QCPDataSelection>();

		// abort if event position is invalid
		if (dataPoints.dataPointCount() < 1) { return; }

		if (selectedGraphs().first()->data()->at(dataPoints.dataRange().begin())->value <
			selectedGraphs().first()->data()->at(dataPoints.dataRange().end())->value) {
			textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignRight);
		} else if (selectedGraphs().first()->data()->at(dataPoints.dataRange().begin())->value >
				   selectedGraphs().first()->data()->at(dataPoints.dataRange().end())->value) {
			textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignLeft);
		}
		it = selectedGraphs().first()->data()->at(dataPoints.dataRange().begin());
	}
	graphTracer->setVisible(true);
	graphTracer->setGraph(selectedGraphs().first());
	graphTracer->setGraphKey(it->key);
	graphTracer->position->setCoords(it->key, it->value);
//	qDebug() << graphTracer->position->key() << graphTracer->position->value();
	graphTracer->updatePosition();
	//qDebug() << it->key << it->value;
	textLabel->setText(QString("(%1, %2)").arg(QString::number(it->key, 'f', 3)).arg(QString::number(it->value, 'f', 3)));
	textLabel->setVisible(true);
	textLabel->position->setCoords(it->key, it->value + yAxis->range().size() * 0.01);

	this->layer("cursorLayer")->replot();
}


void QCustomPlot_custom::manageCursor(double x, double y) {
	cursor.hLine->start->setCoords(xAxis->range().lower, y);
	cursor.hLine->end->setCoords(xAxis->range().upper, y);

	cursor.vLine->start->setCoords(x, yAxis->range().lower);
	cursor.vLine->end->setCoords(x, yAxis->range().upper);

	cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
	cursor.cursorText->position->setCoords(QPointF(x, y));
	QPointF pp = cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
	cursor.cursorText->position->setPixelPosition(pp);
}


void QCustomPlot_custom::plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part) {
	// Set an axis label by double clicking on it
	if (part == QCPAxis::spAxisLabel) { // only react when the actual axis label is clicked, not tick label or axis backbone
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
	if (item) { // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
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
	// make sure this slot is really called by a context menu action, so it carries the data we need
	if (QAction *contextAction = qobject_cast<QAction *>(sender())) {
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
	if (!this->selectedGraphs().empty()) {
		this->removeGraph(this->selectedGraphs().first());
		this->replot();
	}
}

void QCustomPlot_custom::plotAxisLockDrag() {
	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, both directions may be dragged
	if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		this->axisRect()->setRangeDrag(this->xAxis->orientation());
	} else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		this->axisRect()->setRangeDrag(this->yAxis->orientation());
	} else {
		this->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
	}
}


void QCustomPlot_custom::plotAxisLockZoom() {
	// if an axis is selected, only allow the direction of that axis to be zoomed
	// if no axis is selected, both directions may be zoomed
	if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		this->axisRect()->setRangeZoom(this->xAxis->orientation());
	} else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
		this->axisRect()->setRangeZoom(this->yAxis->orientation());
	} else {
		this->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
	}
}


void QCustomPlot_custom::plotContextMenuRequest(QPoint pos) {
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (this->legend->selectTest(pos, false) >= 0) { // context menu on legend requested
		menu->addAction("Move to top left", this, &QCustomPlot_custom::plotMoveLegend)->setData(
				(int) (Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, &QCustomPlot_custom::plotMoveLegend)->setData(
				(int) (Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, &QCustomPlot_custom::plotMoveLegend)->setData(
				(int) (Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, &QCustomPlot_custom::plotMoveLegend)->setData(
				(int) (Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, &QCustomPlot_custom::plotMoveLegend)->setData(
				(int) (Qt::AlignBottom | Qt::AlignLeft));
	} else { // general context menu on graphs requested
		menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
		if (!this->selectedGraphs().empty()) {
			menu->addAction("Remove selected graph", this, &QCustomPlot_custom::plotContextMenuRemoveSelectedGraph);
		}
		if (this->graphCount() > 0) {
			menu->addAction("Remove all graphs", this, &QCustomPlot_custom::plotContextMenuRemoveAllGraphs);
		}
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