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

public slots:

	void QLineEdit_addFunction_returnPressed();

	void QPushButton_PlotPoints_clicked();

public:
    explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow();

private:
	Ui::MainWindow *ui;

	void statusBarMsg(const char *msg, int time = 2000);

    static QVector<double> generateXArray(int lowerLim, int upperLim, unsigned int length);

	QList<QCPGraph *> *graphList = new QList<QCPGraph *>; // stored pointers to the points graphs


	void plot(const QVector<double> &xArray, const QVector<double> &yArray);

};

#endif //GUI_APP_MAINWINDOW_H