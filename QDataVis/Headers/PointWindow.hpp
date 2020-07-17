//
// Created by John on 30.11.2019.
//

#ifndef QDATAVIS_POINTWINDOW_HPP
#define QDATAVIS_POINTWINDOW_HPP

#include <QWidget>
#include <QtCore>
#include <QColorDialog>
#include <QFontDialog>
#include "ui_PointWindow.h"
#include "QCustomPlotCustom.hpp"


class PointWindow : public QDialog {
Q_OBJECT

public:
	explicit PointWindow(QWidget *parent = nullptr);

	~PointWindow() override;

	Ui::uiPointWindow *ui;

	DataSet *pDataSet = nullptr;
	QListWidgetItem *m_listWidgetItem = nullptr;

	void setGraph(QListWidgetItem *listWidgetItem);

	void saveGraph() const;

Q_SIGNALS:

	void currentGraphChanged();

};


#endif //QDATAVIS_POINTWINDOW_HPP

