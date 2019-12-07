//
// Created by John on 30.11.2019.
//

#ifndef QDATAVIS_POINTWINDOW_H
#define QDATAVIS_POINTWINDOW_H

#include <QWidget>
#include <QtCore>
#include <QColorDialog>
#include <QFontDialog>
#include "ui_PointWindow.h"
#include "QCustomPlot_custom.h"


class PointWindow : public QDialog {
Q_OBJECT

public:
	explicit PointWindow(QWidget *parent = nullptr);

	~PointWindow();

	Ui::uiPointWindow *ui;

	QCPGraph *m_graph = nullptr;
	QListWidgetItem *m_listWidgetItem = nullptr;

	void setGraph(QCPGraph *const pGraph, QListWidgetItem *listWidgetItem);

	void saveGraph();

Q_SIGNALS:

	void currentGraphChanged();

};


#endif //QDATAVIS_POINTWINDOW_H
