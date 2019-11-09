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
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();


public slots:

	void replotGraphsOnRangeChange(QCPRange range);

	void GraphParametersChanged();

	void QLineEdit_addFunction_returnPressed();

	void QPushButton_PlotPoints_clicked();

	void QPushButton_deleteFunction_clicked();

	void savePlotImage();

	void onResult(QNetworkReply *reply);

	void replyFini(QNetworkReply *reply);


public:
	Ui::MainWindow *ui;

	PlotPropertiesWindow *plotWindow = new PlotPropertiesWindow();

	QColorDialog *popUpColorDialog = new QColorDialog(plotWindow);
	QFontDialog *popUpFontDialog = new QFontDialog(plotWindow);
	QPenDialog *popUpPenDialog = new QPenDialog(plotWindow);


private:
	QList<QCPGraph *> *pointsGraphList = new QList<QCPGraph *>; // stored pointers of the points graphs

	QMap<QCPGraph *, BinaryTree *> *mFunctionGraph = new QMap<QCPGraph *, BinaryTree *>; // stores pointers of the function graphs and tree

	QHash<QListWidgetItem *, QCPTextElement *> *graphTextElements = new QHash<QListWidgetItem *, QCPTextElement *>;

	std::vector<std::future<void>> m_Futures;


public:
	static QVector<double> generateXArray(double lowerLim, double upperLim, unsigned int length);


private:
	QNetworkAccessManager *networkManager = new QNetworkAccessManager();

	void statusBarMsg(const QString &msg, int time = 2000);

	void changeAxisTicker(QCPAxis *axis, const QString &value);

	inline void setUpTitlePageConnections();

	inline void setUpGeneralPageConnections();

	inline void setUpAxesPageConnections();

	void calculateAndDrawFunction(QString &function);

	static QColor getGraphColor(int colorIndex);

	QPointF getClosestPoint(QCPGraph *graph, QPoint point);
};


#endif //GUI_APP_MAINWINDOW_H