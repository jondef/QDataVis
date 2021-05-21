//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//
//	QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
//	dateTicker->setDateTimeFormat("d. MMMM\nyyyy");
//	ui->customPlot->xAxis->setTicker(dateTicker);
//	double now = QDateTime::currentDateTime().toTime_t();
//	ui->customPlot->xAxis->setRange(now - now / 10, now);
//	ui->customPlot->yAxis->setRange(0, 10000);


#include <QtConcurrent/QtConcurrent>
#include <utility>
#include "QCustomPlotCustom.hpp"
#include "SettingManager.hpp"
#include "MainWindow.hpp"


QCustomPlotCustom::QCustomPlotCustom(QWidget *parent) : QCustomPlot(parent) {
    // enable openGL
    setOpenGl(SettingManager::getSetting("settings/useOpenGL").toBool(), 16); // enable openGL
    qDebug() << "using openGL:" << openGl();
    qRegisterAnimationInterpolator<QCPRange>(QCustomPlotCustom::QCPRangeInterpolator);
    qRegisterAnimationInterpolator<QPointF>(QCPItemTextCustom::QCPTextPosInterpolator);

    // region cursor stuff
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
    // endregion

    // region graph tracer stuff
    textLabel->setLayer(this->cursorLayer);
    textLabel->setFont(QFont("MS Shell Dlg 2", -1, QFont::Bold));
    textLabel->setSelectable(false);
    textLabel->setBrush(QBrush(QColor(Qt::white)));
    textLabel->setAntialiased(true);
    textLabel->setPositionAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    textLabel->setVisible(false);
    textLabel->topLeft->pixelPosition(); // this is required to so that when the user clicks on a graph to trace it for the 1st time, it's not slow.
    graphTracer->setLayer(this->cursorLayer);
    graphTracer->setInterpolating(true);
    graphTracer->setStyle(QCPItemTracer::tsCircle);
    graphTracer->setPen(QPen(Qt::red));
    graphTracer->setBrush(Qt::red);
    graphTracer->setSize(7);
    graphTracer->setSelectable(false);
    graphTracer->setVisible(false);
    graphTracer->setInterpolating(true);
    // endregion

    // * connections for graph tracers
    connect(this, &QCustomPlotCustom::mouseMove, this, &QCustomPlotCustom::traceGraph);
    connect(this, &QCustomPlotCustom::mouseRelease, this, &QCustomPlotCustom::traceGraph);
    connect(this, &QCustomPlotCustom::plottableClick, this, [this](QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event) { traceGraph(event); });
    // * axes connections
    connect(xAxis, qOverload<const QCPRange &>(&QCPAxis::rangeChanged), this, &QCustomPlotCustom::replotGraphsOnRangeChange);
    connect(this, &QCustomPlotCustom::axisDoubleClick, this, &QCustomPlotCustom::plotAxisLabelDoubleClick);
    // connect slot that ties some axis selections together (especially opposite axes):
    connect(this, &QCustomPlotCustom::selectionChangedByUser, this, &QCustomPlotCustom::plotOppositeAxesConnection);
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed
    connect(this, &QCustomPlotCustom::mousePress, this, &QCustomPlotCustom::plotAxisLockDragZoom);
    connect(this, &QCustomPlotCustom::mouseWheel, this, &QCustomPlotCustom::plotAxisLockDragZoom);
    // make bottom and left axes transfer their ranges to top and right axes
    connect(xAxis, qOverload<const QCPRange &>(&QCPAxis::rangeChanged), xAxis2, qOverload<const QCPRange &>(&QCPAxis::setRange));
    connect(yAxis, qOverload<const QCPRange &>(&QCPAxis::rangeChanged), yAxis2, qOverload<const QCPRange &>(&QCPAxis::setRange));
    // graph selection -> update widget on the right
    connect(this, &QCustomPlotCustom::plottableClick, this, [this](QCPAbstractPlottable *graph) {
        QList<DataSet *>::iterator dataSet = std::find_if(mDataSets.begin(), mDataSets.end(),[&](DataSet *dataSet) { return dataSet->graph == graph; });
        if (dataSet != mDataSets.end()) {
            dynamic_cast<MainWindow *>(parentWidget()->parentWidget())->setSelectedDataSet(*dataSet);
        }
    });
    connect(this, &QCustomPlotCustom::mouseRelease, this, [this]() {
        dynamic_cast<MainWindow *>(parentWidget()->parentWidget())->setSelectedDataSet(nullptr);
    });
    connect(this, &QCustomPlotCustom::updateDataSetData, this, [](DataSet *dataSet, QSharedPointer<QCPGraphDataContainer> data) {
        dataSet->graph->setData(std::move(data));
    });

    // region legend initialization
    legend->setVisible(false);
    QFont legendFont = font();
    legendFont.setPointSize(10);
    legend->setFont(legendFont);
    legend->setSelectedFont(legendFont);
    legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items
    connect(this, &QCustomPlotCustom::legendDoubleClick, this, &QCustomPlotCustom::plotLegendGraphDoubleClick);
    // endregion

    stickAxisToZeroLine(false);
    setCursor(false);
    setPlotRange(QCPRange(-10, 10), QCPRange(-10, 10));
}

