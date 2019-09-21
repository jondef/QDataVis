//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_MAINWINDOW_H
#define GUI_APP_MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "dependencies/qcustomplot/include/qcustomplot.h"
#include "Thread.h"
#include "BinaryTree.h"

#define QCUSTOMPLOT_USE_OPENGL // use openGL


class MainWindow : public QMainWindow {
Q_OBJECT // enable signals and slots

public slots:

	void QLineEdit_addFunction_returnPressed();

	void QPushButton_PlotPoints_clicked();

	void QPushButton_deleteFunction_clicked();

	void plotOppositeAxesConnection();

	void plotAxisLockDrag();

	void plotAxisLockZoom();

	void plotGraphClicked(QCPAbstractPlottable *plottable, int dataIndex);

	void plotContextMenuRequest(QPoint pos);

	void plotTitleDoubleClicked(QMouseEvent *event);

	void plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

	void plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

	void plotContextMenuRemoveAllGraphs();

	void plotContextMenuRemoveSelectedGraph();

	void plotMoveLegend();

	void savePlotImage();


public:
    explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

private:
	Ui::MainWindow *ui;

	void initGraph();

	void statusBarMsg(const QString &msg, int time = 2000);

    static QVector<double> generateXArray(int lowerLim, int upperLim, unsigned int length);

	QList<QCPGraph *> *pointsGraphList = new QList<QCPGraph *>; // stored pointers of the points graphs

	QList<QCPGraph *> *functionGraphList = new QList<QCPGraph *>; // stored pointers of the function graph

};

#endif //GUI_APP_MAINWINDOW_H