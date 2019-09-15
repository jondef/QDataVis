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

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow {
Q_OBJECT // enable signals and slots

public slots:

	void QLineEdit_addFunction_returnPressed();

	void QPushButton_PlotPoints_clicked();

	void QPushButton_deleteFunction_clicked();

public:
    explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

private:
	Ui::MainWindow *ui;

	void statusBarMsg(const char *msg, int time = 2000);

    static QVector<double> generateXArray(int lowerLim, int upperLim, unsigned int length);

	QList<QCPGraph *> *pointsGraphList = new QList<QCPGraph *>; // stored pointers of the points graphs

	QList<QCPCurve *> *functionGraphList = new QList<QCPCurve *>; // stored pointers of the function graph

	void plot(const QVector<double> &xArray, const QVector<double> &yArray);

};

#endif //GUI_APP_MAINWINDOW_H