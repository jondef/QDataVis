//
// Created by John on 29.09.2019.
//

#ifndef GUI_APP_PLOTPROPERTIESWINDOW_H
#define GUI_APP_PLOTPROPERTIESWINDOW_H

#include <QWidget>
#include <QtCore>
#include <QColorDialog>
#include <QFontDialog>
#include "ui_PlotPropertiesWindow.h"
#include "QCustomPlot_custom.hpp"


class PlotPropertiesWindow : public QWidget {
Q_OBJECT

public:

	explicit PlotPropertiesWindow(QWidget *parent = nullptr);

	~PlotPropertiesWindow();

protected:

	void showEvent(QShowEvent *event) override;

signals:

	void windowClosed();

public slots:


private slots:

	void changeAxisTicker(QCPAxis *axis, const QString &value);

	void okButtonPressed();

	void setUpAxesPageConnections();

	void setUpTitlePageConnections();

	void setUpGeneralPageConnections();

private:

	Ui::uiPlotPropertiesWindow *ui;

	// todo: add this to the custom_customplot class
	QHash<QListWidgetItem *, QCPTextElement *> *graphTextElements = new QHash<QListWidgetItem *, QCPTextElement *>;

	QFontDialog *titleFontDialog = new QFontDialog(this);
	QColorDialog *titleColorDialog = new QColorDialog(this);

	QColorDialog *popUpColorDialog = new QColorDialog(this);
	QFontDialog *popUpFontDialog = new QFontDialog(this);
	QPenDialog *popUpPenDialog = new QPenDialog(this);
};


#endif //GUI_APP_PLOTPROPERTIESWINDOW_H
