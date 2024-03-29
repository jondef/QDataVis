//
// Created by John on 19.10.2019.
//

// ! Taken and modified from
// https://stackoverflow.com/questions/49477877/qcustomplot-replot-qcplayer
//

#ifndef GUI_APP_QCUSTOMPLOT_CUSTOM_H
#define GUI_APP_QCUSTOMPLOT_CUSTOM_H

#define QCUSTOMPLOT_USE_OPENGL // use openGL

#include "qcp.h"
#include "DataSet.hpp"
#include "QCPItemTextCustom.hpp"


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

public slots:

    void globalPointDensityChanged(int density);

    void updateColors();

    void onMouseMoveReplotCursor(QMouseEvent *event);

    void stickAxisToZeroLines();

    void setPlotRange(QCPRange xAxisRange, QCPRange yAxisRange);

    void replotGraphsOnRangeChange();

    void addFunctionGraph(const QString &functionString, QListWidgetItem *listWidgetItem);

    void addPointsGraph(const QString &functionString, QListWidgetItem *listWidgetItem);

    void deleteGraph(DataSet *graph);

    void stickAxisToZeroLine(bool enabled);

    void setCursor(bool enabled);

    inline static QColor getGraphColor(int colorIndex) {
        return colors.at(colorIndex % 10);
    }

private  slots:

    void traceGraph(QMouseEvent *event);

    void plotOppositeAxesConnection();

    void plotAxisLockDragZoom();

    void plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

    void plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

private:
    static QVariant QCPRangeInterpolator(const QCPRange &start, const QCPRange &end, qreal progress);

signals:
    void updateDataSetData(DataSet *, QSharedPointer<QCPGraphDataContainer>);

public:
    QCPLayer *cursorLayer;
    QCPCursor cursor;

    QCPItemTextCustom *textLabel = new QCPItemTextCustom(this);
    QCPItemTracer *graphTracer = new QCPItemTracer(this);

    QList<DataSet *> mDataSets;

    int mGlobalPointDensity = 1000;
};


#endif //GUI_APP_QCUSTOMPLOT_CUSTOM_H
