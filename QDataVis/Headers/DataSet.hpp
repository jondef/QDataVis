//
// Created by jon on 29.06.20.
//

#ifndef QDATAVIS_GRAPH_H
#define QDATAVIS_GRAPH_H

#include <qcustomplot.h>
#include "BinaryTree.hpp"

struct DataSet {
	DataSet() = default;

	~DataSet() {
		delete listWidgetItem;
		delete graph;
		delete binaryTree;
	}

	QString name;
	QListWidgetItem *listWidgetItem;
	QCPGraph *graph;
	BinaryTree *binaryTree;

	bool followGlobalPointDensity = true;
	int pointDensity = 1000;
};


#endif //QDATAVIS_GRAPH_H
