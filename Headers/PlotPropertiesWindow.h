//
// Created by John on 29.09.2019.
//

#ifndef GUI_APP_PLOTPROPERTIESWINDOW_H
#define GUI_APP_PLOTPROPERTIESWINDOW_H

#include <QWidget>
#include <QtCore>
#include <QMainWindow>
#include "ui_plotpropertieswindow.h"


class PlotPropertiesWindow : public QWidget {
Q_OBJECT

public slots:


public:
	explicit PlotPropertiesWindow(QWidget *parent = nullptr);

	~PlotPropertiesWindow();


private:
	Ui::Form *ui;


};


#endif //GUI_APP_PLOTPROPERTIESWINDOW_H
