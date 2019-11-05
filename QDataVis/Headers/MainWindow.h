//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_MAINWINDOW_H
#define GUI_APP_MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QObject>
#include "ui_mainwindow.h"
#include "QCustomPlot_custom.h"
#include "Thread.h"
#include "BinaryTree.h"
#include "qpendialog.h"
#include <future>
#include "PlotPropertiesWindow.h"


#define QCUSTOMPLOT_USE_OPENGL // use openGL


class MainWindow : public QMainWindow {
Q_OBJECT


public:
	static QVector<double> generateXArray(int lowerLim, int upperLim, unsigned int length); // enable signals and slots

	Ui::MainWindow *ui;

public slots:

	void stickAxisToZeroLines();

	void GraphParametersChanged();

	void QLineEdit_addFunction_returnPressed();

	void QPushButton_PlotPoints_clicked();

	void QPushButton_deleteFunction_clicked();

	void plotOppositeAxesConnection();

	void plotAxisLockDrag();

	void plotAxisLockZoom();

	void plotGraphClicked(QCPAbstractPlottable *plottable, int dataIndex);

	void plotContextMenuRequest(QPoint pos);

	void plotAxisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part);

	void plotLegendGraphDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

	void plotContextMenuRemoveAllGraphs();

	void plotContextMenuRemoveSelectedGraph();

	void plotMoveLegend();

	void savePlotImage();

	void Test(QMouseEvent *event);

	void onResult(QNetworkReply *reply);

	void replyFini(QNetworkReply *reply);

	void onMouseMoveReplotCursor(QMouseEvent *event);


public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

	PlotPropertiesWindow *plotWindow = new PlotPropertiesWindow();

	QColorDialog *popUpColorDialog = new QColorDialog(plotWindow);
	QFontDialog *popUpFontDialog = new QFontDialog(plotWindow);
	QPenDialog *popUpPenDialog = new QPenDialog(plotWindow);

private:

	QNetworkAccessManager *networkManager = new QNetworkAccessManager();


	void initGraph();

	void statusBarMsg(const QString &msg, int time = 2000);

	QList<QCPGraph *> *pointsGraphList = new QList<QCPGraph *>; // stored pointers of the points graphs

	QList<QCPGraph *> *functionGraphList = new QList<QCPGraph *>; // stored pointers of the function graph

	QHash<QListWidgetItem *, QCPTextElement *> *graphTextElements = new QHash<QListWidgetItem *, QCPTextElement *>;

	std::vector<std::future<void>> m_Futures;

	QCPItemText *textLabel;
	QCPItemTracer *graphTracer;

	void changeAxisTicker(QCPAxis *axis, const QString &value);

	inline void setUpTitlePageConnections();

	inline void setUpGeneralPageConnections();

	inline void setUpAxesPageConnections();

	void calculateAndDrawFunction(QString &function);

	static QColor getGraphColor(int colorIndex);

};


#endif //GUI_APP_MAINWINDOW_H