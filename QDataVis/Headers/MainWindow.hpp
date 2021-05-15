//
// Created by John on 12/06/2019.
//

#ifndef GUI_APP_MAINWINDOW_H
#define GUI_APP_MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <future>
#include <memory>
#include "ui_MainWindow.h"
#include "QCustomPlotCustom.hpp"
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

    void closeEvent(QCloseEvent *event) override {
        writeSettings();
    }

public slots:

    void setSelectedDataSet(DataSet *dataSet);

private slots:

    void addFunctionGraph();

    void removeFunctionGraph();

    void removePointGraph();

    void updateColors(bool checked);

    void graphDoubleClicked(QListWidgetItem *item);

    void addPointsGraph();

    void savePlotImage();

    void addRegression();

    void readSettings();

    void writeSettings();

    void exportData();

    void importData();

    inline void statusBarMsg(const QString &msg, int time = 2000) {
        ui->statusBar->showMessage(msg, time);
    }

private:
    friend class PlotPropertiesWindow;

    Ui::uiMainWindow *ui;

    PlotPropertiesWindow *plotWindow = new PlotPropertiesWindow(this);
    PointWindow *pointGraphDialog = new PointWindow(this);
};


#endif //GUI_APP_MAINWINDOW_H