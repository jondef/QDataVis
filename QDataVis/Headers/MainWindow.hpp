//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_MAINWINDOW_H
#define GUI_APP_MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include <QObject>
#include <future>
#include <memory>
#include "ui_MainWindow.h"
#include "QCustomPlot_custom.hpp"
#include "Thread.hpp"
#include "BinaryTree.hpp"
#include "QPenDialog.hpp"
#include "PlotPropertiesWindow.hpp"
#include "PointWindow.hpp"


#define QCUSTOMPLOT_USE_OPENGL // use openGL

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

public slots:

	void replotGraphsOnRangeChange(QCPRange range);

	void GraphParametersChanged();

	void QLineEdit_addFunction_returnPressed();

	//void QPushButton_PlotPoints_clicked();

	void QPushButton_deleteFunction_clicked();

	void savePlotImage();

	void onResult(QNetworkReply *reply);

	void replyFini(QNetworkReply *reply);


private:
	friend class PlotPropertiesWindow;

	Ui::uiMainWindow *ui;

	PlotPropertiesWindow *plotWindow = new PlotPropertiesWindow(this);
	PointWindow *pointGraphDialog = new PointWindow(this);

private:
	QHash<QCPGraph *, BinaryTree *> *mFunctionGraph = new QHash<QCPGraph *, BinaryTree *>; // stores pointers of the function graphs and tree

	std::vector<std::future<void>> m_Futures;

public:
	static QVector<double> generateXArray(double lowerLim, double upperLim, unsigned int length);

	static QColor getGraphColor(int colorIndex);

	static int globalPointDensity;

private:
	QNetworkAccessManager *networkManager = new QNetworkAccessManager();

	void statusBarMsg(const QString &msg, int time = 2000);

	void changeAxisTicker(QCPAxis *axis, const QString &value);

	QPointF getClosestPoint(QCPGraph *graph, QPoint point);
};


#endif //GUI_APP_MAINWINDOW_H