QCustomPlotCustom::~QCustomPlotCustom() {
    removeLayer(cursorLayer);
    removeItem(textLabel);
    removeItem(graphTracer);
}

void QCustomPlotCustom::updateColors() {
    QColor backgroundColor = QApplication::palette().color(backgroundRole());
    QColor foregroundColor = QApplication::palette().color(foregroundRole());
    QPen foregroundPen = QPen(foregroundColor);
    setBackground(QBrush(backgroundColor));

    for (QCPAxis *i : {xAxis, xAxis2, yAxis, yAxis2}) {
        i->setTickPen(foregroundPen);
        i->grid()->setPen(foregroundPen);
        i->grid()->setSubGridPen(foregroundPen);
        i->grid()->setZeroLinePen(QPen(QBrush(foregroundColor), 2));
        i->setSubTickPen(foregroundPen);
        i->setBasePen(foregroundPen);
        i->setLabelColor(foregroundColor);
        i->setTickLabelColor(foregroundColor);
    }
    // update the graph tracer
    textLabel->setColor(foregroundColor); // text color
    textLabel->setBrush(QBrush(backgroundColor)); // background color
    // update cursor color
    cursor.cursorText->setColor(foregroundColor);
    cursor.cursorText->setBrush(QBrush(backgroundColor));
    cursor.hLine->setPen(QColor(foregroundColor));
    cursor.vLine->setPen(QColor(foregroundColor));

    replot();
}

void QCustomPlotCustom::onMouseMoveReplotCursor(QMouseEvent *event) {
    double x = this->xAxis->pixelToCoord(event->pos().x());
    double y = this->yAxis->pixelToCoord(event->pos().y());

    cursor.hLine->start->setCoords(xAxis->range().lower, y);
    cursor.hLine->end->setCoords(xAxis->range().upper, y);

    cursor.vLine->start->setCoords(x, yAxis->range().lower);
    cursor.vLine->end->setCoords(x, yAxis->range().upper);

    cursor.cursorText->setText(QString("(%1, %2)").arg(x).arg(y));
    cursor.cursorText->position->setCoords(QPointF(x, y));
    QPointF pp = cursor.cursorText->position->pixelPosition() + QPointF(60.0, -15.0);
    cursor.cursorText->position->setPixelPosition(pp);

    this->layer("cursorLayer")->replot();
}


void QCustomPlotCustom::setPlotRange(QCPRange xAxisRange, QCPRange yAxisRange) {
    QVariant start;
    QVariant end;

    QPropertyAnimation *animation1 = new QPropertyAnimation(xAxis, "range", this);
    animation1->setDuration(1000);
    start.setValue(xAxis->range());
    animation1->setStartValue(start);
    end.setValue(xAxisRange);
    animation1->setEndValue(end);
    animation1->setDirection(QAbstractAnimation::Forward);
    animation1->setEasingCurve(QEasingCurve::InOutQuart);

    QPropertyAnimation *animation2 = new QPropertyAnimation(yAxis, "range", this);
    animation2->setDuration(1000);
    start.setValue(yAxis->range());
    animation2->setStartValue(start);
    end.setValue(yAxisRange);
    animation2->setEndValue(end);
    animation2->setDirection(QAbstractAnimation::Forward);
    animation2->setEasingCurve(QEasingCurve::InOutQuart);

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(animation1);
    group->addAnimation(animation2);
    group->start(QAbstractAnimation::DeleteWhenStopped);
    connect(group, &QParallelAnimationGroup::finished, this, [this]() { replot(QCustomPlot::rpQueuedReplot); });
}

/**
 * ! GCPCurve has performance issues
 * Graphs are used to display single-valued data.
 * Single-valued means that there should only be one data point per unique key coordinate.
 * In other words, the graph can't have loops. If you do want to plot non-single-valued curves,
 * rather use the QCPCurve plottable.
 *
 * This function is used when adding a new function graph
 */
