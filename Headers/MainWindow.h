//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_MAINWINDOW_H
#define GUI_APP_MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"
#include "dependencies/qcustomplot/include/qcustomplot.h"

#define QCUSTOMPLOT_USE_OPENGL // use openGL

#include "Thread.h"
#include "BinaryTree.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT // enable signals and slots

public:
    explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

    QCustomPlot *customPlot = new QCustomPlot;
    QCPCurve *plotCurves = new QCPCurve(customPlot->xAxis, customPlot->yAxis);

    QList<QPair<double, double>> *pPointsList = new QList<QPair<double, double>>{};


public slots:
    void QLineEdit_addFunction_returnPressed();

    void QPushButton_PlotAllPoints_clicked();

private:
	Ui::MainWindow *ui;
    void statusBarMsg(const char *msg, int time = 2000);

    static QVector<double> generateXArray(int lowerLim, int upperLim, unsigned int length);


    // CONTAINERS
    QWidget *pQWidget_centralWidget = new QWidget;
    QGridLayout *pQGridLayout_mainLayout = new QGridLayout;

    QTabWidget *pQTabWidget_graphPlottables = new QTabWidget();
    QTabWidget *pQTabWidget_graphOptions = new QTabWidget();

    // WIDGETS INSTANCES
    QLabel *pQLabel_addFunction = new QLabel("Add function");
    QLineEdit *pQLineEdit_addFunction = new QLineEdit();

    QPushButton *pQPushButton_Quit = new QPushButton("Quit");

    QPushButton *pQPushButton_PlotAllPoints = new QPushButton("Plot points");

    QTextEdit *pQTextEdit_pointsList = new QTextEdit();

	QProgressBar *progressBar = new QProgressBar;


	QList<QCPGraph *> *graphList = new QList<QCPGraph *>; // stored pointers to the points graphs
    // PLOT FUNCTIONS
    void initGraph();

    void plot(const QVector<double> &xArray, const QVector<double> &yArray);

};

#endif //GUI_APP_MAINWINDOW_H