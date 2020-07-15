//
// Created by jon on 29.06.20.
//

#ifndef QDATAVIS_GRAPH_H
#define QDATAVIS_GRAPH_H

#include <qcustomplot.h>

#include <utility>
#include "BinaryTree.hpp"

struct DataSet {
	DataSet() = default;

	~DataSet() {
		delete listWidgetItem;
		delete graph;
		delete binaryTree;
	}

	void changeColor(QColor color) {
		this->color = std::move(color);
		// * update the color on the list widget item
		QPixmap pixmap = QPixmap(16, 16);
		pixmap.fill(this->color);
		this->listWidgetItem->setIcon(QIcon(pixmap));
		// * update the graph
		QPen graphPen;
		graphPen.setColor(this->color);
		graphPen.setWidthF(this->graphWidth); // between 1 and 2 acceptable (float/int)
		this->graph->setPen(graphPen);
//		this->graph->setBrush(QBrush(QColor(0, 0, 255, 20))); // set background
	}

	void configureListWidgetItem() {
		QVariant variant;
		variant.setValue(this);
		listWidgetItem->setData(Qt::UserRole, variant);
		listWidgetItem->setText(this->name);
	}

	QString name;
	QListWidgetItem *listWidgetItem;
	QCPGraph *graph;
	BinaryTree *binaryTree;
	QColor color;
	double graphWidth = 3;

	bool overrideGlobalPointDensity = false;
	unsigned int pointDensity = 1000;
	int minimumDomain = -10;
	int maximumDomain = 10;
};


#endif //QDATAVIS_GRAPH_H