void QCustomPlotCustom::addFunctionGraph(const QString &functionString, QListWidgetItem *listWidgetItem) {
    DataSet *pDataSet = new DataSet();

    pDataSet->graph = new QCPGraph(xAxis, yAxis);
    pDataSet->graph->setName(functionString);
    pDataSet->listWidgetItem = listWidgetItem;
    pDataSet->displayName = functionString;
    pDataSet->graph->addToLegend();

    QtConcurrent::run(QThreadPool::globalInstance(), [this, pDataSet, functionString]() {
        pDataSet->binaryTree = new BinaryTree(functionString);
        emit updateDataSetData(pDataSet, pDataSet->binaryTree->calculateTree(xAxis->range().lower, xAxis->range().upper, mGlobalPointDensity));
        replot(QCustomPlotCustom::rpQueuedReplot);
    });
    pDataSet->changeColor(getGraphColor(mDataSets.size()));
    pDataSet->configureListWidgetItem();
    mDataSets.append(pDataSet);
}

void QCustomPlotCustom::deleteGraph(DataSet *graph) {
    removeGraph(graph->graph);
    mDataSets.removeOne(graph);
    replot();
}

/**
 * This function is used when adding a new points graph
 */
void QCustomPlotCustom::addPointsGraph(const QString &graphName, QListWidgetItem *listWidgetItem) {
    DataSet *pDataSet = new DataSet();

    pDataSet->displayName = graphName;
    pDataSet->listWidgetItem = listWidgetItem;
    pDataSet->graph = new QCPGraph(xAxis, yAxis);
    pDataSet->graph->setName(graphName);

    pDataSet->changeColor(getGraphColor(mDataSets.size()));
    pDataSet->configureListWidgetItem();

    mDataSets.append(pDataSet);
    replot();
}

void QCustomPlotCustom::globalPointDensityChanged(int density) {
    mGlobalPointDensity = density;
    replotGraphsOnRangeChange();
}

/**
 * ! When making changes here, make sure it also works with opengl.
 * Updating the dataSet while a replot is in progress must be avoided because
 * it will crash. Solution is to update the data in the main thread.
 */
void QCustomPlotCustom::replotGraphsOnRangeChange() {
    QThreadPool::globalInstance()->clear(); // clear the queue

    QtConcurrent::run(QThreadPool::globalInstance(), [this]() {
        for (DataSet *dataSet : mDataSets) {
            if (dataSet->dataSetIsFunction()) {
                emit updateDataSetData(dataSet, dataSet->binaryTree->calculateTree(xAxis->range().lower, xAxis->range().upper, mGlobalPointDensity));
            }
        }
        replot(QCustomPlotCustom::rpQueuedReplot);
    });
}


void QCustomPlotCustom::stickAxisToZeroLines() {
    this->axisRect()->setAutoMargins(QCP::msNone);

    int pxx = yAxis->coordToPixel(0);
    int pxy = xAxis->coordToPixel(0);
    xAxis->setOffset(-this->axisRect()->height() - this->axisRect()->top() + pxx);
    yAxis->setOffset(this->axisRect()->left() - pxy);
}


/**
 * This function draws the text label and the graph tracer
 * at the nearest position to the mouse on the graph.
 */
