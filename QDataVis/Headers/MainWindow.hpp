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


class MainWindow : public QMainWindow {
Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);

	~MainWindow() override;

public slots:

	void addFunctionGraph();

	void removeGraph();

	void savePlotImage();

	void updateColors(bool checked);

	void graphDoubleClicked(QListWidgetItem *item);

	void addPointsGraph();

private:
	friend class PlotPropertiesWindow;

	Ui::uiMainWindow *ui;

	PlotPropertiesWindow *plotWindow = new PlotPropertiesWindow(this);
	PointWindow *pointGraphDialog = new PointWindow(this);

private:
	std::vector<std::future<void>> m_Futures;

public:


private:
	inline void statusBarMsg(const QString &msg, int time = 2000);
};


#endif //GUI_APP_MAINWINDOW_H