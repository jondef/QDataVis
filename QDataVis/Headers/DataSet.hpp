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

	bool overrideGlobalPointDensity = false;
	unsigned int pointDensity = 1000;
	int minimumDomain = -10;
	int maximumDomain = 10;
};


#endif //QDATAVIS_GRAPH_H
