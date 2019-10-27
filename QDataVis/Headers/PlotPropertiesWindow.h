//
// Created by John on 29.09.2019.
//

#ifndef GUI_APP_PLOTPROPERTIESWINDOW_H
#define GUI_APP_PLOTPROPERTIESWINDOW_H

#include <QWidget>
#include <QtCore>
#include <QColorDialog>
#include <QFontDialog>
#include "ui_plotpropertieswindow.h"
#include "QCustomPlot_custom.h"


class PlotPropertiesWindow : public QWidget {
Q_OBJECT

signals:

	void windowClosed();

public slots:

	void okButtonPressed();

public:
	explicit PlotPropertiesWindow(QWidget *parent = nullptr);

	~PlotPropertiesWindow();

	Ui::Form *ui;

	QFontDialog *titleFontDialog = new QFontDialog();
	QColorDialog *titleColorDialog = new QColorDialog();

};


#endif //GUI_APP_PLOTPROPERTIESWINDOW_H