void QCustomPlotCustom::traceGraph(QMouseEvent *event) {
    // (if event is a click AND mouse hasn't moved AND there's no plottable on click position) OR selectedGraphs is not equal to one
    if ((event->button() == Qt::LeftButton && !mMouseHasMoved && !plottableAt(event->pos(), true)) || selectedGraphs().size() != 1) {
        textLabel->setVisible(false);
        graphTracer->setVisible(false);
        this->layer("cursorLayer")->replot();
        return;
    }
    // cancel if event isn't a mouse move
    if (event->buttons() != Qt::NoButton) {
        return;
    }
    QCPGraph *selectedGraph = selectedGraphs().first();
    QVariant details;

    if ((bool) selectedGraph->selectTest(event->pos(), false, &details)) {
        QCPDataSelection dataPoints = details.value<QCPDataSelection>();
        // abort if event position is invalid // can happen if mouse is outside the plot
        if (dataPoints.dataPointCount() < 1) { return; }
        // abort if point of graph is nan, for example sqrt(x) for negative values.
        if (dataPoints.dataRange().end() > selectedGraph->data()->size()) { return; }

        QCPGraphDataContainer::const_iterator it_begin = selectedGraph->data()->at(dataPoints.dataRange().begin());
        QCPGraphDataContainer::const_iterator it_end = selectedGraph->data()->at(dataPoints.dataRange().end());
        // set tracer text alignment
        textLabel->setVisible(true);
        textLabel->setText(QString("(%1, %2)").arg(QString::number(it_begin->key, 'f', 3), QString::number(it_begin->value, 'f', 3)));

        QPropertyAnimation *animation1 = new QPropertyAnimation(textLabel, "position", this);
        animation1->setDuration(150);
        QVariant start;
        start.setValue(textLabel->position->coords());
        animation1->setStartValue(start);
        double rectWidth = xAxis->pixelToCoord(textLabel->topLeft->pixelPosition().x()) - xAxis->pixelToCoord(textLabel->topRight->pixelPosition().x());
        QVariant end;
        end.setValue(QPointF(it_begin->value < it_end->value ? (it_begin->key + rectWidth / 2) : (it_begin->key - rectWidth / 2), it_begin->value + yAxis->range().size() * 0.01));
        animation1->setEndValue(end);
        animation1->setDirection(QAbstractAnimation::Forward);
        animation1->setEasingCurve(QEasingCurve::Linear);
        connect(animation1, &QVariantAnimation::valueChanged, this, [this]() { layer("cursorLayer")->replot(); });
        animation1->start(QAbstractAnimation::DeleteWhenStopped);

        graphTracer->setVisible(true);
        graphTracer->setGraph(selectedGraph);
        graphTracer->setGraphKey(it_begin->key);

        graphTracer->updatePosition(); // if graphTracer position is nan, take next data point.
        if (_isnan(graphTracer->position->value())) {
            graphTracer->setGraphKey(it_end->key);
            textLabel->position->setCoords(it_end->key, it_end->value + yAxis->range().size() * 0.01);
        }
        this->layer("cursorLayer")->replot();
    }
}


void QCustomPlotCustom::plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part) {
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

void QCustomPlotCustom::plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item) {
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

/**
 * if an axis is selected, only allow the direction of that axis to be dragged / zoomed
 * if no axis is selected, both directions may be dragged / zoomed
 */
void QCustomPlotCustom::plotAxisLockDragZoom() {
    if (this->xAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        this->axisRect()->setRangeDrag(this->xAxis->orientation());
        this->axisRect()->setRangeZoom(this->xAxis->orientation());
    } else if (this->yAxis->selectedParts().testFlag(QCPAxis::spAxis)) {
        this->axisRect()->setRangeDrag(this->yAxis->orientation());
        this->axisRect()->setRangeZoom(this->yAxis->orientation());
    } else {
        this->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
        this->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    }
}


void QCustomPlotCustom::plotOppositeAxesConnection() {
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

void QCustomPlotCustom::stickAxisToZeroLine(bool enabled) {
    if (enabled) {
        connect(this, &QCustomPlotCustom::beforeReplot, this, &QCustomPlotCustom::stickAxisToZeroLines);
        replot();
    } else {
        disconnect(this, &QCustomPlotCustom::beforeReplot, this, &QCustomPlotCustom::stickAxisToZeroLines);
        axisRect()->setAutoMargins(QCP::msAll);
        xAxis->setOffset(0);
        yAxis->setOffset(0);
        replot();
    }
}


void QCustomPlotCustom::setCursor(bool enabled) {
    if (enabled) {
        connect(this, &QCustomPlotCustom::mouseMove, this, &QCustomPlotCustom::onMouseMoveReplotCursor);
        cursor.cursorText->setVisible(true);
    } else {
        disconnect(this, &QCustomPlotCustom::mouseMove,this, &QCustomPlotCustom::onMouseMoveReplotCursor);
        cursor.hLine->start->setCoords(0, 0);
        cursor.hLine->end->setCoords(0, 0);

        cursor.vLine->start->setCoords(0, 0);
        cursor.vLine->end->setCoords(0, 0);

        cursor.cursorText->setVisible(false);
        layer("cursorLayer")->replot();
    }
}

QVariant QCustomPlotCustom::QCPRangeInterpolator(const QCPRange &start, const QCPRange &end, qreal progress) {
    QVariant done;

    double upper;
    if (start.upper >= end.upper) {
        upper = start.upper - abs(start.upper - end.upper) * progress;
    } else {
        upper = start.upper + abs(start.upper - end.upper) * progress;
    }
    double lower;
    if (start.lower >= end.lower) {
        lower = start.lower - abs(start.lower - end.lower) * progress;
    } else {
        lower = start.lower + abs(start.lower - end.lower) * progress;
    }
    done.setValue(QCPRange(lower, upper));

    return done;
}

